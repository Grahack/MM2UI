#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400, 300);

    myButton.addListener(this);
    addAndMakeVisible(myButton);

    statusLabel.setText("OFF", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightgrey);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    addAndMakeVisible(statusLabel);

    glideControl.onValueChanged = [](int value) {
        DBG("Nouvelle valeur : " << value);  // Console debug
        // Tu pourrais envoyer du MIDI CC ici
    };
    addAndMakeVisible(glideControl);

    // Initialiser le MIDI
    auto midiOutputs = juce::MidiOutput::getAvailableDevices();
    if (!midiOutputs.isEmpty())
    {
        auto deviceInfo = midiOutputs[0]; // ← Utilise le premier port MIDI dispo
        midiOut = juce::MidiOutput::openDevice(deviceInfo.identifier);

        if (midiOut != nullptr)
            DBG("Port MIDI ouvert : " + deviceInfo.name);
        else
            DBG("Erreur d'ouverture MIDI");
    }
    else
    {
        DBG("Aucun périphérique MIDI trouvé");
    }
}

MainComponent::~MainComponent()
{
    myButton.removeListener(this);
    midiOut.reset(); // Ferme proprement le port MIDI
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    myButton.setBounds(10, 10, 50, 40);
    statusLabel.setBounds(60, 10, 80, 40);
    glideControl.setBounds(10, 50, 60, 170);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &myButton)
    {

        bool isOn = myButton.getToggleState();

        statusLabel.setText(isOn ? "ON" : "OFF",
                            juce::dontSendNotification);

        if (midiOut != nullptr)
        {
            int midiChannel = 1;
            int controllerNumber = 64;
            int value = isOn ? 127 : 0;

            auto msg = juce::MidiMessage::controllerEvent(
                           midiChannel, controllerNumber, value);
            midiOut->sendMessageNow(msg);

            DBG("CC" << controllerNumber << " = " << value);
        }
    }
}

