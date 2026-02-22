cmake_minimum_required(VERSION 3.15)
project(OnAirDeck VERSION 0.1.0 LANGUAGES C CXX)

# Generate compile_commands.json to help VS Code's IntelliSense resolve include paths
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add JUCE as subdirectory
add_subdirectory(JUCE)

juce_add_gui_app(OnAirDeck
    PRODUCT_NAME "OnAirDeck"
    VERSION "0.1.0"
    BUNDLE_ID "com.andpia.onairdeck"
)

file(GLOB_RECURSE OnAirDeck_sources CONFIGURE_DEPENDS
    "Source/*.cpp"
    "Source/*.h")

target_sources(OnAirDeck PRIVATE ${OnAirDeck_sources})

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
    juce::juce_recommended_warning_flags
)

target_compile_definitions(OnAirDeck PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=1
)

juce_generate_juce_header(OnAirDeck)

set_target_properties(OnAirDeck PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Use generator expressions for configuration-specific flags
target_compile_options(OnAirDeck PRIVATE
    $<$<CONFIG:Debug>:-g -O0 -fsanitize=address>
    $<$<CONFIG:Release>:-O3>
)
target_link_options(OnAirDeck PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)

# AppleClang specifics
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    target_compile_options(OnAirDeck PRIVATE -fcolor-diagnostics)
endif()
