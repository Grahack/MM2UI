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
    void sendCC(int chan, int cc, int val);
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

private:
    juce::TextButton myButton {};
    juce::Label statusLabel;
    GlideControl glideControl { "OSC 1"};
    juce::ComboBox midiOutputSelector;
    juce::Label midiOutputLabel { {}, "MIDI out:" };
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;

    void refreshMidiOutputs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

