#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1000, 1000);

    channel = 0;
    channelSelector.addListener(this);
    channelSelector.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(channelSelector);
    for (int i = 0; i < 16; i++)
    {
        channelSelector.addItem("CHAN " + std::to_string(i), i+1);
    }
    channelSelector.setSelectedId(1);

    refreshButton.setButtonText("REFRESH");
    refreshButton.addListener(this);
    addAndMakeVisible(refreshButton);

    testSlider.setRange(0, 127, 1);  // 1 for integer value to be displayed
    testSlider.addListener(this);
    testSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::lightgrey);
    testSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
    testSlider.setSliderStyle(juce::Slider::LinearVertical);
    testSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
    addAndMakeVisible(testSlider);

    sliderLabel.setText("OSC 1", juce::dontSendNotification);
    sliderLabel.setColour(juce::Label::backgroundColourId, juce::Colours::black);
    addAndMakeVisible(sliderLabel);
    sliderLabel.attachToComponent(&testSlider, false);

    midiOutputSelector.addListener(this);
    addAndMakeVisible(midiOutputSelector);

    refreshMidiOutputs();
}

MainComponent::~MainComponent()
{
    channelSelector.removeListener(this);
    refreshButton.removeListener(this);
    midiOutputSelector.removeListener(this);
    midiOut.reset(); // Closes the MIDI out port
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    int h = 50;
    auto headerArea = area.removeFromTop(h);
    channelSelector.setBounds(headerArea.removeFromLeft(100));
    midiOutputSelector.setBounds(headerArea.removeFromLeft(200));
    refreshButton.setBounds(headerArea.removeFromLeft(100));
    testSlider.setBounds(200, 200, 50, 150);
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
    if (comboBoxThatHasChanged == &channelSelector)
    {
        channel = channelSelector.getSelectedId();
        DBG("MIDI is now: " + std::to_string(channel));
    }
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
    if (button == &refreshButton)
    {
        refreshMidiOutputs();
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &testSlider)
    {
        sendCC(channel, 7, testSlider.getValue());
    }
}
