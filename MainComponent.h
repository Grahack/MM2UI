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
    // OSC waveforms
    OwnedArray<ComboBox> oscAlgosArray;
    static const int slidersCount = 42;
    struct nameNRPN {
        char name[7];
        int  NRPN;
    };
    int oscAlgosNRPN[3] = {27, 31, 35};
    std::string algosArray[12] = {"Sine", "SinF", "SinH",
                                  "TrPs", "TrSw", "TrSq",
                                  "Saw",  "Sup1", "Sup2",
                                  "SwHs", "Squr", "SqHs"};
    std::string algosArrayOsc1[4] = {"fmSi", "fmTr", "fmSw", "fmSq"};
    // all the sliders
    nameNRPN oscNameNRPNs[slidersCount] = {
        {"LVL", 39}, {"SHP", 28}, {"TUNE", 29}, {"F.TUNE", 30},
        {"LVL", 40}, {"SHP", 32}, {"TUNE", 33}, {"F.TUNE", 34},
        {"LVL", 41}, {"SHP", 36}, {"TUNE", 37}, {"F.TUNE", 38},
        {"NZE", 42}, {"DRV", 48}, {"EQ", 107}, {"VEL", 25}, {"MIX", 17},
        {"A", 49}, {"D", 50}, {"S", 51}, {"R", 52},
        {"A", 53}, {"D", 54}, {"S", 55}, {"R", 56},
        {"A", 57}, {"D", 58}, {"S", 59}, {"R", 60},
        {"V.DET", 19}, {"O.DET", 20}, {"GLD", 22}, {"PB.D", 23}, {"PB.U", 24},
        {"C.O.", 43}, {"RESO", 44},
        {"tim", 108}, {"fbk", 109}, {"lvl", 110},
        {"dcy", 111}, {"mod", 112}, {"lvl", 113}
    };
    OwnedArray<Slider> slidersArray;
    OwnedArray<Label> labelsArray;
    // env section
    juce::Label vcaEnvLabel;
    juce::Label vcfEnvLabel;
    juce::Label env3EnvLabel;
    juce::ToggleButton vcaEnvReset;
    juce::ToggleButton vcfEnvReset;
    juce::ToggleButton env3EnvReset;
    // LFOs section
    std::string lfoWavesArray[19] = {"BI Sin", "BI Tri", "BI Saw", "BI Squ",
                                     "BI S&H", "BI Rnd",
                                     "UNI Atk A", "UNI Atk B", "UNI Atk C",
                                     "UNI Dcy A", "UNI Dcy B", "UNI Dcy C",
                                     "UNI A-D A", "UNI A-D B", "UNI A-D C",
                                     "UNI Sqr_", "UNI _Sqr",
                                     "UNI S&H", "UNI Rnd"};
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
    // voice + filter + fx section
    // voice
    juce::Label voiceLabel;
    juce::ComboBox voiceAssign;
    juce::ComboBox voiceUnison;
    juce::ToggleButton voicePhaseReset;
    // filter
    juce::Label filterLabel;
    juce::ComboBox filterType;
    juce::ComboBox filterChar;
    OwnedArray<juce::Slider> filterArray;
    // Rotary sliders: env amt, env vel, kbd, fm3
    int filterNRPNs[4] = {45, 26, 46, 47};
    OwnedArray<juce::Label> filterLblArray;
    // fx
    juce::ComboBox chorusCombo;
    juce::ComboBox panCombo;
    juce::Slider panSpread;
    juce::ComboBox delayCombo;
    juce::Label reverbLabel;
    // ops
    juce::ComboBox lagCombo;
    juce::Slider lagSlider;
    OwnedArray<juce::ComboBox> multArray;

    void refreshMidiPorts();

    struct ParamSpec {
        int offset;
        int resolution;  // 7 for MSB and 14 for MSB/LSB
        std::string uiElement;
        int num;
    };
    static const std::unordered_map<std::string, ParamSpec> paramMap;
    int readParamValue(const uint8_t* data, const ParamSpec& spec);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
