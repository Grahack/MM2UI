#pragma once

#include <JuceHeader.h>
#include "GlideControl.h"

class MainComponent  : public juce::Component,
                       public juce::Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;

private:
    juce::TextButton myButton { "Clique-moi" };
    juce::Label statusLabel;
    GlideControl glideControl { "Glisser ici" };

    std::unique_ptr<juce::MidiOutput> midiOut;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

