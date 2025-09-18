#pragma once

#include <JuceHeader.h>

class GlideControl : public juce::Component
{
public:
    GlideControl(const juce::String& name) : label(name, name)
    {
        addAndMakeVisible(label);
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::black);

        valueLabel.setText("0", juce::dontSendNotification);
        valueLabel.setJustificationType(juce::Justification::centred);
        valueLabel.setColour(juce::Label::backgroundColourId, juce::Colours::white);
        valueLabel.setColour(juce::Label::textColourId, juce::Colours::black);
        addAndMakeVisible(valueLabel);

        setMouseCursor(juce::MouseCursor::UpDownResizeCursor); // Curseur visuel
    }

    void resized() override
    {
        auto area = getLocalBounds();
        label.setBounds(area.removeFromTop(20));
        valueLabel.setBounds(area.removeFromBottom(20));
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::lightgrey);
        g.setColour(juce::Colours::darkgrey);
        g.drawRect(getLocalBounds(), 2);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        dragStartY = event.getPosition().y;
        startValue = currentValue;
    }

    void mouseDrag(const juce::MouseEvent& event) override
    {
        int deltaY = event.getPosition().y - dragStartY;
        int newValue = juce::jlimit(0, 127, startValue - deltaY); // Haut = +, Bas = -
        if (newValue != currentValue)
        {
            currentValue = newValue;
            valueLabel.setText(juce::String(currentValue), juce::dontSendNotification);

            if (onValueChanged != nullptr)
                onValueChanged(currentValue);
        }
    }

    void setValue(int value)
    {
        currentValue = juce::jlimit(0, 127, value);
        valueLabel.setText(juce::String(currentValue), juce::dontSendNotification);
    }

    int getValue() const { return currentValue; }

    std::function<void(int)> onValueChanged;  // Callback utilisateur

private:
    juce::Label label;
    juce::Label valueLabel;
    int currentValue = 0;
    int dragStartY = 0;
    int startValue = 0;
};

