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
    getLookAndFeel().setColour(juce::Label::backgroundColourId, mm2);
    getLookAndFeel().setColour(juce::Label::textColourId,
                               juce::Colours::black);

    // MIDI channel
    channel = 0;
    channelSelector.addListener(this);
    channelSelector.setJustificationType(juce::Justification::centred);
    channelSelector.setColour(ComboBox::ColourIds::backgroundColourId, dark);
    addAndMakeVisible(channelSelector);
    for (int i = 0; i < 16; i++)
    {
        channelSelector.addItem("CHAN " + std::to_string(i), i+1);
    }
    channelSelector.setSelectedId(1);

    // MIDI in/out
    midiInputSelector.addListener(this);
    midiInputSelector.setColour(ComboBox::ColourIds::backgroundColourId, dark);
    addAndMakeVisible(midiInputSelector);
    midiOutputSelector.addListener(this);
    midiOutputSelector.setColour(ComboBox::ColourIds::backgroundColourId, dark);
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
        if ( i == 0 ) // Osc1 specials
        {
            for (int j = 0; j < 4; j++)
            {
                oscAlgosArray[i]->addItem("OSC " + std::to_string(i+1) + ": " +
                                          algosArrayOsc1[j], j+1+12);
            }
        } else {  // Osc2 and Osc3 specials
            for (int j = 0; j < 8; j++)
            {
                oscAlgosArray[i]->addItem("OSC " + std::to_string(i+1) + ": " +
                                          "Wt0" + std::to_string(j+1), j+1+12);
            }
        }
        oscAlgosArray[i]->setSelectedId(1, juce::dontSendNotification);
        oscAlgosArray[i]->setLookAndFeel(&customLookAndFeel);
        oscAlgosArray[i]->addListener(this);
        addAndMakeVisible(*oscAlgosArray[i]);
    }
    // See controllers section in MainComponent.h
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
        // EQ
        else if ( i == 14 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(29, 227, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int eq = value - 128;
                return (eq > 0 ? "+" : "") + juce::String(eq);
            };
            slidersArray[i]->valueFromTextFunction = [](const String &text)
            {
                int val = text.getIntValue();
                return val + 128;
            };
            slidersArray[i]->setValue(128, juce::dontSendNotification);
        }
        // MIX
        else if ( i == 16 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(40, 88, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int mix = value - 64;
                return (mix > 0 ? "+" : "") + juce::String(mix);
            };
            slidersArray[i]->valueFromTextFunction = [](const String &text)
            {
                int val = text.getIntValue();
                return val + 64;
            };
            slidersArray[i]->setValue(64, juce::dontSendNotification);
        }
        // PB Down
        else if ( i == 32 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(52, 64, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                int pb = value - 64;
                return (pb == 0 ? "-" : "") + juce::String(pb);
            };
            slidersArray[i]->valueFromTextFunction = [](const String &text)
            {
                int val = text.getIntValue();
                return val + 64;
            };
            slidersArray[i]->setValue(64, juce::dontSendNotification);
        }
        // PB Up
        else if ( i == 33 )
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(0, 12, 1);
            slidersArray[i]->textFromValueFunction = [](double value)
            {
                return "+" + juce::String(value);
            };
            // needed for the + to appear but not for pitch bend down...
            slidersArray[i]->updateText();
        }
        else
        {
            // 1 for integer value to be displayed
            slidersArray[i]->setRange(0, 127, 1);
        }
        addAndMakeVisible(*slidersArray[i]);
        // SHAPE not visible on startup
        if ( i < 12 && (i-1) % 4 == 0 )
        {
            slidersArray[i]->setVisible(false);
        }
        // labels
        labelsArray.add(new juce::Label());
        labelsArray[i]->setText(oscNameNRPNs[i].name, juce::dontSendNotification);
        labelsArray[i]->setJustificationType(juce::Justification::centred);
        auto sliderLabelFont = juce::FontOptions(18.0f, juce::Font::bold);
        labelsArray[i]->setFont(sliderLabelFont);
        labelsArray[i]->attachToComponent(slidersArray[i], false);
    }

    // env section
    // sliders are created in the previous section
    auto headerLabelFont = juce::FontOptions(22.0f, juce::Font::bold);
    auto rtzText = "RTZ";
    vcaEnvLabel.setText("ENV 1: VCA", juce::dontSendNotification);
    vcaEnvLabel.setJustificationType(juce::Justification::centred);
    vcaEnvLabel.setFont(headerLabelFont);
    addAndMakeVisible(vcaEnvLabel);
    vcaEnvReset.setButtonText(rtzText);
    vcaEnvReset.addListener(this);
    addAndMakeVisible(vcaEnvReset);
    vcfEnvLabel.setText("ENV 2: VCF", juce::dontSendNotification);
    vcfEnvLabel.setJustificationType(juce::Justification::centred);
    vcfEnvLabel.setFont(headerLabelFont);
    addAndMakeVisible(vcfEnvLabel);
    vcfEnvReset.setButtonText(rtzText);
    vcfEnvReset.addListener(this);
    addAndMakeVisible(vcfEnvReset);
    env3EnvLabel.setText("ENV 3", juce::dontSendNotification);
    env3EnvLabel.setJustificationType(juce::Justification::centred);
    env3EnvLabel.setFont(headerLabelFont);
    addAndMakeVisible(env3EnvLabel);
    env3EnvReset.setButtonText(rtzText);
    env3EnvReset.addListener(this);
    addAndMakeVisible(env3EnvReset);

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
                                           + ": " + lfoWavesArray[j], j+1);
        }
        lfoArray[i]->waveform->setSelectedId(1, juce::dontSendNotification);
        lfoArray[i]->waveform->setLookAndFeel(&customLookAndFeel);
        for (int j = 0; j < 4; j++)
        {
            lfoArray[i]->mode->addItem(lfoModesArray[j], j+1);
        }
        lfoArray[i]->mode->setJustificationType(juce::Justification::centred);
        lfoArray[i]->mode->setSelectedId(1, juce::dontSendNotification);
        lfoArray[i]->mode->setColour(
                ComboBox::ColourIds::backgroundColourId, dark);
        lfoArray[i]->speed->setSliderStyle(juce::Slider::Rotary);
        lfoArray[i]->speed->setRange(0, 157, 1);  // 1 for integer value to be displayed
        lfoArray[i]->speed->setColour(
                Slider::ColourIds::textBoxBackgroundColourId, dark);
        lfoArray[i]->speed->setLookAndFeel(&customLookAndFeel);
        const char* txt[] = {"32/", "24/", "16/", "12/",
                             "8/", "6/", "4/", "3/", "2/", "3/2",
                             "/1", "2d", "1t", "/2", "4d", "2t",
                             "/4", "8d", "4t", "/8", "16d", "8t",
                             "/16", "32d", "16t", "/32", "64d", "32t",
                             "/64", "64t"};
        lfoArray[i]->speed->textFromValueFunction = [txt](double value)
        {
            if ( value < 128 ) return juce::String(value);
            if ( value >= 128 + size(txt)) return juce::String("");
            return juce::String(txt[int(value - 128)]);
        };
        lfoArray[i]->speed->valueFromTextFunction = [txt](const String &text)
        {
            for (int i = 0; i < size(txt); i++)
            {
                if ( text == txt[i] ) return 128 + i;
            }
            int value = text.getDoubleValue();
            if ( 0 <= value < 128 ) return value;
            return 0;
        };
    }

    // Voice + Filter + FX section
    auto VFFXLabelFont = juce::FontOptions(22.0f, juce::Font::bold);
    // Voice label
    voiceLabel.setText("VOICE", juce::dontSendNotification);
    voiceLabel.setJustificationType(juce::Justification::centred);
    voiceLabel.setFont(headerLabelFont);
    addAndMakeVisible(voiceLabel);
    voicePhaseReset.setButtonText(rtzText);
    voicePhaseReset.addListener(this);
    addAndMakeVisible(voicePhaseReset);
    // Assign ComboBox
    addAndMakeVisible(voiceAssign);
    voiceAssign.setLookAndFeel(&customLookAndFeel);
    voiceAssign.addListener(this);
    std::string assignsArray[4] = {"Modern", "Vintage", "Mono", "Legato"};
    for (int i = 0; i < 4; i++)
    {
        voiceAssign.addItem("Assign: "  + assignsArray[i], i+1);
    }
    voiceAssign.setSelectedId(1, juce::dontSendNotification);
    // Unisson ComboBox
    addAndMakeVisible(voiceUnison);
    voiceUnison.setLookAndFeel(&customLookAndFeel);
    voiceUnison.addListener(this);
    std::string unisonsArray[4] = {"1", "2", "3", "6"};
    for (int i = 0; i < 4; i++)
    {
        voiceUnison.addItem(unisonsArray[i], i+1);
    }
    voiceUnison.setSelectedId(1, juce::dontSendNotification);

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
    vcaEnvReset.removeListener(this);
    vcfEnvReset.removeListener(this);
    env3EnvReset.removeListener(this);
    for (int i = 0; i < 3; i++)
    {
        lfoArray[i]->waveform->removeListener(this);
        lfoArray[i]->waveform->setLookAndFeel(nullptr);
        lfoArray[i]->mode->removeListener(this);
        lfoArray[i]->mode->setLookAndFeel(nullptr);
        lfoArray[i]->speed->removeListener(this);
        lfoArray[i]->speed->setLookAndFeel(nullptr);
    }
    voiceAssign.removeListener(this);
    voiceUnison.removeListener(this);
    voicePhaseReset.removeListener(this);;

    setLookAndFeel(nullptr);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
}

void MainComponent::resized()
{
    int internalMargin = 25;
    auto area = getLocalBounds();
    int totalW = area.getWidth();
    int headerHeight = 50;
    auto headerArea = area.removeFromTop(headerHeight);
    channelSelector.setBounds(headerArea.removeFromLeft(totalW * 1 / 10));
    midiInputSelector.setBounds(headerArea.removeFromLeft(totalW * 4 / 10));
    midiOutputSelector.setBounds(headerArea.removeFromLeft(totalW * 4 / 10));
    refreshButton.setBounds(headerArea.removeFromLeft(totalW * 1 / 10));
    // some space
    area.removeFromTop(internalMargin);
    // Oscillators section
    // Protect this section from a premature execution
    if (oscAlgosArray.size() < 3) return;
    int algosComboBoxHeight = 40;
    auto algosArea = area.removeFromTop(algosComboBoxHeight);
    int oscSlidersWidth = algosArea.getWidth() / 20;
    for (int i = 0; i < 3; i++)
    {
        oscAlgosArray[i]->setBounds(algosArea.removeFromLeft(oscSlidersWidth * 4));
        algosArea.removeFromLeft(oscSlidersWidth);
    }
    // Protect this section from a premature execution
    if (slidersArray.size() == 0) return;
    int slidersHeight = 150;
    int slidersLabelHeight = 30;
    area.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto oscArea = area.removeFromTop(slidersHeight);
    for (int i = 0; i < 17; i++)
    {
        if (i == 4 || i == 8 || i == 12)
        {
            oscArea.removeFromLeft(oscSlidersWidth);
        }
        slidersArray[i]->setBounds(oscArea.removeFromLeft(oscSlidersWidth));
    }
    // some space
    area.removeFromTop(internalMargin);
    // env + LFO section
    int envHeaderHeight = 40;
    int envLFOHeight = envHeaderHeight + slidersLabelHeight + slidersHeight;
    auto envLFOArea = area.removeFromTop(envLFOHeight);
    // env section : labels
    int envSlidersWidth = area.getWidth() / 19;
    auto envArea = envLFOArea.removeFromLeft(envSlidersWidth * 14);
    auto envLabelsArea = envArea.removeFromTop(envHeaderHeight);
    int envLabelsWidth = envSlidersWidth * 3;
    vcaEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    vcaEnvReset.setBounds(envLabelsArea.removeFromLeft(envSlidersWidth));
    envLabelsArea.removeFromLeft(envSlidersWidth);
    vcfEnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    vcfEnvReset.setBounds(envLabelsArea.removeFromLeft(envSlidersWidth));
    envLabelsArea.removeFromLeft(envSlidersWidth);
    env3EnvLabel.setBounds(envLabelsArea.removeFromLeft(envLabelsWidth));
    env3EnvReset.setBounds(envLabelsArea.removeFromLeft(envSlidersWidth));
    // env section : sliders
    envArea.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto envSlidersArea = envArea.removeFromTop(slidersHeight);
    for (int i = 17; i < 29; i++)
    {
        if (i > 17 && (i-1) % 4 == 0)
        {
            envSlidersArea.removeFromLeft(envSlidersWidth);
        }
        slidersArray[i]->setBounds(envSlidersArea.removeFromLeft(envSlidersWidth));
    }
    // LFO section
    // Protect this section from a premature execution
    if (lfoArray.size() < 3) return;
    envLFOArea.removeFromLeft(envSlidersWidth);
    auto lfoArea = envLFOArea.removeFromLeft(envLabelsWidth * 4);
    int lfoW = lfoArea.getWidth();
    for (int i = 0; i < 3; i++)
    {
        auto lfoBlockArea = lfoArea.removeFromTop(envLFOHeight / 3);
        int h1 = lfoBlockArea.getHeight() / 7 * 3;  // slightly less than half
        lfoArray[i]->waveform->setBounds(lfoBlockArea.removeFromTop(h1));
        lfoArray[i]->mode->setBounds(lfoBlockArea.removeFromLeft(lfoW / 3));
        // position, readonly, width, height
        lfoArray[i]->speed->setTextBoxStyle(juce::Slider::TextBoxLeft,
                                            false,
                                            lfoW / 3,
                                            50);
        lfoArray[i]->speed->setBounds(lfoBlockArea.removeFromLeft(lfoW * 2 / 3));
    }
    // some space
    area.removeFromTop(internalMargin);
    // Voice + Filter + FX section: VFFX
    int VFFXHeaderHeight = 40;
    int VFFXHeight = 2*VFFXHeaderHeight + slidersLabelHeight + slidersHeight;
    auto VFFXArea = area.removeFromTop(VFFXHeight);
    int VFFXSlidersWidth = area.getWidth() / 19;
    // voice
    auto voiceArea = VFFXArea.removeFromLeft(VFFXSlidersWidth * 5);
    auto voiceHeadersArea = voiceArea.removeFromTop(VFFXHeaderHeight);
    voiceLabel.setBounds(voiceHeadersArea.removeFromLeft(VFFXSlidersWidth * 4));
    voicePhaseReset.setBounds(voiceHeadersArea.removeFromLeft(VFFXSlidersWidth * 1));
    auto voiceCombosArea = voiceArea.removeFromTop(VFFXHeaderHeight);
    voiceAssign.setBounds(voiceCombosArea.removeFromLeft(VFFXSlidersWidth * 4));
    voiceUnison.setBounds(voiceCombosArea.removeFromLeft(VFFXSlidersWidth * 1));
    voiceArea.removeFromTop(slidersLabelHeight);  // spacer for the attached labels
    auto voiceSlidersArea = voiceArea.removeFromTop(slidersLabelHeight + slidersHeight);
    for (int i = 29; i < 34; i++)
    {
        slidersArray[i]->setBounds(voiceSlidersArea.removeFromLeft(VFFXSlidersWidth));
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
    else if (combo == &voiceAssign)
    {
        sendNRPN(channel, 117, (*combo).getSelectedId() - 1);
    }
    else if (combo == &voiceUnison)
    {
        sendNRPN(channel, 118, (*combo).getSelectedId() - 1);
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
                // disable SHAPE slider
                bool visible = ( algoIndex >= 3 );
                slidersArray[4*i + 1]->setVisible(visible);
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
    else if (button == &vcaEnvReset)
    {
        int value = button->getToggleState() ? 1 : 0;
        sendNRPN(channel, 120, value);
    }
    else if (button == &vcfEnvReset)
    {
        int value = button->getToggleState() ? 1 : 0;
        sendNRPN(channel, 121, value);
    }
    else if (button == &env3EnvReset)
    {
        int value = button->getToggleState() ? 1 : 0;
        sendNRPN(channel, 122, value);
    }
    else if (button == &voicePhaseReset)
    {
        int value = button->getToggleState() ? 1 : 0;
        sendNRPN(channel, 124, value);
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    for (int i = 0; i < slidersCount; i++)
    {
        if (slider == slidersArray[i])
        {
            int value = (*slider).getValue();
            if ( i == 14 )  // EQ
            {
                sendNRPN_MSB_LSB(channel, oscNameNRPNs[i].NRPN, value);
            }
            else
            {
                sendNRPN(channel, oscNameNRPNs[i].NRPN, value);
            }
            // color code tuning and mix sliders
            if ( i < 12 && ((i-2) % 4 == 0 || (i-3) % 4 == 0 ) || i == 16 )
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
            // color code eq slider
            if ( i == 14 )
            {
                juce::Colour colour = juce::Colours::darkgrey;
                if (value < 128)
                {
                    colour = juce::Colours::darkred;
                }
                else if (value > 128)
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
    //DBG("Received MIDI message: " + message.getDescription());
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

        // programCategory
        // fKnobAssignment
        // qKnobAssignment
        // mKnobAssignment
        // encoderXAssignment (X from 1 to 4)
        // programTempo (from 0->40 to 255->295)
        int tempo = readParamValue(data, paramMap.at("programTempo")) + 40;
        DBG("Tempo: " << tempo);

        for (const auto& [key, spec] : paramMap)
        {
            int val = readParamValue(data, paramMap.at(key));
            std::string uiElt = spec.uiElement;
            int num = spec.num;
            juce::MessageManager::callAsync([this, val, uiElt, num] {
                if ( uiElt == "slidersArray" )
                    slidersArray[num]->setValue(val);
                else if ( uiElt == "oscAlgosArray" )
                    oscAlgosArray[num]->setSelectedId(val+1);
                else if ( uiElt == "Reset" )
                {
                    auto dsn = juce::NotificationType::dontSendNotification;
                    switch ( num )
                    {
                        case 0: vcaEnvReset.setToggleState(val==1, dsn); break;
                        case 1: vcfEnvReset.setToggleState(val==1, dsn); break;
                        case 2: env3EnvReset.setToggleState(val==1, dsn); break;
                        case 3: voicePhaseReset.setToggleState(val==1, dsn); break;
                    }
                }
                else if ( uiElt == "lfo_waveform" )
                    lfoArray[num]->waveform->setSelectedId(val+1);
                else if ( uiElt == "lfo_speed" )
                    lfoArray[num]->speed->setValue(val);
                else if ( uiElt == "lfo_mode" )
                    lfoArray[num]->mode->setSelectedId(val+1);
                else if ( uiElt == "voiceAssign" )
                    voiceAssign.setSelectedId(val+1);
                else if ( uiElt == "voiceUnison" )
                    voiceUnison.setSelectedId(val+1);
                else if ( uiElt != "" )
                    DBG("Unknown UI element: " + uiElt);
            });
        }

        // panSpread
        // filterEnvVelocity
        // filterCutoff
        // filterResonance
        // filterEnvAmount (from 29->-99 to 227->99, 128->0)
        int fltEnvAmt = readParamValue(data, paramMap.at("filterEnvAmount"));
        int centeredEnv = fltEnvAmt - 128;
        DBG("Filter Env Amount: " << centeredEnv);
        // keyTracking
        // filterFMAmtFromOSC
        // matrixXSource matrixXDestination matrixXAmount (X from 1 to 10)
        // op1Source
        // op1Amount
        // op2SourceA
        // op2SourceB
        // op3SourceA
        // op3SourceB
        // modulationKnob
        // delayTime
        // delayFeedback
        // delaySendLevel
        // reverbDecay
        // reverbModAmount
        // reverbSendLevel
        // arpStyle
        // arpGateLength
        // arpSpeed
        // panSpreadMode
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
    { "programNameChar0", {8, 7, "", 0} },
    { "programNameChar1", {9, 7, "", 0} },
    { "programNameChar2", {10, 7, "", 0} },
    { "programNameChar3", {11, 7, "", 0} },
    { "programNameChar4", {12, 7, "", 0} },
    { "programNameChar5", {13, 7, "", 0} },
    { "programNameChar6", {14, 7, "", 0} },
    { "programNameChar7", {15, 7, "", 0} },
    { "programCategory", {16, 7, "", 0} },
    { "fKnobAssignment", {17, 7, "", 0} },
    { "qKnobAssignment", {18, 7, "", 0} },
    { "mKnobAssignment", {19, 7, "", 0} },
    { "encoder1Assignment", {20, 7, "", 0} },
    { "encoder2Assignment", {21, 7, "", 0} },
    { "encoder3Assignment", {22, 7, "", 0} },
    { "encoder4Assignment", {23, 7, "", 0} },
    { "programVolume", {24, 7, "slidersArray", 16} },
    { "programTempo", {25, 14, "", 0} },
    { "voiceDetune", {27, 7, "slidersArray", 29} },
    { "oscDetune", {28, 7, "slidersArray", 30} },
    { "panSpread", {29, 7, "", 0} },
    { "glide", {30, 7, "slidersArray", 31} },
    { "pitchBendDown", {31, 7, "slidersArray", 32} },
    { "pitchBendUp", {32, 7, "slidersArray", 33} },
    { "vcaVelocitySensitivity", {33, 7, "slidersArray", 15} },
    { "filterEnvVelocity", {34, 7, "", 0} },
    { "osc1Algorithm", {35, 7, "oscAlgosArray", 0} },
    { "osc1Shape", {36, 7, "slidersArray", 1} },
    { "osc1Coarse", {37, 7, "slidersArray", 2} },
    { "osc1Fine", {38, 7, "slidersArray", 3} },
    { "osc2Algorithm", {39, 7, "oscAlgosArray", 1} },
    { "osc2Shape", {40, 7, "slidersArray", 5} },
    { "osc2Coarse", {41, 7, "slidersArray", 6} },
    { "osc2Fine", {42, 7, "slidersArray", 7} },
    { "osc3Algorithm", {43, 7, "oscAlgosArray", 2} },
    { "osc3Shape", {44, 7, "slidersArray", 9} },
    { "osc3Coarse", {45, 7, "slidersArray", 10} },
    { "osc3Fine", {46, 7, "slidersArray", 11} },
    { "osc1Volume", {47, 7, "slidersArray", 0} },
    { "osc2Volume", {48, 7, "slidersArray", 4} },
    { "osc3Volume", {49, 7, "slidersArray", 8} },
    { "whiteNoiseVolume", {50, 7, "slidersArray", 12} },
    { "filterCutoff", {51, 14, "", 0} },
    { "filterResonance", {53, 7, "", 0} },
    { "filterEnvAmount", {54, 14, "", 0} },
    { "keyTracking", {56, 7, "", 0} },
    { "filterFMAmtFromOSC", {57, 7, "", 0} },
    { "driveLevel", {58, 7,  "slidersArray", 13} },
    { "env1Attack", {59, 7,  "slidersArray", 17} },
    { "env1Decay", {60, 7,   "slidersArray", 18} },
    { "env1Sustain", {61, 7, "slidersArray", 19} },
    { "env1Release", {62, 7, "slidersArray", 20} },
    { "env2Attack", {63, 7,  "slidersArray", 21} },
    { "env2Decay", {64, 7,   "slidersArray", 22} },
    { "env2Sustain", {65, 7, "slidersArray", 23} },
    { "env2Release", {66, 7, "slidersArray", 24} },
    { "env3Attack", {67, 7,  "slidersArray", 25} },
    { "env3Decay", {68, 7,   "slidersArray", 26} },
    { "env3Sustain", {69, 7, "slidersArray", 27} },
    { "env3Release", {70, 7, "slidersArray", 28} },
    { "lfo1Waveform", {71, 7,  "lfo_waveform", 0} },
    { "lfo1Speed",    {72, 14, "lfo_speed", 0} },
    { "lfo1Mode",     {74, 7,  "lfo_mode", 0} },
    { "lfo2Waveform", {75, 7,  "lfo_waveform", 1} },
    { "lfo2Speed",    {76, 14, "lfo_speed", 1} },
    { "lfo2Mode",     {78, 7,  "lfo_mode", 1} },
    { "lfo3Waveform", {79, 7,  "lfo_waveform", 2} },
    { "lfo3Speed",    {80, 14, "lfo_speed", 2} },
    { "lfo3Mode",     {82, 7,  "lfo_mode", 2} },
    { "matrix1Source",      {83, 7, "", 0} },
    { "matrix1Destination", {84, 7, "", 0} },
    { "matrix1Amount",      {85, 14, "", 0} },
    { "matrix2Source",      {87, 7, "", 0} },
    { "matrix2Destination", {88, 7, "", 0} },
    { "matrix2Amount",      {89, 14, "", 0} },
    { "matrix3Source",      {91, 7, "", 0} },
    { "matrix3Destination", {92, 7, "", 0} },
    { "matrix3Amount",      {93, 14, "", 0} },
    { "matrix4Source",      {95, 7, "", 0} },
    { "matrix4Destination", {96, 7, "", 0} },
    { "matrix4Amount",      {97, 14, "", 0} },
    { "matrix5Source",      {99, 7, "", 0} },
    { "matrix5Destination", {100, 7, "", 0} },
    { "matrix5Amount",      {101, 14, "", 0} },
    { "matrix6Source",      {103, 7, "", 0} },
    { "matrix6Destination", {104, 7, "", 0} },
    { "matrix6Amount",      {105, 14, "", 0} },
    { "matrix7Source",      {107, 7, "", 0} },
    { "matrix7Destination", {108, 7, "", 0} },
    { "matrix7Amount",      {109, 14, "", 0} },
    { "matrix8Source",      {111, 7, "", 0} },
    { "matrix8Destination", {112, 7, "", 0} },
    { "matrix8Amount",      {113, 14, "", 0} },
    { "matrix9Source",      {115, 7, "", 0} },
    { "matrix9Destination", {116, 7, "", 0} },
    { "matrix9Amount",      {117, 14, "", 0} },
    { "matrix10Source",      {119, 7, "", 0} },
    { "matrix10Destination", {120, 7, "", 0} },
    { "matrix10Amount",      {121, 14, "", 0} },
    { "op1Source", {123, 7, "", 0} },
    { "op1Amount", {124, 7, "", 0} },
    { "op2SourceA", {125, 7, "", 0} },
    { "op2SourceB", {126, 7, "", 0} },
    { "op3SourceA", {127, 7, "", 0} },
    { "op3SourceB", {128, 7, "", 0} },
    { "modulationKnob", {129, 7, "", 0} },
    { "eqFrequencyControl", {130, 14, "slidersArray", 14} },
    { "delayTime", {132, 14, "", 0} },
    { "delayFeedback", {134, 7, "", 0} },
    { "delaySendLevel", {135, 7, "", 0} },
    { "reverbDecay", {136, 7, "", 0} },
    { "reverbModAmount", {137, 7, "", 0} },
    { "reverbSendLevel", {138, 7, "", 0} },
    { "arpStyle", {139, 7, "", 0} },
    { "arpGateLength", {140, 7, "", 0} },
    { "arpSpeed", {141, 7, "", 0} },
    { "voiceMode", {142, 7, "voiceAssign", 0} },
    { "voiceUnisonCount", {143, 7, "voiceUnison", 0} },
    { "panSpreadMode", {144, 7, "", 0} },
    { "env1Reset", {145, 7, "Reset", 0} },
    { "env2Reset", {146, 7, "Reset", 1} },
    { "env3Reset", {147, 7, "Reset", 2} },
    // NRPN 123 not used
    { "oscPhaseReset", {149, 7, "Reset", 3} },
    { "filterType", {150, 7, "", 0} },
    { "filterCharacter", {151, 7, "", 0} },
    { "chorus", {152, 7, "", 0} },
    { "delayMode", {153, 7, "", 0} },
    // NRPN 129 not used
    { "arpOnOff", {155, 7, "", 0} },
    { "arpLatch", {156, 7, "", 0} },
    { "arpOctaveSpread", {157, 7, "", 0} },
    { "arpPatternLength", {158, 7, "", 0} },
    { "arpStep1", {159, 7, "", 0} },
    { "arpStep2", {160, 7, "", 0} },
    { "arpStep3", {161, 7, "", 0} },
    { "arpStep4", {162, 7, "", 0} },
    { "arpStep5", {163, 7, "", 0} },
    { "arpStep6", {164, 7, "", 0} },
    { "arpStep7", {165, 7, "", 0} },
    { "arpStep8", {166, 7, "", 0} },
    { "arpStep9", {167, 7, "", 0} },
    { "arpStep10", {168, 7, "", 0} },
    { "arpStep11", {169, 7, "", 0} },
    { "arpStep12", {170, 7, "", 0} },
    { "arpStep13", {171, 7, "", 0} },
    { "arpStep14", {172, 7, "", 0} },
    { "arpStep15", {173, 7, "", 0} },
    { "arpStep16", {174, 7, "", 0} }
};
