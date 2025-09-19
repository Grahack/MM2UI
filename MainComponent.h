#pragma once

#include <JuceHeader.h>
#include "GlideControl.h"

class MainComponent  : public juce::Component,
                       public juce::ToggleButton::Listener,
                       public juce::ComboBox::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    juce::TextButton myButton { "Clique-moi" };
    juce::Label statusLabel;
    GlideControl glideControl { "Glisser ici" };
    juce::ComboBox midiOutputSelector;
    juce::Label midiOutputLabel { {}, "Sortie MIDI :" };
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;

    void refreshMidiOutputs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

