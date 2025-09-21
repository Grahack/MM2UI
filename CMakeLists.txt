cmake_minimum_required(VERSION 3.15)

project(GriMidui VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# Remplace par le chemin vers JUCE
set(JUCE_DIR ../JUCE)

add_subdirectory(${JUCE_DIR} JUCE)

juce_add_gui_app(GriMidui
    COMPANY_NAME "ProfGra"
)

juce_generate_juce_header(GriMidui)

target_sources(GriMidui PRIVATE
    Main.cpp
    MainComponent.cpp
    MainComponent.h
)

target_compile_definitions(GriMidui PRIVATE
    JUCE_APPLICATION_NAME_STRING="GriMidui"
    JUCE_APPLICATION_VERSION_STRING="0.1.0"
)

target_link_libraries(GriMidui PRIVATE
    juce::juce_gui_basics
    juce::juce_audio_devices
    curl
)

