#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1000, 1000);

    getLookAndFeel().setColour(juce::Slider::thumbColourId,
                               juce::Colours::red);
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId,
                               juce::Colours::black);
    getLookAndFeel().setColour(juce::Slider::backgroundColourId,
                               juce::Colours::lightgrey);
    getLookAndFeel().setColour(juce::Label::backgroundColourId,
                               juce::Colours::black);
    getLookAndFeel().setColour(juce::Label::textColourId,
                               juce::Colours::white);

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
    for (int i = 0; i < 3; i++)
    {
        oscAlgosArray.add(new ComboBox());
        for (int j = 0; j < 12; j++)
        {
            oscAlgosArray[i]->addItem(algosArray[j], j+1);
        }
        oscAlgosArray[i]->setSelectedId(1, juce::dontSendNotification);
        oscAlgosArray[i]->addListener(this);
        addAndMakeVisible(*oscAlgosArray[i]);
    }
    // See numName init in MainComponent.h
    for (int i = 0; i < slidersCount; i++)
    {
        slidersArray.add(new juce::Slider());
        slidersArray[i]->setRange(0, 127, 1);  // 1 for integer value to be displayed
        slidersArray[i]->addListener(this);
        slidersArray[i]->setSliderStyle(juce::Slider::LinearVertical);
        slidersArray[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
        // TUNE is between -24 and +12 semitones
        if ( i < 12 && (i-2) % 4 == 0 )
        {
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int tune = (value * (12 - (-24))) / 127 - 24;
                return (tune > 0 ? "+" : "") + juce::String(tune);
            };
            slidersArray[i]->setValue(85, juce::dontSendNotification);
        }
        // FINE TUNE is between -25 and +25 cents
        else if ( i < 12 && (i-3) % 4 == 0 )
        {
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int tune = (value * (25 - (-25))) / 127 - 25;
                return (tune > 0 ? "+" : "") + juce::String(tune);
            };
            slidersArray[i]->setValue(63, juce::dontSendNotification);
        }
        addAndMakeVisible(*slidersArray[i]);
        labelsArray.add(new juce::Label());
        labelsArray[i]->setText(oscNameNRPNs[i].name, juce::dontSendNotification);
        labelsArray[i]->setJustificationType(juce::Justification::centred);
        labelsArray[i]->attachToComponent(slidersArray[i], false);
        addAndMakeVisible(*labelsArray[i]);
    }

    // env section
    vcaEnvLabel.setText("ENV 1: VCA", juce::dontSendNotification);
    vcaEnvLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vcaEnvLabel);
    vcfEnvLabel.setText("ENV 2: VCF", juce::dontSendNotification);
    vcfEnvLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vcfEnvLabel);
    env3EnvLabel.setText("ENV 3", juce::dontSendNotification);
    env3EnvLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(env3EnvLabel);

    //slidersArray[i]->setSliderStyle(juce::Slider::Rotary);
    //slidersArray[i]->setLookAndFeel(&customLookAndFeel);
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
    setLookAndFeel(nullptr);
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
    // some space
    area.removeFromTop(10);
    // Oscillators section
    // Protect this section from a premature execution
    if (oscAlgosArray.size() < 3) return;
    int algosComboBoxHeight = 30;
    auto algosArea = area.removeFromTop(algosComboBoxHeight);
    int oscSlidersWidth = algosArea.getWidth() / 16;
    for (int i = 0; i < 3; i++)
    {
        oscAlgosArray[i]->setBounds(algosArea.removeFromLeft(oscSlidersWidth * 4));
        algosArea.removeFromLeft(oscSlidersWidth);
    }
    // Protect this section from a premature execution
    if (slidersArray.size() == 0) return;
    int slidersHeight = 200;
    int slidersLabelHeight = 30;
    area.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto oscArea = area.removeFromTop(slidersHeight);
    for (int i = 0; i < 13; i++)
    {
        if (i > 0 && i % 4 == 0)
        {
            oscArea.removeFromLeft(oscSlidersWidth);
        }
        slidersArray[i]->setBounds(oscArea.removeFromLeft(oscSlidersWidth));
    }
    // some space
    area.removeFromTop(10);
    // env section
    int envHeaderHeight = 40;
    auto envLabelsArea = area.removeFromTop(envHeaderHeight);
    int envLabelsWidth = envLabelsArea.getWidth() / 3;
    vcaEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    vcfEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    env3EnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    area.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto envArea = area.removeFromTop(slidersHeight);
    int envSlidersWidth = envArea.getWidth() / 14;
    for (int i = 13; i < 25; i++)
    {
        if (i > 13 && (i-1) % 4 == 0)
        {
            envArea.removeFromLeft(envSlidersWidth);
        }
        slidersArray[i]->setBounds(envArea.removeFromLeft(envSlidersWidth));
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

void MainComponent::comboBoxChanged(juce::ComboBox* combo)
{
    if (combo == &channelSelector)
    {
        channel = channelSelector.getSelectedId();
        DBG("CHAN is now: " + std::to_string(channel));
    }
    else if (combo == &midiOutputSelector)
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
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (combo == oscAlgosArray[i])
            {
                int param = oscAlgosNRPN[i];
                int algoIndex = (*combo).getSelectedId() - 1;
                sendNRPN(channel, param, algoIndex);
            }
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
    int param = 0;
    for (int i = 0; i < slidersCount; i++)
    {
        if (slider == slidersArray[i])
        {
            param = oscNameNRPNs[i].NRPN;
        }
    }
    sendNRPN(channel, param, (*slider).getValue());
}

void MainComponent::sendCC(int chan, int cc, int val)
{
    if (midiOut != nullptr)
    {
        auto msg = juce::MidiMessage::controllerEvent(chan, cc, val);
        midiOut->sendMessageNow(msg);
    }
    else
    {
        DBG("No active MIDI out!");
    }
}

void MainComponent::sendNRPN(int channel, int parameterNumber, int value)
{
    if (parameterNumber < 0 || parameterNumber > 16383 || value > 127)
        return;

    uint8_t nrpnMSB = (parameterNumber >> 7) & 0x7F;
    uint8_t nrpnLSB = parameterNumber & 0x7F;

    // NRPN MSB and LSB
    sendCC(channel, 99, nrpnMSB); // NRPN MSB
    sendCC(channel, 98, nrpnLSB); // NRPN LSB

    // Data Entry MSB (value)
    sendCC(channel, 6, value);

    // Optionally clear NRPN selection (recommended good practice)
    sendCC(channel, 99, 127);
    sendCC(channel, 98, 127);
}
