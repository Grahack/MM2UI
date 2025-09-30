#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(1000, 1000);

    juce::Colour dark = juce::Colour (40, 40, 40);
    juce::Colour mm2 = juce::Colour (0xffF76D22);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, mm2);
    getLookAndFeel().setColour(juce::Slider::textBoxTextColourId,
                               juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::textBoxBackgroundColourId, dark);
    getLookAndFeel().setColour(juce::Slider::trackColourId,
                               juce::Colours::darkgrey);
    getLookAndFeel().setColour(juce::Slider::backgroundColourId,
                               juce::Colours::lightgrey);
    getLookAndFeel().setColour(juce::TextButton::buttonColourId, dark);
    getLookAndFeel().setColour(juce::ComboBox::backgroundColourId, dark);
    getLookAndFeel().setColour(juce::Label::backgroundColourId, mm2);
    getLookAndFeel().setColour(juce::Label::textColourId,
                               juce::Colours::black);

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

    // MIDI in/out
    midiInputSelector.addListener(this);
    addAndMakeVisible(midiInputSelector);
    midiOutputSelector.addListener(this);
    addAndMakeVisible(midiOutputSelector);
    refreshMidiPorts();
    refreshButton.setButtonText("REFRESH");
    refreshButton.addListener(this);
    addAndMakeVisible(refreshButton);

    // oscillators section
    for (int i = 0; i < 3; i++)
    {
        oscAlgosArray.add(new ComboBox());
        for (int j = 0; j < 12; j++)
        {
            oscAlgosArray[i]->addItem("OSC " + std::to_string(i+1) + ": " +
                                      algosArray[j], j+1);
        }
        oscAlgosArray[i]->setSelectedId(1, juce::dontSendNotification);
        oscAlgosArray[i]->setColour(juce::ComboBox::textColourId,
                                    juce::Colours::black);
        oscAlgosArray[i]->setColour(juce::ComboBox::backgroundColourId,
                                    juce::Colour(0xffF76D22));
        oscAlgosArray[i]->setLookAndFeel(&customLookAndFeel);
        oscAlgosArray[i]->addListener(this);
        addAndMakeVisible(*oscAlgosArray[i]);
    }
    // See numName init in MainComponent.h
    for (int i = 0; i < slidersCount; i++)
    {
        slidersArray.add(new juce::Slider());
        slidersArray[i]->addListener(this);
        slidersArray[i]->setSliderStyle(juce::Slider::LinearVertical);
        slidersArray[i]->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 30);
        // TUNE is between -24 and +12 semitones
        if ( i < 12 && (i-2) % 4 == 0 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(40, 76, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int tune = value - 64;
                return (tune > 0 ? "+" : "") + juce::String(tune);
            };
            slidersArray[i]->valueFromTextFunction = [](const String &text)
            {
                int val = text.getIntValue();
                return val + 64;
            };
            slidersArray[i]->setValue(64, juce::dontSendNotification);
        }
        // FINE TUNE is between -25 and +25 cents
        else if ( i < 12 && (i-3) % 4 == 0 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(39, 89, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int tune = value - 64;
                return (tune > 0 ? "+" : "") + juce::String(tune);
            };
            slidersArray[i]->valueFromTextFunction = [](const String &text)
            {
                int val = text.getIntValue();
                return val + 64;
            };
            slidersArray[i]->setValue(64, juce::dontSendNotification);
        }
        else
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(0, 127, 1);
        }
        addAndMakeVisible(*slidersArray[i]);
        labelsArray.add(new juce::Label());
        labelsArray[i]->setText(oscNameNRPNs[i].name, juce::dontSendNotification);
        labelsArray[i]->setJustificationType(juce::Justification::centred);
        auto sliderLabelFont = juce::FontOptions(16.0f, juce::Font::bold);
        labelsArray[i]->setFont(sliderLabelFont);
        labelsArray[i]->attachToComponent(slidersArray[i], false);
        addAndMakeVisible(*labelsArray[i]);
    }

    // env section
    auto envLabelFont = juce::FontOptions(22.0f, juce::Font::bold);
    vcaEnvLabel.setText("ENV 1: VCA", juce::dontSendNotification);
    vcaEnvLabel.setJustificationType(juce::Justification::centred);
    vcaEnvLabel.setFont(envLabelFont);
    addAndMakeVisible(vcaEnvLabel);
    vcfEnvLabel.setText("ENV 2: VCF", juce::dontSendNotification);
    vcfEnvLabel.setJustificationType(juce::Justification::centred);
    vcfEnvLabel.setFont(envLabelFont);
    addAndMakeVisible(vcfEnvLabel);
    env3EnvLabel.setText("ENV 3", juce::dontSendNotification);
    env3EnvLabel.setJustificationType(juce::Justification::centred);
    env3EnvLabel.setFont(envLabelFont);
    addAndMakeVisible(env3EnvLabel);

    // LFO section
    for (int i = 0; i < 3; i++)
    {
        auto b = std::make_unique<lfoBlock>();
        b->waveform = std::make_unique<juce::ComboBox>();
        b->mode     = std::make_unique<juce::ComboBox>();
        b->speed    = std::make_unique<juce::Slider>();
        lfoArray.add(b.release());
        addAndMakeVisible(*lfoArray[i]->waveform);
        addAndMakeVisible(*lfoArray[i]->mode);
        addAndMakeVisible(*lfoArray[i]->speed);
        lfoArray[i]->waveform->addListener(this);
        lfoArray[i]->mode->addListener(this);
        lfoArray[i]->speed->addListener(this);
        for (int j = 0; j < 19; j++)
        {
            lfoArray[i]->waveform->addItem("LFO " + std::to_string(i+1)
                                           + " " + lfoWavesArray[j], j+1);
        }
        lfoArray[i]->waveform->setSelectedId(1, juce::dontSendNotification);
        for (int j = 0; j < 4; j++)
        {
            lfoArray[i]->mode->addItem(lfoModesArray[j], j+1);
        }
        lfoArray[i]->mode->setJustificationType(juce::Justification::centred);
        lfoArray[i]->mode->setSelectedId(1, juce::dontSendNotification);
        lfoArray[i]->speed->setSliderStyle(juce::Slider::Rotary);
        lfoArray[i]->speed->setRange(0, 157, 1);  // 1 for integer value to be displayed
        lfoArray[i]->speed->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        lfoArray[i]->speed->setLookAndFeel(&customLookAndFeel);
    }

    // First drawing request because previous ones were aborted
    // because of arrays not full of what we needed.
    resized();
}

MainComponent::~MainComponent()
{
    channelSelector.removeListener(this);
    midiInputSelector.removeListener(this);
    midiOutputSelector.removeListener(this);
    refreshButton.removeListener(this);
    midiOut.reset(); // Closes the MIDI out port
    for (int i = 0; i < 3; i++)
    {
        oscAlgosArray[i]->removeListener(this);
    }
    for (int i = 0; i < slidersCount; i++)
    {
        slidersArray[i]->removeListener(this);
    }
    for (int i = 0; i < 3; i++)
    {
        lfoArray[i]->waveform->removeListener(this);
        lfoArray[i]->mode->removeListener(this);
        lfoArray[i]->speed->removeListener(this);
    }
    setLookAndFeel(nullptr);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    int w = area.getWidth();
    int headerHeight = 50;
    auto headerArea = area.removeFromTop(headerHeight);
    channelSelector.setBounds(headerArea.removeFromLeft(w * 1 / 10));
    midiInputSelector.setBounds(headerArea.removeFromLeft(w * 4 / 10));
    midiOutputSelector.setBounds(headerArea.removeFromLeft(w * 4 / 10));
    refreshButton.setBounds(headerArea.removeFromLeft(w * 1 / 10));
    // some space
    area.removeFromTop(10);
    // Oscillators section
    // Protect this section from a premature execution
    if (oscAlgosArray.size() < 3) return;
    int algosComboBoxHeight = 40;
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
    int envSlidersWidth = area.getWidth() / 14;
    int envHeaderHeight = 40;
    auto envLabelsArea = area.removeFromTop(envHeaderHeight);
    int envLabelsWidth = envSlidersWidth * 4;
    vcaEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    envLabelsArea.removeFromLeft(envSlidersWidth);
    vcfEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    envLabelsArea.removeFromLeft(envSlidersWidth);
    env3EnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    area.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto envArea = area.removeFromTop(slidersHeight);
    for (int i = 13; i < 25; i++)
    {
        if (i > 13 && (i-1) % 4 == 0)
        {
            envArea.removeFromLeft(envSlidersWidth);
        }
        slidersArray[i]->setBounds(envArea.removeFromLeft(envSlidersWidth));
    }
    // some space
    area.removeFromTop(10);
    // LFO section
    // Protect this section from a premature execution
    if (oscAlgosArray.size() < 3) return;
    auto lfoArea = area.removeFromTop(60);
    int lfoBaseWidth = lfoArea.getWidth() / 14;  // just to match env section
    int lfoW = lfoBaseWidth * 4 / 3;
    for (int i = 0; i < 3; i++)
    {
        lfoArray[i]->waveform->setBounds(lfoArea.removeFromLeft(lfoW));
        lfoArray[i]->mode->setBounds(lfoArea.removeFromLeft(lfoW));
        lfoArray[i]->speed->setBounds(lfoArea.removeFromLeft(lfoW));
        if (i < 2)
        {
            lfoArea.removeFromLeft(lfoBaseWidth);
        }
    }
}

void MainComponent::refreshMidiPorts()
{
    // IN
    midiInputSelector.clear();
    availableMidiInputs = juce::MidiInput::getAvailableDevices();

    for (int i = 0; i < availableMidiInputs.size(); ++i)
    {
        midiInputSelector.addItem("IN: " + availableMidiInputs[i].name, i + 1);
    }

    if (availableMidiInputs.isEmpty())
    {
        midiInputSelector.addItem("IN: No MIDI in found!", 1);
        midiInputSelector.setEnabled(false);
    }
    else if (availableMidiInputs.size() > 1)
    {
        // Selects the second available
        midiInputSelector.setSelectedId(2);
        comboBoxChanged(&midiInputSelector);
    }
    else
    {
        // Selects the first available
        midiInputSelector.setSelectedId(1);
        comboBoxChanged(&midiInputSelector);
    }
    // OUT
    midiOutputSelector.clear();
    availableMidiOutputs = juce::MidiOutput::getAvailableDevices();

    for (int i = 0; i < availableMidiOutputs.size(); ++i)
    {
        midiOutputSelector.addItem("OUT: " + availableMidiOutputs[i].name, i + 1);
    }

    if (availableMidiOutputs.isEmpty())
    {
        midiOutputSelector.addItem("OUT: No MIDI out found!", 1);
        midiOutputSelector.setEnabled(false);
    }
    else if (availableMidiOutputs.size() > 1)
    {
        // Selects the second available
        midiOutputSelector.setSelectedId(2);
        comboBoxChanged(&midiOutputSelector);
    }
    else
    {
        // Selects the first available
        midiOutputSelector.setSelectedId(1);
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
    else if (combo == &midiInputSelector)
    {
        int index = midiInputSelector.getSelectedId() - 1;
        if (index >= 0 && index < availableMidiInputs.size())
        {
            auto deviceInfo = availableMidiInputs[index];

            // Arrêter l'entrée MIDI précédente si elle existe
            if (midiIn)
                midiIn->stop();

            midiIn = juce::MidiInput::openDevice(deviceInfo.identifier, this);

            if (midiIn)
            {
                midiIn->start();
                DBG("MIDI in  is now: " + deviceInfo.name);
            }
            else
            {
                DBG("Could not open MIDI in: " + deviceInfo.name);
            }
        }
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
        for (int i = 0; i < 3; ++i)
        {
            int value = 0;
            if (combo == lfoArray[i]->waveform.get())
            {
                int param = lfoNRPNs[i].waveform;
                sendNRPN(channel, param, (*combo).getSelectedId() - 1);
            }
            else if (combo == lfoArray[i]->mode.get())
            {
                int param = lfoNRPNs[i].mode;
                sendNRPN(channel, param, (*combo).getSelectedId() - 1);
            }
        }
    }
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &refreshButton)
    {
        refreshMidiPorts();
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    for (int i = 0; i < slidersCount; i++)
    {
        if (slider == slidersArray[i])
        {
            int value = (*slider).getValue();
            sendNRPN(channel, oscNameNRPNs[i].NRPN, value);
            // color code tuning sliders
            if ( i < 12 && ((i-2) % 4 == 0 || (i-3) % 4 == 0 ))
            {
                juce::Colour colour = juce::Colours::darkgrey;
                if (value < 64)
                {
                    colour = juce::Colours::darkred;
                }
                else if (value > 64)
                {
                    colour = juce::Colours::darkgreen;
                }
                slider->setColour(juce::Slider::trackColourId, colour);
            }
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (slider == lfoArray[i]->speed.get())
        {
            sendNRPN_MSB_LSB(channel, lfoNRPNs[i].speed, (*slider).getValue());
        }
    }
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

void MainComponent::sendNRPN_MSB_LSB(int channel, int parameterNumber, int value)
{
    // Vérification des plages valides
    if (parameterNumber < 0 || parameterNumber > 16383 || value < 0 || value > 16383)
        return;

    // Découpage du paramètre en MSB/LSB
    uint8_t nrpnMSB = (parameterNumber >> 7) & 0x7F;
    uint8_t nrpnLSB = parameterNumber & 0x7F;

    // Découpage de la valeur en MSB/LSB
    uint8_t valueMSB = (value >> 7) & 0x7F;
    uint8_t valueLSB = value & 0x7F;

    // Envoi du paramètre NRPN (MSB, LSB)
    sendCC(channel, 99, nrpnMSB); // NRPN MSB
    sendCC(channel, 98, nrpnLSB); // NRPN LSB

    // Envoi de la valeur (MSB, LSB)
    sendCC(channel, 6, valueMSB);  // Data Entry MSB
    sendCC(channel, 38, valueLSB); // Data Entry LSB

    // Optionnel : nettoyage (bonne pratique)
    sendCC(channel, 99, 127);
    sendCC(channel, 98, 127);
}

int MainComponent::readParamValue(const uint8_t* data, const ParamSpec& spec)
{
    int realOffset = spec.offset - 1;
    if (spec.resolution == 7)
        return data[realOffset];
    else
        return (data[realOffset] << 7) | data[realOffset + 1];
}

void MainComponent::handleIncomingMidiMessage(juce::MidiInput* source,
                                              const juce::MidiMessage& message)
{
    DBG("Received MIDI message: " + message.getDescription());
    if (!message.isSysEx()) return;

    const uint8_t* data = message.getSysExData();
    int size = message.getSysExDataSize();

    // Only handle MM2 messages
    // F0 doesn't seem to belong to the data var
    if (size >= 8 && data[0] == 0x00
                  && data[1] == 0x21 && data[2] == 0x22
                  && data[3] == 0x4D && data[4] == 0x02
                  && data[5] == 0x03 && data[6] == 0x10)
    {
        DBG("Received an MM2 message!");

        // progName is 8 chars: programNameCharX
        std::string progName;
        for (int i = 0; i < 8; ++i)
        {
            std::string key = "programNameChar" + std::to_string(i);
            int ascii = readParamValue(data, paramMap.at(key));
            char c = static_cast<char>(ascii);
            progName += c;
        }
        DBG("Program name: " << progName);

        int val = 0;
        // programCategory
        // fKnobAssignment
        // qKnobAssignment
        // mKnobAssignment
        // encoder1Assignment
        // encoder2Assignment
        // encoder3Assignment
        // encoder4Assignment
        // programVolume
        // programTempo (from 0->40 to 255->295)
        int tempo = readParamValue(data, paramMap.at("programTempo")) + 40;
        DBG("Tempo: " << tempo);
        // voiceDetune
        // oscDetune
        // panSpread
        // glide
        // pitchBendDown
        // pitchBendUp
        // vcaVelocitySensitivity
        // filterEnvVelocity
        // oscXAlgorithm, X from 1 to 3
        val = readParamValue(data, paramMap.at("osc1Algorithm"));
        oscAlgosArray[0]->setSelectedId(val+1, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc2Algorithm"));
        oscAlgosArray[1]->setSelectedId(val+1, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc3Algorithm"));
        oscAlgosArray[2]->setSelectedId(val+1, juce::dontSendNotification);
        // oscXShape, X from 1 to 3
        val = readParamValue(data, paramMap.at("osc1Shape"));
        slidersArray[1]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc2Shape"));
        slidersArray[5]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc3Shape"));
        slidersArray[9]->setValue(val, juce::dontSendNotification);
        // oscXCoarse, X from 1 to 3
        val = readParamValue(data, paramMap.at("osc1Coarse"));
        slidersArray[2]->setValue(val);
        val = readParamValue(data, paramMap.at("osc2Coarse"));
        slidersArray[6]->setValue(val);
        val = readParamValue(data, paramMap.at("osc3Coarse"));
        slidersArray[10]->setValue(val);
        // oscXFine, X from 1 to 3
        val = readParamValue(data, paramMap.at("osc1Fine"));
        slidersArray[3]->setValue(val);
        val = readParamValue(data, paramMap.at("osc2Fine"));
        slidersArray[7]->setValue(val);
        val = readParamValue(data, paramMap.at("osc3Fine"));
        slidersArray[11]->setValue(val);
        // oscXVolume, X from 1 to 3
        val = readParamValue(data, paramMap.at("osc1Volume"));
        slidersArray[0]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc2Volume"));
        slidersArray[4]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("osc3Volume"));
        slidersArray[8]->setValue(val, juce::dontSendNotification);
        // whiteNoiseVolume, X from 1 to 3
        val = readParamValue(data, paramMap.at("whiteNoiseVolume"));
        slidersArray[12]->setValue(val, juce::dontSendNotification);
        // filterCutoff
        // filterResonance
        // filterEnvAmount (from 29->-99 to 227->99, 128->0)
        int fltEnvAmt = readParamValue(data, paramMap.at("filterEnvAmount"));
        int centeredEnv = fltEnvAmt - 128;
        DBG("Filter Env Amount: " << centeredEnv);
        // keyTracking
        // filterFMAmtFromOSC
        // driveLevel
        // envXAttack envXDecay envXSustain envXRelease (X from 1 to 3)
        val = readParamValue(data, paramMap.at("env1Attack"));
        slidersArray[13]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env1Decay"));
        slidersArray[14]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env1Sustain"));
        slidersArray[15]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env1Release"));
        slidersArray[16]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env2Attack"));
        slidersArray[17]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env2Decay"));
        slidersArray[18]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env2Sustain"));
        slidersArray[19]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env2Release"));
        slidersArray[20]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env3Attack"));
        slidersArray[21]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env3Decay"));
        slidersArray[22]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env3Sustain"));
        slidersArray[23]->setValue(val, juce::dontSendNotification);
        val = readParamValue(data, paramMap.at("env3Release"));
        slidersArray[24]->setValue(val, juce::dontSendNotification);
        // lfoXWaveform lfoXSpeed lfoXMode (X from 1 to 3)
        for (int i = 0; i < 3; i++)
        {
            val = readParamValue(data,
                    paramMap.at("lfo" + std::to_string(i+1) + "Waveform"));
            lfoArray[i]->waveform->setSelectedId(val+1,
                                                 juce::dontSendNotification);
            val = readParamValue(data,
                    paramMap.at("lfo" + std::to_string(i+1) + "Speed"));
            lfoArray[i]->speed->setValue(val, juce::dontSendNotification);
            val = readParamValue(data,
                    paramMap.at("lfo" + std::to_string(i+1) + "Mode"));
            lfoArray[i]->mode->setSelectedId(val+1, juce::dontSendNotification);
        }
        // matrixXSource matrixXDestination matrixXAmount (X from 1 to 10)
        // op1Source
        // op1Amount
        // op2SourceA
        // op2SourceB
        // op3SourceA
        // op3SourceB
        // modulationKnob
        // eqFrequencyControl
        // delayTime
        // delayFeedback
        // delaySendLevel
        // reverbDecay
        // reverbModAmount
        // reverbSendLevel
        // arpStyle
        // arpGateLength
        // arpSpeed
        // voiceMode
        // voiceUnisonCount
        // panSpreadMode
        // env1Reset
        // env2Reset
        // env3Reset
        // oscPhaseReset
        // filterType
        // filterCharacter
        // chorus
        // delayMode
        // arpOnOff
        // arpLatch
        // arpOctaveSpread
        // arpPatternLength
        // arpStep1 to arpStep16
    }
}

// https://www.reddit.com/r/synthesizers/comments/1m5hgju/micromonsta_2_web_editor_and_librarian/?chainedPosts=t3_1nmn0h9
const std::unordered_map<std::string,
                         MainComponent::ParamSpec> MainComponent::paramMap = {
    { "programNameChar0", {8, 7} },
    { "programNameChar1", {9, 7} },
    { "programNameChar2", {10, 7} },
    { "programNameChar3", {11, 7} },
    { "programNameChar4", {12, 7} },
    { "programNameChar5", {13, 7} },
    { "programNameChar6", {14, 7} },
    { "programNameChar7", {15, 7} },
    { "programCategory", {16, 7} },
    { "fKnobAssignment", {17, 7} },
    { "qKnobAssignment", {18, 7} },
    { "mKnobAssignment", {19, 7} },
    { "encoder1Assignment", {20, 7} },
    { "encoder2Assignment", {21, 7} },
    { "encoder3Assignment", {22, 7} },
    { "encoder4Assignment", {23, 7} },
    { "programVolume", {24, 7} },
    { "programTempo", {25, 14} },
    { "voiceDetune", {27, 7} },
    { "oscDetune", {28, 7} },
    { "panSpread", {29, 7} },
    { "glide", {30, 7} },
    { "pitchBendDown", {31, 7} },
    { "pitchBendUp", {32, 7} },
    { "vcaVelocitySensitivity", {33, 7} },
    { "filterEnvVelocity", {34, 7} },
    { "osc1Algorithm", {35, 7} },
    { "osc1Shape", {36, 7} },
    { "osc1Coarse", {37, 7} },
    { "osc1Fine", {38, 7} },
    { "osc2Algorithm", {39, 7} },
    { "osc2Shape", {40, 7} },
    { "osc2Coarse", {41, 7} },
    { "osc2Fine", {42, 7} },
    { "osc3Algorithm", {43, 7} },
    { "osc3Shape", {44, 7} },
    { "osc3Coarse", {45, 7} },
    { "osc3Fine", {46, 7} },
    { "osc1Volume", {47, 7} },
    { "osc2Volume", {48, 7} },
    { "osc3Volume", {49, 7} },
    { "whiteNoiseVolume", {50, 7} },
    { "filterCutoff", {51, 14} },
    { "filterResonance", {53, 7} },
    { "filterEnvAmount", {54, 14} },
    { "keyTracking", {56, 7} },
    { "filterFMAmtFromOSC", {57, 7} },
    { "driveLevel", {58, 7} },
    { "env1Attack", {59, 7} },
    { "env1Decay", {60, 7} },
    { "env1Sustain", {61, 7} },
    { "env1Release", {62, 7} },
    { "env2Attack", {63, 7} },
    { "env2Decay", {64, 7} },
    { "env2Sustain", {65, 7} },
    { "env2Release", {66, 7} },
    { "env3Attack", {67, 7} },
    { "env3Decay", {68, 7} },
    { "env3Sustain", {69, 7} },
    { "env3Release", {70, 7} },
    { "lfo1Waveform", {71, 7} },
    { "lfo1Speed",    {72, 14} },
    { "lfo1Mode",     {74, 7} },
    { "lfo2Waveform", {75, 7} },
    { "lfo2Speed",    {76, 14} },
    { "lfo2Mode",     {78, 7} },
    { "lfo3Waveform", {79, 7} },
    { "lfo3Speed",    {80, 14} },
    { "lfo3Mode",     {82, 7} },
    { "matrix1Source",      {83, 7} },
    { "matrix1Destination", {84, 7} },
    { "matrix1Amount",      {85, 14} },
    { "matrix2Source",      {87, 7} },
    { "matrix2Destination", {88, 7} },
    { "matrix2Amount",      {89, 14} },
    { "matrix3Source",      {91, 7} },
    { "matrix3Destination", {92, 7} },
    { "matrix3Amount",      {93, 14} },
    { "matrix4Source",      {95, 7} },
    { "matrix4Destination", {96, 7} },
    { "matrix4Amount",      {97, 14} },
    { "matrix5Source",      {99, 7} },
    { "matrix5Destination", {100, 7} },
    { "matrix5Amount",      {101, 14} },
    { "matrix6Source",      {103, 7} },
    { "matrix6Destination", {104, 7} },
    { "matrix6Amount",      {105, 14} },
    { "matrix7Source",      {107, 7} },
    { "matrix7Destination", {108, 7} },
    { "matrix7Amount",      {109, 14} },
    { "matrix8Source",      {111, 7} },
    { "matrix8Destination", {112, 7} },
    { "matrix8Amount",      {113, 14} },
    { "matrix9Source",      {115, 7} },
    { "matrix9Destination", {116, 7} },
    { "matrix9Amount",      {117, 14} },
    { "matrix10Source",      {119, 7} },
    { "matrix10Destination", {120, 7} },
    { "matrix10Amount",      {121, 14} },
    { "op1Source", {123, 7} },
    { "op1Amount", {124, 7} },
    { "op2SourceA", {125, 7} },
    { "op2SourceB", {126, 7} },
    { "op3SourceA", {127, 7} },
    { "op3SourceB", {128, 7} },
    { "modulationKnob", {129, 7} },
    { "eqFrequencyControl", {130, 14} },
    { "delayTime", {132, 14} },
    { "delayFeedback", {134, 7} },
    { "delaySendLevel", {135, 7} },
    { "reverbDecay", {136, 7} },
    { "reverbModAmount", {137, 7} },
    { "reverbSendLevel", {138, 7} },
    { "arpStyle", {139, 7} },
    { "arpGateLength", {140, 7} },
    { "arpSpeed", {141, 7} },
    { "voiceMode", {142, 7} },
    { "voiceUnisonCount", {143, 7} },
    { "panSpreadMode", {144, 7} },
    { "env1Reset", {145, 7} },
    { "env2Reset", {146, 7} },
    { "env3Reset", {147, 7} },
    // NRPN 123 not used
    { "oscPhaseReset", {149, 7} },
    { "filterType", {150, 7} },
    { "filterCharacter", {151, 7} },
    { "chorus", {152, 7} },
    { "delayMode", {153, 7} },
    // NRPN 129 not used
    { "arpOnOff", {155, 7} },
    { "arpLatch", {156, 7} },
    { "arpOctaveSpread", {157, 7} },
    { "arpPatternLength", {158, 7} },
    { "arpStep1", {159, 7} },
    { "arpStep2", {160, 7} },
    { "arpStep3", {161, 7} },
    { "arpStep4", {162, 7} },
    { "arpStep5", {163, 7} },
    { "arpStep6", {164, 7} },
    { "arpStep7", {165, 7} },
    { "arpStep8", {166, 7} },
    { "arpStep9", {167, 7} },
    { "arpStep10", {168, 7} },
    { "arpStep11", {169, 7} },
    { "arpStep12", {170, 7} },
    { "arpStep13", {171, 7} },
    { "arpStep14", {172, 7} },
    { "arpStep15", {173, 7} },
    { "arpStep16", {174, 7} },
};
