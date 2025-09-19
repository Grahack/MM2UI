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

    glideControl.onValueChanged = [](int value) {
        DBG("Nouvelle valeur : " << value);  // Console debug
        // Tu pourrais envoyer du MIDI CC ici
    };
    addAndMakeVisible(glideControl);

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
        else
        {
            DBG("⚠️ Aucune sortie MIDI active !");
        }
    }
}

