#pragma once

#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

class sliderLookAndFeel : public juce::LookAndFeel_V4
{   
public:
    sliderLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colours::red);
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

        // Draw thumb
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        juce::Path thumb;
        if (style == juce::Slider::LinearHorizontal)
            thumb.addEllipse(sliderPos - 5, y + height / 2.0f - 5, 10, 10);
        else if (style == juce::Slider::LinearVertical)
            thumb.addEllipse(x + width / 2.0f - 5, sliderPos - 5, 10, 10);

        g.fillPath(thumb);
        
    }

private:
    float trackThickness = 10.0f; 

    ~sliderLookAndFeel() override {}
};

//==============================================================================
class AudioPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&, juce::AudioProcessorValueTreeState& Reference);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    juce::AudioProcessorValueTreeState& Reference;

    //===============================================================================
    //buttons
    
    std::vector<juce::TextButton> sampleButtons { 8 };
    std::vector<juce::TextButton> fillsButtons { 4 };
    std::vector<juce::TextButton> verseButtons { 4 };

    TextButton introButton { "Intro" };
    TextButton outroButton { "Outro" };

    TextButton editButton { "Edit" };
    TextButton stopButton { "Stop" };

    //==============================================================================
    // sliders

    juce::Slider tempoSlider;
    juce::Slider reverbSlider;

    juce::Label tempoLabel { "Tempo", "Tempo" };
    juce::Label reverbLabel { "Reverb", "Reverb" };


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
