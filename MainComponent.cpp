#include "MainComponent.h"

MainComponent::MainComponent()
{
    setSize(400, 300);

    myButton.addListener(this);
    addAndMakeVisible(myButton);

    addAndMakeVisible(draggable);
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
    draggable.setTopLeftPosition(10, 70);
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &myButton)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::InfoIcon,
                                               "Bouton cliqué", "Tu as cliqué le bouton !");
    }
}

