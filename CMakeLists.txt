cmake_minimum_required(VERSION 3.22)

# Centralized version
set(ONAIRDECK_VERSION "0.1.0")
set(ONAIRDECK_COMPANY_NAME "BremaStack")

project(OnAirDeck VERSION ${ONAIRDECK_VERSION} LANGUAGES C CXX)

message(STATUS "OnAirDeck version: ${ONAIRDECK_VERSION}")
if(CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Build mode: multi-config (${CMAKE_CONFIGURATION_TYPES})")
else()
    message(STATUS "Build mode: single-config (${CMAKE_BUILD_TYPE})")
endif()
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Generate compile_commands.json to help VS Code's IntelliSense resolve include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configurable options
option(ONAIRDECK_USE_CURL "Enable CURL support for HTTP functionality" ON)
option(ONAIRDECK_ENABLE_SANITIZERS "Enable AddressSanitizer in Debug builds (non-MSVC only)" ON)
option(ONAIRDECK_UNITY_BUILD "Enable Unity Build to speed up compilation" OFF)

# Web UI configuration
# Path to the pre-built Vite dist/ folder.
# If not set explicitly and the `vendor/web-ui` submodule has a built dist/, it is auto-detected.
# You can always override it manually, for example:
#   cmake -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist ...
set(WEBUI_DIST_PATH "" CACHE PATH
    "Path to the Web UI dist/ folder produced by 'npm run build'")
option(ONAIRDECK_BUILD_BUNDLED_WEBUI
    "Automatically build vendor/web-ui/dist when the bundled frontend sources are available"
    ON)

set(ONAIRDECK_WEBUI_SUBMODULE_DIR "${CMAKE_SOURCE_DIR}/vendor/web-ui")
set(_ONAIRDECK_BUNDLED_WEBUI_DIST_DIR "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/dist")
set(_ONAIRDECK_BUNDLED_WEBUI_INDEX "${_ONAIRDECK_BUNDLED_WEBUI_DIST_DIR}/index.html")
set(_ONAIRDECK_HAS_BUNDLED_WEBUI_SOURCES OFF)
if(EXISTS "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/package.json")
    set(_ONAIRDECK_HAS_BUNDLED_WEBUI_SOURCES ON)
endif()

if(NOT WEBUI_DIST_PATH)
    if(_ONAIRDECK_HAS_BUNDLED_WEBUI_SOURCES)
        set(WEBUI_DIST_PATH "${_ONAIRDECK_BUNDLED_WEBUI_DIST_DIR}")
        if(EXISTS "${_ONAIRDECK_BUNDLED_WEBUI_INDEX}")
            message(STATUS "WebUI dist: auto-detected from submodule at ${WEBUI_DIST_PATH}")
        else()
            message(STATUS "WebUI dist: will be built from bundled frontend sources at ${ONAIRDECK_WEBUI_SUBMODULE_DIR}")
        endif()
    endif()
endif()

# URL of the Vite dev server, loaded in Debug builds only.
set(WEBUI_DEV_SERVER_URL "http://localhost:5173" CACHE STRING
    "Vite dev server URL (used in Debug builds for hot-reload support)")

# Ensure JUCE submodule is available with a clear actionable error.
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/vendor/JUCE/CMakeLists.txt")
    message(FATAL_ERROR
        "JUCE submodule not found at vendor/JUCE.\n"
        "Initialize submodules and retry:\n"
        "  git submodule update --init --recursive"
    )
endif()

# Add JUCE as subdirectory
add_subdirectory(vendor/JUCE)

juce_add_gui_app(OnAirDeck
    PRODUCT_NAME "OnAirDeck"
    VERSION "${ONAIRDECK_VERSION}"
    COMPANY_NAME "${ONAIRDECK_COMPANY_NAME}"
    BUNDLE_ID "com.andpia.onairdeck"
    NEEDS_CURL TRUE
    NEEDS_WEB_BROWSER TRUE
)

# Use `src` directory and auto-generate source list so new files are picked up
file(GLOB_RECURSE ONAIRDECK_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/src/*.mm
    ${CMAKE_SOURCE_DIR}/src/*.m
    ${CMAKE_SOURCE_DIR}/src/*.hpp
    ${CMAKE_SOURCE_DIR}/src/*.h
)
if(NOT ONAIRDECK_SOURCES)
    message(WARNING "No source files found in ${CMAKE_CURRENT_SOURCE_DIR}/src")
endif()
target_sources(OnAirDeck PRIVATE ${ONAIRDECK_SOURCES})

# Include local `src` headers for the target and request C++17 via compile features
target_include_directories(OnAirDeck PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(OnAirDeck PRIVATE cxx_std_17)

# CURL support: only if requested and available, and not on Windows
if(ONAIRDECK_USE_CURL AND NOT CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(CURL QUIET)
    if(CURL_FOUND)
        set(USE_CURL ON)
        message(STATUS "CURL support: enabled")
    else()
        set(USE_CURL OFF)
        message(STATUS "CURL support: disabled (CURL not found)")
    endif()
else()
    set(USE_CURL OFF)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(STATUS "CURL support: disabled (Windows)")
    else()
        message(STATUS "CURL support: disabled (ONAIRDECK_USE_CURL=OFF)")
    endif()
endif()

# Linux-specific: validate that pkg-config can find required JUCE dependencies
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Linux detected: checking for JUCE dependencies via pkg-config...")
    find_package(PkgConfig REQUIRED)

    # Check for ALSA (audio)
    pkg_check_modules(ALSA QUIET alsa)
    if(NOT ALSA_FOUND)
        message(SEND_ERROR
            "ALSA development package not found.\n"
            "On Ubuntu 24.04 LTS, install with:\n"
            "  sudo apt-get install libasound2-dev\n"
            "Or run the setup script:\n"
            "  bash scripts/setup-linux.sh"
        )
    endif()

    # Check for OpenGL (graphics)
    pkg_check_modules(GL QUIET gl)
    if(NOT GL_FOUND)
        message(SEND_ERROR
            "OpenGL development package not found.\n"
            "On Ubuntu 24.04 LTS, install with:\n"
            "  sudo apt-get install libgl-dev\n"
            "Or run the setup script:\n"
            "  bash scripts/setup-linux.sh"
        )
    endif()

    # Check for WebKit2GTK and GTK3 (browser component + GUI)
    pkg_check_modules(WEBKIT2GTK QUIET webkit2gtk-4.1)
    pkg_check_modules(GTK3 QUIET gtk+-x11-3.0)
    if(NOT WEBKIT2GTK_FOUND OR NOT GTK3_FOUND)
        message(SEND_ERROR
            "GTK3 or WebKit2GTK development packages not found.\n"
            "On Ubuntu 24.04 LTS, install with:\n"
            "  sudo apt-get install libgtk-3-dev libwebkit2gtk-4.1-dev\n"
            "Or run the setup script:\n"
            "  bash scripts/setup-linux.sh"
        )
    endif()
endif()

target_link_libraries(OnAirDeck PRIVATE
    juce::juce_core
    juce::juce_audio_basics
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_events
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)

# Warning flags only for Debug builds to avoid noise in Release
target_link_libraries(OnAirDeck PRIVATE
    $<$<CONFIG:Debug>:juce::juce_recommended_warning_flags>
)

if(USE_CURL)
    target_link_libraries(OnAirDeck PRIVATE CURL::libcurl)
endif()

# On Windows, try to locate the WebView2 SDK so that the modern Chromium-based
# backend and the ResourceProvider API (JUCE 8) are available.  The SDK is NOT
# required; when it is absent the project falls back to the legacy IE backend
# which still compiles and runs, just without the resource-provider feature.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/JUCE/extras/Build/CMake")
    # Allow the NuGet package location to be supplied via an environment variable
    # (e.g. set by CI after nuget install) without forcing callers to repeat it on
    # the cmake command line.  Use FORCE so that JUCE's own cache initialisation
    # (which pre-populates the variable with OFF) does not shadow the env value.
    if(DEFINED ENV{JUCE_WEBVIEW2_PACKAGE_LOCATION}
       AND NOT "$ENV{JUCE_WEBVIEW2_PACKAGE_LOCATION}" STREQUAL "")
        set(JUCE_WEBVIEW2_PACKAGE_LOCATION "$ENV{JUCE_WEBVIEW2_PACKAGE_LOCATION}" CACHE PATH
            "Directory containing the Microsoft.Web.WebView2 NuGet package folder" FORCE)
        message(STATUS "WebView2: using JUCE_WEBVIEW2_PACKAGE_LOCATION from environment: ${JUCE_WEBVIEW2_PACKAGE_LOCATION}")
    endif()
    # If still not set (or was set to a falsy placeholder), probe common NuGet
    # package cache locations used by nuget.exe, dotnet, and PowerShell
    # PackageManagement so developers do not need to set the variable manually.
    if(NOT JUCE_WEBVIEW2_PACKAGE_LOCATION)
        foreach(_probe_dir
            "$ENV{LOCALAPPDATA}/PackageManagement/NuGet/Packages"
            "$ENV{USERPROFILE}/.nuget/packages"
        )
            if(EXISTS "${_probe_dir}")
                file(GLOB _wv2_probe "${_probe_dir}/*Microsoft.Web.WebView2*")
                if(_wv2_probe)
                    set(JUCE_WEBVIEW2_PACKAGE_LOCATION "${_probe_dir}" CACHE PATH
                        "Directory containing the Microsoft.Web.WebView2 NuGet package folder" FORCE)
                    message(STATUS "WebView2: auto-detected NuGet packages at ${JUCE_WEBVIEW2_PACKAGE_LOCATION}")
                    break()
                endif()
            endif()
        endforeach()
    endif()
    find_package(WebView2 QUIET)
    if(WebView2_FOUND)
        set(USE_WEBVIEW2 ON)
        message(STATUS "WebView2 SDK: found -> enabling JUCE_USE_WIN_WEBVIEW2")
        target_include_directories(OnAirDeck PRIVATE "${WebView2_include_dir}")
        target_link_libraries(OnAirDeck PRIVATE "${WebView2_library}")
    else()
        set(USE_WEBVIEW2 OFF)
        message(WARNING
            "WebView2 SDK not found. The app will use the legacy IE backend, which does not support "
            "modern JavaScript (ES modules) and will show a blank white page at runtime.\n"
            "Install the Microsoft.Web.WebView2 NuGet package so CMake can detect it:\n"
            "  Register-PackageSource -Name nuget -ProviderName NuGet "
            "-Location https://www.nuget.org/api/v2 -Trusted -Force\n"
            "  Install-Package Microsoft.Web.WebView2 -Scope CurrentUser -Force\n"
            "See docs/build-and-run.md for details."
        )
    endif()
else()
    set(USE_WEBVIEW2 OFF)
endif()

target_compile_definitions(OnAirDeck PRIVATE
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=${USE_CURL}
    JUCE_USE_WIN_WEBVIEW2=$<BOOL:${USE_WEBVIEW2}>
    ONAIRDECK_VERSION="${ONAIRDECK_VERSION}"
    ONAIRDECK_COMPANY_NAME="${ONAIRDECK_COMPANY_NAME}"
    WEBUI_DEV_SERVER_URL="${WEBUI_DEV_SERVER_URL}"
)

juce_generate_juce_header(OnAirDeck)

set_target_properties(OnAirDeck PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    UNITY_BUILD ${ONAIRDECK_UNITY_BUILD}
)

set(_ONAIRDECK_WEBUI_BUILD_TARGET_ENABLED OFF)
if(ONAIRDECK_BUILD_BUNDLED_WEBUI
   AND _ONAIRDECK_HAS_BUNDLED_WEBUI_SOURCES
   AND WEBUI_DIST_PATH STREQUAL "${_ONAIRDECK_BUNDLED_WEBUI_DIST_DIR}")
    find_program(ONAIRDECK_NPM_EXECUTABLE NAMES npm npm.cmd)

    if(ONAIRDECK_NPM_EXECUTABLE)
        set(_ONAIRDECK_WEBUI_MANIFESTS
            "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/package.json"
            "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/index.html"
            "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/vite.config.ts"
        )
        if(EXISTS "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/package-lock.json")
            list(APPEND _ONAIRDECK_WEBUI_MANIFESTS "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/package-lock.json")
            set(_ONAIRDECK_WEBUI_INSTALL_ARGS ci)
        else()
            set(_ONAIRDECK_WEBUI_INSTALL_ARGS install)
        endif()
        if(EXISTS "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/postcss.config.mjs")
            list(APPEND _ONAIRDECK_WEBUI_MANIFESTS "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/postcss.config.mjs")
        endif()

        file(GLOB_RECURSE ONAIRDECK_WEBUI_BUILD_INPUTS CONFIGURE_DEPENDS
            "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/src/*"
            "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/guidelines/*"
        )

        set(_ONAIRDECK_WEBUI_INSTALL_STAMP "${CMAKE_CURRENT_BINARY_DIR}/webui-node-modules.stamp")

        add_custom_command(
            OUTPUT "${_ONAIRDECK_WEBUI_INSTALL_STAMP}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${ONAIRDECK_WEBUI_SUBMODULE_DIR}"
                "${ONAIRDECK_NPM_EXECUTABLE}" ${_ONAIRDECK_WEBUI_INSTALL_ARGS}
            COMMAND ${CMAKE_COMMAND} -E touch "${_ONAIRDECK_WEBUI_INSTALL_STAMP}"
            DEPENDS ${_ONAIRDECK_WEBUI_MANIFESTS}
            COMMENT "Installing bundled Web UI dependencies"
            VERBATIM
        )

        add_custom_command(
            OUTPUT "${_ONAIRDECK_BUNDLED_WEBUI_INDEX}"
            COMMAND ${CMAKE_COMMAND} -E chdir "${ONAIRDECK_WEBUI_SUBMODULE_DIR}"
                "${ONAIRDECK_NPM_EXECUTABLE}" run build -- --base=./
            DEPENDS "${_ONAIRDECK_WEBUI_INSTALL_STAMP}" ${_ONAIRDECK_WEBUI_MANIFESTS} ${ONAIRDECK_WEBUI_BUILD_INPUTS}
            COMMENT "Building bundled Web UI"
            VERBATIM
        )

        add_custom_target(OnAirDeckWebUI DEPENDS "${_ONAIRDECK_BUNDLED_WEBUI_INDEX}")
        add_dependencies(OnAirDeck OnAirDeckWebUI)
        set(_ONAIRDECK_WEBUI_BUILD_TARGET_ENABLED ON)
        message(STATUS "WebUI build: enabled for bundled frontend sources")
    else()
        message(WARNING
            "npm was not found in PATH, so CMake cannot auto-build vendor/web-ui/dist.\n"
            "Install Node.js and npm, or pass -DWEBUI_DIST_PATH=<path-to-dist>."
        )
    endif()
endif()

# Configuration-specific compiler flags
if(NOT MSVC)
    target_compile_options(OnAirDeck PRIVATE
        $<$<CONFIG:Release>:-O3>
    )
    # Sanitizers only in Debug when enabled
    if(ONAIRDECK_ENABLE_SANITIZERS)
        target_compile_options(OnAirDeck PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
        target_link_options(OnAirDeck PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
        message(STATUS "AddressSanitizer: enabled in Debug builds")
    else()
        message(STATUS "AddressSanitizer: disabled")
    endif()
    target_compile_options(OnAirDeck PRIVATE $<$<CONFIG:Debug>:-g -O0>)
else()
    target_compile_options(OnAirDeck PRIVATE
        $<$<CONFIG:Debug>:/Zi>
        $<$<CONFIG:Release>:/O2>
    )
endif()

# AppleClang specifics
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    target_compile_options(OnAirDeck PRIVATE -fcolor-diagnostics)
endif()

# Post-build: copy the pre-built Web UI into the output directory.
# Set WEBUI_DIST_PATH to the dist/ folder produced by `npm run build`.
set(_ONAIRDECK_IS_RELEASE_CONFIG OFF)
set(_ONAIRDECK_IS_MULTICONFIG OFF)
if(CMAKE_CONFIGURATION_TYPES)
    set(_ONAIRDECK_IS_MULTICONFIG ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(_ONAIRDECK_IS_RELEASE_CONFIG ON)
endif()

set(_ONAIRDECK_WEBUI_DIST_READY OFF)
if(WEBUI_DIST_PATH AND EXISTS "${WEBUI_DIST_PATH}")
    set(_ONAIRDECK_WEBUI_DIST_READY ON)
elseif(_ONAIRDECK_WEBUI_BUILD_TARGET_ENABLED)
    set(_ONAIRDECK_WEBUI_DIST_READY ON)
endif()

if(_ONAIRDECK_WEBUI_DIST_READY)
    if(APPLE)
        # macOS: copy into the .app bundle's Resources folder.
        add_custom_command(TARGET OnAirDeck POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WEBUI_DIST_PATH}"
                "$<TARGET_BUNDLE_DIR:OnAirDeck>/Contents/Resources/WebUI"
            COMMENT "Copying Web UI to OnAirDeck.app/Contents/Resources/WebUI"
        )
    else()
        # Windows / Linux: copy next to the executable.
        add_custom_command(TARGET OnAirDeck POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${WEBUI_DIST_PATH}"
                "$<TARGET_FILE_DIR:OnAirDeck>/WebUI"
            COMMENT "Copying Web UI to WebUI/ next to executable"
        )
    endif()
    if(_ONAIRDECK_WEBUI_BUILD_TARGET_ENABLED)
        message(STATUS "WebUI dist: ${WEBUI_DIST_PATH} -> built and copied post-build")
    else()
        message(STATUS "WebUI dist: ${WEBUI_DIST_PATH} -> copied post-build")
    endif()
else()
    if(WEBUI_DIST_PATH)
        if(_ONAIRDECK_IS_RELEASE_CONFIG)
            message(FATAL_ERROR
                "WEBUI_DIST_PATH is set but the path does not exist: ${WEBUI_DIST_PATH}\n"
                "Release builds require a valid frontend dist folder.\n"
                "Build the frontend first, for example:\n"
                "  cd vendor/web-ui && npm install && npm run build"
            )
        else()
            message(WARNING
                "WEBUI_DIST_PATH is set but the path does not exist: ${WEBUI_DIST_PATH}\n"
                "Build the frontend first, for example:\n"
                "  cd vendor/web-ui && npm install && npm run build"
            )
        endif()
    else()
        if(_ONAIRDECK_IS_RELEASE_CONFIG)
            message(FATAL_ERROR
                "WebUI dist: not configured.\n"
                "Release builds require a pre-built dist folder.\n"
                "Default expected path: vendor/web-ui/dist\n"
                "Override with: -DWEBUI_DIST_PATH=<path-to-dist>"
            )
        elseif(_ONAIRDECK_IS_MULTICONFIG)
            message(WARNING
                "WebUI dist: not configured (multi-config generator detected).\n"
                "Release builds require a pre-built dist folder.\n"
                "Default expected path: vendor/web-ui/dist\n"
                "Override with: -DWEBUI_DIST_PATH=<path-to-dist>\n"
                "Build the frontend before invoking 'cmake --build --config Release'."
            )
        else()
            message(STATUS
                "WebUI dist: not configured.\n"
                "Release builds should provide a pre-built dist folder.\n"
                "Default expected path: vendor/web-ui/dist\n"
                "Override with: -DWEBUI_DIST_PATH=<path-to-dist>"
            )
        endif()
    endif()
endif()

message(STATUS "WebUI dev server URL: ${WEBUI_DEV_SERVER_URL} (Debug builds only)")
message(STATUS "Unity Build: ${ONAIRDECK_UNITY_BUILD}")

