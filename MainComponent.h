#pragma once

#include <JuceHeader.h>

class MainComponent  : public juce::Component,
                       public juce::Button::Listener,
                       public juce::Slider::Listener,
                       public juce::ComboBox::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sendCC(int chan, int cc, int val);
    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    juce::TextButton myButton;
    juce::Slider testSlider;
    juce::Label sliderLabel;
    juce::ComboBox midiOutputSelector;
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;

    void refreshMidiOutputs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

