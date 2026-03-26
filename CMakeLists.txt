cmake_minimum_required(VERSION 3.22)

# Centralized version
set(ONAIRDECK_VERSION "0.1.0")
set(ONAIRDECK_COMPANY_NAME "BremaLabs")

project(OnAirDeck VERSION ${ONAIRDECK_VERSION} LANGUAGES C CXX)

message(STATUS "OnAirDeck version: ${ONAIRDECK_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME}")

# Generate compile_commands.json to help VS Code's IntelliSense resolve include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configurable options
option(ONAIRDECK_USE_CURL "Enable CURL support for HTTP functionality" ON)
option(ONAIRDECK_ENABLE_SANITIZERS "Enable AddressSanitizer in Debug builds (non-MSVC only)" ON)
option(ONAIRDECK_UNITY_BUILD "Enable Unity Build to speed up compilation" OFF)
option(ONAIRDECK_ALLOW_MISSING_WEBUI
    "Allow Release builds to proceed even when no Web UI dist/ folder is found. OFF by default so Release packages always include the Web UI."
    OFF)

# Web UI configuration
# Path to the pre-built Vite dist/ folder.
# If not set explicitly and the `vendor/web-ui` submodule has a built dist/, it is auto-detected.
# You can always override it manually, for example:
#   cmake -DWEBUI_DIST_PATH=/path/to/on-air-deck-figma/dist ...
set(WEBUI_DIST_PATH "" CACHE PATH
    "Path to the Web UI dist/ folder produced by 'npm run build'")

set(ONAIRDECK_WEBUI_SUBMODULE_DIR "${CMAKE_SOURCE_DIR}/vendor/web-ui")
if(NOT WEBUI_DIST_PATH)
    set(_WEBUI_DIST_AUTODETECT "${ONAIRDECK_WEBUI_SUBMODULE_DIR}/dist")
    if(EXISTS "${_WEBUI_DIST_AUTODETECT}")
        set(WEBUI_DIST_PATH "${_WEBUI_DIST_AUTODETECT}")
        message(STATUS "WebUI dist: auto-detected from submodule at ${WEBUI_DIST_PATH}")
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

# On Windows, locate the WebView2 SDK so that the modern Chromium-based
# backend and the ResourceProvider API (JUCE 8) are available.
# In Release builds this is required – without WebView2 JUCE falls back to the
# legacy IE backend which cannot render modern React/Vite applications (white page).
# Pass -DWEBVIEW2_PATH=<dir>/build/native or let the scripts install it automatically.
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/vendor/JUCE/extras/Build/CMake")
    find_package(WebView2 QUIET)
    if(WebView2_FOUND)
        set(USE_WEBVIEW2 ON)
        message(STATUS "WebView2 SDK: found -> enabling JUCE_USE_WIN_WEBVIEW2")
        target_include_directories(OnAirDeck PRIVATE "${WebView2_include_dir}")
        target_link_libraries(OnAirDeck PRIVATE "${WebView2_library}")
    else()
        set(USE_WEBVIEW2 OFF)
        if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT ONAIRDECK_ALLOW_MISSING_WEBUI)
            message(FATAL_ERROR
                "Windows Release build requires the Microsoft WebView2 SDK.\n"
                "Without it JUCE falls back to the legacy Internet Explorer backend, which cannot\n"
                "render modern React/Vite applications and will show a blank white page at runtime.\n"
                "\n"
                "Install the NuGet package and pass the build/native path to CMake:\n"
                "  nuget install Microsoft.Web.WebView2 -OutputDirectory <dir>\n"
                "  cmake ... -DWEBVIEW2_PATH=<dir>\\Microsoft.Web.WebView2.<VERSION>\\build\\native\n"
                "\n"
                "Or use the helper script which installs it automatically:\n"
                "  .\\scripts\\build-windows.ps1\n"
                "\n"
                "To allow a Release build without WebView2 (results in blank page at runtime):\n"
                "  -DONAIRDECK_ALLOW_MISSING_WEBUI=ON\n"
            )
        else()
            message(STATUS "WebView2 SDK: not found -> using legacy IE browser backend (resource provider unavailable)")
        endif()
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
message(STATUS "WebUI dist path (resolved): ${WEBUI_DIST_PATH}")
if(WEBUI_DIST_PATH AND EXISTS "${WEBUI_DIST_PATH}")
    message(STATUS "WebUI dist: ${WEBUI_DIST_PATH} -> will be copied post-build")
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
        # Install rule so CPack/install packages include WebUI/ next to the executable.
        install(DIRECTORY "${WEBUI_DIST_PATH}/"
            DESTINATION "WebUI"
            COMPONENT Runtime
        )
    endif()
else()
    if(WEBUI_DIST_PATH)
        # Path was set explicitly but does not exist — always fatal.
        message(FATAL_ERROR
            "WEBUI_DIST_PATH is set but the path does not exist: ${WEBUI_DIST_PATH}\n"
            "Build the frontend first:\n"
            "  cd vendor/web-ui && npm install && npm run build\n"
            "Or pass the correct path: -DWEBUI_DIST_PATH=<path-to-dist>"
        )
    else()
        # Path was not set and auto-detection found nothing.
        if(CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT ONAIRDECK_ALLOW_MISSING_WEBUI)
            message(FATAL_ERROR
                "Release build requires a pre-built Web UI dist/ folder, but none was found.\n"
                "Expected location: vendor/web-ui/dist\n"
                "Build the frontend first:\n"
                "  cd vendor/web-ui && npm install && npm run build\n"
                "Or point CMake at a pre-built dist/:\n"
                "  -DWEBUI_DIST_PATH=<path-to-dist>\n"
                "To allow a Release build without the Web UI (not recommended):\n"
                "  -DONAIRDECK_ALLOW_MISSING_WEBUI=ON"
            )
        else()
            message(STATUS
                "WebUI dist: not configured (no dist/ found).\n"
                "Debug builds will fall back to the Vite dev server.\n"
                "Default expected path: vendor/web-ui/dist\n"
                "Override with: -DWEBUI_DIST_PATH=<path-to-dist>"
            )
        endif()
    endif()
endif()

message(STATUS "WebUI dev server URL: ${WEBUI_DEV_SERVER_URL} (Debug builds only)")
message(STATUS "Unity Build: ${ONAIRDECK_UNITY_BUILD}")

