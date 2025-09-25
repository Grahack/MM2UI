#pragma once

#include <map>
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

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
    void sendNRPN(int channel, int param, int value);

private:
    CustomLookAndFeel customLookAndFeel;
    // MIDI channel
    int channel;
    juce::ComboBox channelSelector;
    // MIDI out
    juce::ComboBox midiOutputSelector;
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;
    juce::TextButton refreshButton;
    // controllers section
    OwnedArray<ComboBox> oscAlgosArray;
    static const int slidersCount = 25;
    struct nameNum {
        char name[6];
        int  num;
    };
    int oscAlgosNum[3] = {27, 31, 35};
    std::string algosArray[12] = {"Sine", "SinF", "SinH",
                                  "TrPs", "TrSw", "TrSq",
                                  "Saw",  "Sup1", "Sup2",
                                  "SwHs", "Squr", "SqHs"};
    nameNum nameNumArray[slidersCount] = {
        {"LVL 1", 39}, {"SHP 1", 28}, {"TUNE1", 29}, {"F.TU1", 30},
        {"LVL 2", 40}, {"SHP 2", 32}, {"TUNE2", 33}, {"F.TU2", 34},
        {"LVL 3", 41}, {"SHP 3", 36}, {"TUNE3", 37}, {"F.TU3", 38},
        {"NZE",   42},
        {"A", 49}, {"D", 50}, {"S", 51}, {"R", 52},
        {"A", 53}, {"D", 54}, {"S", 55}, {"R", 56},
        {"A", 57}, {"D", 58}, {"S", 59}, {"R", 60}};
    OwnedArray<Slider> slidersArray;
    OwnedArray<Label> labelsArray;
    juce::Label vcaEnvLabel;
    juce::Label vcfEnvLabel;
    juce::Label env3EnvLabel;

    void refreshMidiOutputs();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

