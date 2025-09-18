#pragma once

#include <JuceHeader.h>

class DraggableItem : public juce::Component
{
public:
    DraggableItem(const juce::String& labelText)
        : label(labelText, labelText)
    {
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
        setSize(100, 40);
        setColour(juce::Component::backgroundColourId, juce::Colours::lightblue);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(findColour(juce::Component::backgroundColourId));
        g.setColour(juce::Colours::black);
        g.drawRect(getLocalBounds(), 2);
    }

    void resized() override
    {
        label.setBounds(getLocalBounds());
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        dragStartPos = getPosition();
        mouseDownPos = event.getPosition();
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        auto delta = event.getPosition() - mouseDownPos;
        setTopLeftPosition(dragStartPos + delta);
    }

private:
    juce::Label label;
    juce::Point<int> dragStartPos;
    juce::Point<int> mouseDownPos;
};

