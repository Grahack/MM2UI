#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize (400, 300);

    // Bouton cliquable
    myButton.addListener(this);
    addAndMakeVisible(myButton);

    // Élément draggable
    draggableLabel.setText("Fais glisser ceci", juce::dontSendNotification);
    draggableLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightblue);
    draggableLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    draggableLabel.setInterceptsMouseClicks(true, true);
    draggableLabel.setMouseDragSensitivity(2);
    draggableLabel.setEditable(false, false, false);
    addAndMakeVisible(draggableLabel);
}

MainComponent::~MainComponent()
{
    myButton.removeListener(this);
}

void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::white);
}

void MainComponent::resized()
{
    myButton.setBounds (10, 10, 150, 40);
    draggableLabel.setBounds (10, 60, 150, 40);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &myButton)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                               "Bouton cliqué", "Tu as cliqué le bouton !");
    }
}

