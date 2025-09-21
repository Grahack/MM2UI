#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1000, 1000);

    myButton.setButtonText("NOISE");
    myButton.setClickingTogglesState(true);
    myButton.addListener(this);
    addAndMakeVisible(myButton);

    statusLabel.setText("OFF", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightgrey);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(statusLabel);

    glideControl.onValueChanged = [this](int value) {
        sendCC(1, 7, value);
    };
    addAndMakeVisible(glideControl);

    testSlider.setRange(0, 127, 1);  // 1 for integer value to be displayed
    testSlider.addListener(this);
    glideControl.onValueChanged = [this](int value) {
        sendCC(1, 7, value);
    };
    testSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::lightgrey);
    testSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    testSlider.setSliderStyle(juce::Slider::LinearVertical);
    testSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    addAndMakeVisible(testSlider);

    sliderLabel.setText("OSC 1", juce::dontSendNotification);
    sliderLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightgrey);
    addAndMakeVisible(sliderLabel);
    sliderLabel.attachToComponent(&testSlider, false);

    midiOutputSelector.addListener(this);
    addAndMakeVisible(midiOutputSelector);

    midiOutputLabel.attachToComponent(&midiOutputSelector, true);
    addAndMakeVisible(midiOutputLabel);

    refreshMidiOutputs();
}

MainComponent::~MainComponent()
{
    myButton.removeListener(this);
    midiOutputSelector.removeListener(this);
    midiOut.reset(); // Closes the MIDI out port
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    midiOutputSelector.setBounds(20, 20, 300, 50);
    myButton.setBounds(20, 70, 150, 50);
    statusLabel.setBounds(180, 70, 70, 50);
    glideControl.setBounds(20, 130, 50, 200);
    testSlider.setBounds(200, 200, 150, 150);
}

void MainComponent::refreshMidiOutputs()
{
    midiOutputSelector.clear();
    availableMidiOutputs = juce::MidiOutput::getAvailableDevices();

    for (int i = 0; i < availableMidiOutputs.size(); ++i)
    {
        midiOutputSelector.addItem(availableMidiOutputs[i].name, i + 1);
    }

    if (availableMidiOutputs.isEmpty())
    {
        midiOutputSelector.addItem("No MIDI out found!", 1);
        midiOutputSelector.setEnabled(false);
    }
    else
    {
        midiOutputSelector.setSelectedId(1);
        // Selects the first available
        comboBoxChanged(&midiOutputSelector);
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &midiOutputSelector)
    {
        int index = midiOutputSelector.getSelectedId() - 1;

        if (index >= 0 && index < availableMidiOutputs.size())
        {
            midiOut.reset(); // Closes the current one
            auto deviceInfo = availableMidiOutputs[index];
            midiOut = juce::MidiOutput::openDevice(deviceInfo.identifier);

            if (midiOut != nullptr)
                DBG("MIDI out is now: " + deviceInfo.name);
            else
                DBG("Could not open MIDI out: " + deviceInfo.name);
        }
    }
}

void MainComponent::sendCC(int chan, int cc, int val)
{
    if (midiOut != nullptr)
    {
        auto msg = juce::MidiMessage::controllerEvent(chan, cc, val);
        midiOut->sendMessageNow(msg);
        DBG("CC" << cc << " = " << val << " on chan " << chan);
    }
    else
    {
        DBG("No active MIDI out!");
    }
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &myButton)
    {
        bool isOn = myButton.getToggleState();
        statusLabel.setText(isOn ? "ON" : "OFF", juce::dontSendNotification);
        sendCC(1, 7, isOn ? 127 : 0);
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &testSlider)
    {
        sendCC(1, 7, testSlider.getValue());
    }
}
