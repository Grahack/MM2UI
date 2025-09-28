#pragma once

#include <map>
#include <unordered_map>
#include <JuceHeader.h>
#include "CustomLookAndFeel.h"

using namespace std;

class MainComponent  : public juce::Component,
                       public juce::MidiInputCallback,
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
    void sendNRPN_MSB_LSB(int channel, int param, int value);
    void handleIncomingMidiMessage(juce::MidiInput* source,
                                   const juce::MidiMessage& message) override;

private:
    CustomLookAndFeel customLookAndFeel;
    // MIDI channel
    int channel;
    juce::ComboBox channelSelector;
    // MIDI in/out
    juce::ComboBox midiInputSelector;
    juce::Array<juce::MidiDeviceInfo> availableMidiInputs;
    std::unique_ptr<juce::MidiInput> midiIn;
    juce::ComboBox midiOutputSelector;
    juce::Array<juce::MidiDeviceInfo> availableMidiOutputs;
    std::unique_ptr<juce::MidiOutput> midiOut;
    juce::TextButton refreshButton;
    // controllers section
    OwnedArray<ComboBox> oscAlgosArray;
    static const int slidersCount = 25;
    struct nameNRPN {
        char name[7];
        int  NRPN;
    };
    int oscAlgosNRPN[3] = {27, 31, 35};
    std::string algosArray[12] = {"Sine", "SinF", "SinH",
                                  "TrPs", "TrSw", "TrSq",
                                  "Saw",  "Sup1", "Sup2",
                                  "SwHs", "Squr", "SqHs"};
    nameNRPN oscNameNRPNs[slidersCount] = {
        {"LVL", 39}, {"SHP", 28}, {"TUNE", 29}, {"F.TUNE", 30},
        {"LVL", 40}, {"SHP", 32}, {"TUNE", 33}, {"F.TUNE", 34},
        {"LVL", 41}, {"SHP", 36}, {"TUNE", 37}, {"F.TUNE", 38},
        {"NZE",   42},
        {"A", 49}, {"D", 50}, {"S", 51}, {"R", 52},
        {"A", 53}, {"D", 54}, {"S", 55}, {"R", 56},
        {"A", 57}, {"D", 58}, {"S", 59}, {"R", 60}};
    OwnedArray<Slider> slidersArray;
    OwnedArray<Label> labelsArray;
    juce::Label vcaEnvLabel;
    juce::Label vcfEnvLabel;
    juce::Label env3EnvLabel;
    std::string lfoWavesArray[19] = {"Sin", "Tri", "Saw", "Squ", "S&H", "Rnd",
                                     "Atk A", "Atk B", "Atk C",
                                     "Dcy A", "Dcy B", "Dcy C",
                                     "A-D A", "A-D B", "A-D C",
                                     "Sqr_", "_Sqr"};
    std::string lfoModesArray[4] = {"vce", "one", "key", "free"};
    struct lfoNRPN {
        int waveform;
        int mode;
        int speed;
    };
    lfoNRPN lfoNRPNs[3] = {{61, 63, 62}, {64, 66, 65}, {67, 69, 68}};
    struct lfoBlock {
        std::unique_ptr<juce::ComboBox> waveform;
        std::unique_ptr<juce::ComboBox> mode;
        std::unique_ptr<juce::Slider> speed;
    };
    OwnedArray<lfoBlock> lfoArray;

    void refreshMidiPorts();

    struct ParamSpec {
        int offset;
        int resolution;  // 7 for MSB and 14 for MSB/LSB
    };
    static const std::unordered_map<std::string, ParamSpec> paramMap;
    int readParamValue(const uint8_t* data, const ParamSpec& spec);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
