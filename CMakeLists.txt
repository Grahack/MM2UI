cmake_minimum_required(VERSION 3.15)

project(MM2UI VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# Remplace par le chemin vers JUCE
set(JUCE_DIR ../JUCE)

add_subdirectory(${JUCE_DIR} JUCE)

juce_add_gui_app(MM2UI
    COMPANY_NAME "ProfGra"
)

juce_generate_juce_header(MM2UI)

target_sources(MM2UI PRIVATE
    Main.cpp
    MainComponent.cpp
    MainComponent.h
)

target_compile_definitions(MM2UI PRIVATE
    JUCE_APPLICATION_NAME_STRING="MM2UI"
    JUCE_APPLICATION_VERSION_STRING="0.1.0"
)

target_link_libraries(MM2UI PRIVATE
    juce::juce_gui_basics
    juce::juce_audio_devices
    curl
)

