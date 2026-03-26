#Requires -Version 5.1
<#
.SYNOPSIS
    Windows release build helper for OnAirDeck.

.DESCRIPTION
    Builds the frontend (vendor/web-ui) and then the native OnAirDeck application
    in Release mode using CMake/MSVC.  Produces a ZIP artifact containing the
    OnAirDeck.exe executable alongside the required WebUI/ folder.

    Mirrors the behaviour of scripts/release.sh for Unix, but works natively on
    Windows without a POSIX shell.

    The script automatically installs the Microsoft.Web.WebView2 NuGet SDK if it
    is not already present in the NuGet global packages cache.  WebView2 is
    required so that JUCE compiles with the modern Edge/Chromium browser backend
    (JUCE_USE_WIN_WEBVIEW2=1); without it JUCE falls back to Internet Explorer
    which cannot render modern React/Vite applications and shows a blank white page.

.PARAMETER FrontendDir
    Path to the frontend source directory.
    Default: vendor/web-ui (submodule) relative to the repository root.

.PARAMETER WebUiDistPath
    Path to a pre-built frontend dist/ folder.
    Default: <FrontendDir>/dist  (built by this script if not already present).

.PARAMETER BuildDir
    CMake build output directory.
    Default: out/build/release

.PARAMETER OutDir
    Directory where the final ZIP is written.
    Default: out

.PARAMETER WebView2Path
    Path to the build/native directory of the Microsoft.Web.WebView2 NuGet package
    (the directory that contains include/WebView2.h).
    If not provided the script searches the NuGet global packages cache and, if not
    found there, installs the latest stable version automatically.

.PARAMETER SkipFrontendBuild
    Skip the npm ci / npm run build step (use when dist/ is already present).

.EXAMPLE
    # Build everything from scratch:
    .\scripts\build-windows.ps1

.EXAMPLE
    # Use an existing dist/ from an external repo:
    .\scripts\build-windows.ps1 -WebUiDistPath C:\repos\on-air-deck-figma\dist

.EXAMPLE
    # Only re-run CMake + native build (frontend already built):
    .\scripts\build-windows.ps1 -SkipFrontendBuild

.EXAMPLE
    # Use a pre-installed WebView2 NuGet SDK:
    .\scripts\build-windows.ps1 -WebView2Path C:\webview2-sdk\Microsoft.Web.WebView2.1.0.2651.64\build\native
#>
[CmdletBinding()]
param(
    [string]$FrontendDir   = "",
    [string]$WebUiDistPath = "",
    [string]$BuildDir      = "",
    [string]$OutDir        = "",
    [string]$WebView2Path  = "",
    [switch]$SkipFrontendBuild
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ---------------------------------------------------------------------------
# Resolve repository root (the directory that contains CMakeLists.txt)
# ---------------------------------------------------------------------------
$RepoRoot = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path (Join-Path $RepoRoot "CMakeLists.txt"))) {
    Write-Error "Cannot locate repository root (CMakeLists.txt not found at '$RepoRoot'). Run this script from the repository or scripts/ directory."
    exit 1
}

# Defaults
if (-not $FrontendDir)   { $FrontendDir   = Join-Path $RepoRoot "vendor/web-ui" }
if (-not $WebUiDistPath) { $WebUiDistPath = Join-Path $FrontendDir "dist" }
if (-not $BuildDir)      { $BuildDir      = Join-Path $RepoRoot "out/build/release" }
if (-not $OutDir)        { $OutDir        = Join-Path $RepoRoot "out" }

Write-Host "=== OnAirDeck Windows Release Build ===" -ForegroundColor Cyan
Write-Host "Repository root : $RepoRoot"
Write-Host "Frontend dir    : $FrontendDir"
Write-Host "WebUI dist path : $WebUiDistPath"
Write-Host "CMake build dir : $BuildDir"
Write-Host "Output dir      : $OutDir"
Write-Host ""

# ---------------------------------------------------------------------------
# Check prerequisites
# ---------------------------------------------------------------------------
function Assert-Command([string]$Cmd) {
    if (-not (Get-Command $Cmd -ErrorAction SilentlyContinue)) {
        Write-Error "Required tool '$Cmd' was not found in PATH."
        exit 1
    }
}

if (-not $SkipFrontendBuild) {
    Assert-Command "node"
    Assert-Command "npm"

    $nodeVersion = (node -v) -replace '^v',''
    $nodeMajor   = [int]($nodeVersion -split '\.')[0]
    if ($nodeMajor -lt 20) {
        Write-Error "Node.js >= 20 is required (detected: v$nodeVersion)."
        exit 1
    }
    Write-Host "Node.js : v$nodeVersion"
    Write-Host "npm     : $(npm -v)"
}

Assert-Command "cmake"

# ---------------------------------------------------------------------------
# WebView2 SDK – required for the modern Edge/Chromium browser backend.
# Without WebView2 SDK JUCE compiles with the legacy IE backend, which cannot
# render modern React/Vite applications and will show a blank white page.
# ---------------------------------------------------------------------------
function Find-WebView2InNuGetCache {
    $nugetCacheDirs = @(
        "$env:USERPROFILE\.nuget\packages\microsoft.web.webview2",
        "$env:LOCALAPPDATA\NuGet\Cache\microsoft.web.webview2"
    )
    foreach ($cacheDir in $nugetCacheDirs) {
        if (Test-Path $cacheDir) {
            $candidate = Get-ChildItem $cacheDir -Directory |
                           Sort-Object Name -Descending |
                           Select-Object -First 1 -ExpandProperty FullName
            if ($candidate) {
                $nativePath = Join-Path $candidate "build\native"
                if (Test-Path (Join-Path $nativePath "include\WebView2.h")) {
                    return $nativePath
                }
            }
        }
    }
    return $null
}

$WebView2NativePath = $null
if ($WebView2Path) {
    # Explicit path provided via parameter
    if (-not (Test-Path (Join-Path $WebView2Path "include\WebView2.h"))) {
        Write-Error "WebView2.h not found under the provided -WebView2Path: '$WebView2Path'"
        exit 1
    }
    $WebView2NativePath = $WebView2Path
    Write-Host "WebView2 SDK : $WebView2NativePath (from -WebView2Path parameter)"
} else {
    # Search NuGet global packages cache
    $WebView2NativePath = Find-WebView2InNuGetCache
    if ($WebView2NativePath) {
        Write-Host "WebView2 SDK : $WebView2NativePath (found in NuGet cache)"
    } else {
        # Auto-install via NuGet
        Write-Host "WebView2 SDK not found in NuGet cache. Installing via nuget..." -ForegroundColor Yellow
        if (-not (Get-Command "nuget" -ErrorAction SilentlyContinue)) {
            Write-Error "nuget CLI not found. Install it from https://www.nuget.org/downloads or install the WebView2 SDK manually and pass -WebView2Path."
            exit 1
        }
        $wv2TempDir = Join-Path $env:TEMP "webview2-sdk"
        nuget install Microsoft.Web.WebView2 -OutputDirectory $wv2TempDir -NonInteractive
        if ($LASTEXITCODE -ne 0) {
            Write-Error "nuget install Microsoft.Web.WebView2 failed (exit code $LASTEXITCODE)."
            exit 1
        }
        $WebView2NativePath = Get-ChildItem "$wv2TempDir\Microsoft.Web.WebView2.*\build\native" -Directory |
                                Sort-Object Name -Descending |
                                Select-Object -First 1 -ExpandProperty FullName
        if (-not $WebView2NativePath -or -not (Test-Path "$WebView2NativePath\include\WebView2.h")) {
            Write-Error "WebView2 SDK installation failed: WebView2.h not found under '$wv2TempDir'."
            exit 1
        }
        Write-Host "WebView2 SDK : $WebView2NativePath (installed to $wv2TempDir)"
    }
}
Write-Host ""

# ---------------------------------------------------------------------------
# Step 1 – Build frontend
# ---------------------------------------------------------------------------
if (-not $SkipFrontendBuild) {
    Write-Host ""
    Write-Host "[1/4] Building frontend in '$FrontendDir'" -ForegroundColor Green
    if (-not (Test-Path $FrontendDir)) {
        Write-Error "Frontend directory not found: '$FrontendDir'. Initialize submodules: git submodule update --init --recursive"
        exit 1
    }
    Push-Location $FrontendDir
    try {
        if (Test-Path "package-lock.json") {
            npm ci
        } else {
            npm install
        }
        npm run build
    } finally {
        Pop-Location
    }
} else {
    Write-Host ""
    Write-Host "[1/4] Skipping frontend build (-SkipFrontendBuild)" -ForegroundColor Yellow
}

# Verify dist exists
if (-not (Test-Path (Join-Path $WebUiDistPath "index.html"))) {
    Write-Error "Frontend dist not found at '$WebUiDistPath\index.html'. Build the frontend first or provide -WebUiDistPath."
    exit 1
}
Write-Host "Frontend dist OK: $WebUiDistPath"

# ---------------------------------------------------------------------------
# Step 2 – CMake configure
# ---------------------------------------------------------------------------
Write-Host ""
Write-Host "[2/4] Configuring CMake in '$BuildDir'" -ForegroundColor Green
$distPathFwd = $WebUiDistPath  -replace '\\','/'
$wv2PathFwd  = $WebView2NativePath -replace '\\','/'
Write-Host "WEBUI_DIST_PATH = $distPathFwd"
Write-Host "WEBVIEW2_PATH   = $wv2PathFwd"
Push-Location $RepoRoot
try {
    cmake -S . -B $BuildDir `
        -DCMAKE_BUILD_TYPE=Release `
        -DWEBUI_DIST_PATH="$distPathFwd" `
        -DWEBVIEW2_PATH="$wv2PathFwd"
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
} finally {
    Pop-Location
}

# ---------------------------------------------------------------------------
# Step 3 – Build
# ---------------------------------------------------------------------------
Write-Host ""
Write-Host "[3/4] Building Release target" -ForegroundColor Green
cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# ---------------------------------------------------------------------------
# Step 4 – Collect artifacts and zip
# ---------------------------------------------------------------------------
Write-Host ""
Write-Host "[4/4] Packaging artifacts" -ForegroundColor Green

$StagingDir = Join-Path $OutDir "staging/windows-release"
New-Item -ItemType Directory -Force -Path $StagingDir | Out-Null

# Locate OnAirDeck.exe
$ExeCandidates = @(
    (Join-Path $BuildDir "OnAirDeck_artefacts/Release/OnAirDeck.exe"),
    (Join-Path $BuildDir "Release/OnAirDeck.exe")
)
$ExeSrc = $null
foreach ($p in $ExeCandidates) {
    if (Test-Path $p) { $ExeSrc = $p; break }
}
if (-not $ExeSrc) {
    Write-Error "OnAirDeck.exe not found after build. Searched:`n  $($ExeCandidates -join "`n  ")"
    exit 1
}
Copy-Item $ExeSrc -Destination $StagingDir
Write-Host "Copied: $ExeSrc"

# Locate WebUI/ folder
$WebUICandidates = @(
    (Join-Path $BuildDir "OnAirDeck_artefacts/Release/WebUI"),
    (Join-Path $BuildDir "WebUI")
)
$WebUISrc = $null
foreach ($p in $WebUICandidates) {
    if (Test-Path $p) { $WebUISrc = $p; break }
}
if (-not $WebUISrc) {
    Write-Error "WebUI/ folder not found in build output. Searched:`n  $($WebUICandidates -join "`n  ")"
    exit 1
}
Copy-Item $WebUISrc -Destination $StagingDir -Recurse
Write-Host "Copied: $WebUISrc"

# Create ZIP
$ZipPath = Join-Path $OutDir "OnAirDeck-windows-release.zip"
if (Test-Path $ZipPath) { Remove-Item $ZipPath }
Compress-Archive -Path "$StagingDir/*" -DestinationPath $ZipPath -Force
Write-Host ""
Write-Host "=== Build complete ===" -ForegroundColor Cyan
Write-Host "Artifact: $ZipPath"
Write-Host "Contents:"
Get-ChildItem $StagingDir -Recurse | Select-Object FullName
