class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        juce::Colour mm2 = juce::Colour (0xffF76D22);
        setColour (juce::Slider::thumbColourId, juce::Colours::red);
        setColour (juce::ComboBox::backgroundColourId, mm2);
        setColour (juce::ComboBox::arrowColourId, juce::Colours::black);
        setColour (juce::ComboBox::textColourId, juce::Colours::black);
    }

    Font getComboBoxFont (ComboBox& box) override
    {
        return Font(FontOptions(22.f, juce::Font::bold));
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // fill
        g.setColour (juce::Colour(0xffF76D22));
        g.fillEllipse (rx, ry, rw, rw);

        // outline
        g.setColour (juce::Colours::red);
        g.drawEllipse (rx, ry, rw, rw, 1.0f);

        juce::Path p;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

        // pointer
        g.setColour (juce::Colours::yellow);
        g.fillPath (p);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour,
                               bool, bool isButtonDown) override
    {
        auto buttonArea = button.getLocalBounds();
        auto edge = 4;

        buttonArea.removeFromLeft (edge);
        buttonArea.removeFromTop (edge);

        // shadow
        g.setColour (juce::Colours::darkgrey.withAlpha (0.5f));
        g.fillRect (buttonArea);

        auto offset = isButtonDown ? -edge / 2 : -edge;
        buttonArea.translate (offset, offset);

        g.setColour (backgroundColour);
        g.fillRect (buttonArea);
    }

    void drawLabel(Graphics& g, Label& label) override
    {
        g.fillAll(label.findColour(Label::backgroundColourId));
        g.setColour(label.findColour(Label::textColourId));
        g.setFont(label.getFont());
        g.drawText(label.getText(), label.getLocalBounds(),
                   label.getJustificationType(),
                   true);
        if (!isLabelInComboBox(label))
        {
            g.setColour(juce::Colours::black);
            g.drawRect(label.getLocalBounds(), 1);
        }
    }

private:
    bool isLabelInComboBox(juce::Label& label)
    {
        auto* parent = label.getParentComponent();
        while (parent != nullptr)
        {
            if (dynamic_cast<juce::ComboBox*>(parent) != nullptr)
                return true;
            parent = parent->getParentComponent();
        }
        return false;
    }
};
