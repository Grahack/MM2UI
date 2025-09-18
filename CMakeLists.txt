cmake_minimum_required(VERSION 3.15)

project(MyJuceApp VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

# Remplace par le chemin vers JUCE
set(JUCE_DIR ../JUCE)

add_subdirectory(${JUCE_DIR} JUCE)

juce_add_gui_app(MyJuceApp
    COMPANY_NAME "MonApp"
)

juce_generate_juce_header(MyJuceApp)

target_sources(MyJuceApp PRIVATE
    Main.cpp
    MainComponent.cpp
    MainComponent.h
)

target_compile_definitions(MyJuceApp PRIVATE
    JUCE_APPLICATION_NAME_STRING="MyJuceApp"
    JUCE_APPLICATION_VERSION_STRING="0.1.0"
)

target_link_libraries(MyJuceApp PRIVATE
    juce::juce_gui_basics
    curl
)

