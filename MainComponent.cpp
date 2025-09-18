#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400, 300);

    myButton.addListener(this);
    addAndMakeVisible(myButton);

    glideControl.onValueChanged = [](int value) {
        DBG("Nouvelle valeur : " << value);  // Console debug
        // Tu pourrais envoyer du MIDI CC ici
    };
    addAndMakeVisible(glideControl);
}

MainComponent::~MainComponent()
{
    myButton.removeListener(this);
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
}

void MainComponent::resized()
{
    myButton.setBounds(10, 10, 150, 40);
    glideControl.setBounds(20, 20, 100, 100);

}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &myButton)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                               "Bouton cliqué", "Tu as cliqué le bouton !");
    }
}

