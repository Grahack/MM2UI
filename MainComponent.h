#pragma once

#include <map>
#include <JuceHeader.h>

using namespace std;

class MainComponent  : public juce::Component,
                       public juce::ComboBox::Listener,
                       public juce::Button::Listener,
                       public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void sendCC(int chan, int cc, int val);

private:
    // MIDI channel
    int channel;
    juce::ComboBox channelSelector;
    // MIDI out
    juce::ComboBox midiOutputSelector;
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;
    juce::TextButton refreshButton;
    // controllers section
    static const int slidersCount = 25;
    struct nameNum {
        char name[6];
        int  num;
    };
    nameNum nameNumArray[slidersCount] = {
        {"LVL 1", 48}, {"SHP 1", 39}, {"TUNE1", 40}, {"F.TU1", 41},
        {"LVL 2", 49}, {"SHP 2", 42}, {"TUNE2", 43}, {"F.TU2", 44},
        {"LVL 3", 50}, {"SHP 3", 45}, {"TUNE3", 46}, {"F.TU3", 47},
        {"NZE",   51},
        {"A", 73}, {"D", 75}, {"S", 79}, {"R", 72},
        {"A", 80}, {"D", 81}, {"S", 82}, {"R", 83},
        {"A", 84}, {"D", 85}, {"S", 86}, {"R", 87}};
    OwnedArray<Slider> slidersArray;
    OwnedArray<Label> labelsArray;
    juce::Label vcaEnvLabel;
    juce::Label vcfEnvLabel;
    juce::Label env3EnvLabel;

    void refreshMidiOutputs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

