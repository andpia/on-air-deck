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

target_compile_definitions(OnAirDeck PRIVATE
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=${USE_CURL}
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
if(WEBUI_DIST_PATH AND EXISTS "${WEBUI_DIST_PATH}")
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
    message(STATUS "WebUI dist: ${WEBUI_DIST_PATH} -> copied post-build")
else()
    if(WEBUI_DIST_PATH)
        message(WARNING "WEBUI_DIST_PATH is set but the path does not exist: ${WEBUI_DIST_PATH}")
    else()
        message(STATUS "WebUI dist: not configured (set -DWEBUI_DIST_PATH=<path> to enable)")
    endif()
endif()

message(STATUS "WebUI dev server URL: ${WEBUI_DEV_SERVER_URL} (Debug builds only)")
message(STATUS "Unity Build: ${ONAIRDECK_UNITY_BUILD}")

