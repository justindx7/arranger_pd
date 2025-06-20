#pragma once
#include <JuceHeader.h>

class SliderLookAndFeel : public juce::LookAndFeel_V4
{   
public:
    SliderLookAndFeel()
    {
        // Set the colours for the slider
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colours::red);

        //text box setttings
        // However this is redundant as the text box is not used in the slider 
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::lightgrey);
        trackThickness = 10.0f; // Three times the original thickness (3 * 40.0f)
    }

    void setTrackThickness(float thickness)
    {
        trackThickness = thickness;
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        // Draw the background
        g.setColour(slider.findColour(juce::Slider::trackColourId));

        if (style == juce::Slider::LinearHorizontal)
        {
            float cy = y + height / 2.0f - trackThickness / 2.0f;
            g.fillRect((float)x, cy, (float)width, trackThickness);
        }
        else if (style == juce::Slider::LinearVertical)
        {
            float cx = x + width / 2.0f - trackThickness / 2.0f;
            g.fillRect(cx, (float)y, trackThickness, (float)height);
        }

        const int thSize = 23; // Size of the thumb
        // Draw thumb
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        juce::Path thumb;
        if (style == juce::Slider::LinearHorizontal)
        {
            float cy = y + height / 2.0f;
            thumb.addEllipse(sliderPos - thSize / 2.0f, cy - thSize / 2.0f, thSize, thSize);
        }
        else if (style == juce::Slider::LinearVertical)
        {
            float cx = x + width / 2.0f;
            thumb.addEllipse(cx - thSize / 2.0f, sliderPos - thSize / 2.0f, thSize, thSize);
        }
        g.fillPath(thumb);
    }


private:
    float trackThickness = 10.0f; 

};
