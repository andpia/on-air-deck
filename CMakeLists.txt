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

# Add JUCE as subdirectory
add_subdirectory(JUCE)

juce_add_gui_app(OnAirDeck
    PRODUCT_NAME "OnAirDeck"
    VERSION "${ONAIRDECK_VERSION}"
    COMPANY_NAME "${ONAIRDECK_COMPANY_NAME}"
    BUNDLE_ID "com.andpia.onairdeck"
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
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=${USE_CURL}
    ONAIRDECK_VERSION="${ONAIRDECK_VERSION}"
    ONAIRDECK_COMPANY_NAME="${ONAIRDECK_COMPANY_NAME}"
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

message(STATUS "Unity Build: ${ONAIRDECK_UNITY_BUILD}")
