#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1000, 1000);

    // MIDI channel
    channel = 0;
    channelSelector.addListener(this);
    channelSelector.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(channelSelector);
    for (int i = 0; i < 16; i++)
    {
        channelSelector.addItem("CHAN " + std::to_string(i), i+1);
    }
    channelSelector.setSelectedId(1);

    // MIDI out
    midiOutputSelector.addListener(this);
    addAndMakeVisible(midiOutputSelector);
    refreshMidiOutputs();

    refreshButton.setButtonText("REFRESH");
    refreshButton.addListener(this);
    addAndMakeVisible(refreshButton);

    // oscillators section
    // See numName init in MainComponent.h
    for (int i = 0; i < slidersCount; i++)
    {
        slidersArray.add(new juce::Slider());
        slidersArray[i]->setRange(0, 127, 1);  // 1 for integer value to be displayed
        slidersArray[i]->addListener(this);
        slidersArray[i]->setColour(juce::Slider::backgroundColourId, juce::Colours::lightgrey);
        slidersArray[i]->setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        slidersArray[i]->setSliderStyle(juce::Slider::LinearVertical);
        slidersArray[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
        addAndMakeVisible(*slidersArray[i]);
        labelsArray.add(new juce::Label());
        labelsArray[i]->setText(nameNumArray[i].name, juce::dontSendNotification);
        labelsArray[i]->setJustificationType(juce::Justification::centred);
        labelsArray[i]->setColour(juce::Label::backgroundColourId, juce::Colours::black);
        labelsArray[i]->attachToComponent(slidersArray[i], false);
        addAndMakeVisible(*labelsArray[i]);
    }
    resized();
}

MainComponent::~MainComponent()
{
    channelSelector.removeListener(this);
    midiOutputSelector.removeListener(this);
    refreshButton.removeListener(this);
    midiOut.reset(); // Closes the MIDI out port
    for (int i = 0; i < slidersCount; i++)
    {
        slidersArray[i]->removeListener(this);
    }
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    int w = area.getWidth();
    int headerHeight = 50;
    auto headerArea = area.removeFromTop(headerHeight);
    channelSelector.setBounds(headerArea.removeFromLeft(200));
    midiOutputSelector.setBounds(headerArea.removeFromLeft(200));
    refreshButton.setBounds(headerArea.removeFromLeft(100));
    int slidersHeight = 200;
    int slidersLabelHeight = 30;
    area.removeFromTop(slidersLabelHeight);
    auto oscArea = area.removeFromTop(slidersHeight);
    int oscSlidersWidth = oscArea.getWidth() / 16;
    // Protect this section from a premature execution
    // TODO: replace the hardcoded value with slidersCount
    if (slidersArray.size() < 13) return;
    for (int i = 0; i < slidersCount; i++)
    {
        if (i > 0 && i % 4 == 0)
        {
            oscArea.removeFromLeft(oscSlidersWidth);
        }
        slidersArray[i]->setBounds(oscArea.removeFromLeft(oscSlidersWidth));
    }
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

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &refreshButton)
    {
        refreshMidiOutputs();
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    int CC = 0;
    for (int i = 0; i < slidersCount; i++)
    {
        if (slider == slidersArray[i])
        {
            CC = nameNumArray[i].num;
        }
    }
    sendCC(channel, CC, (*slider).getValue());
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
