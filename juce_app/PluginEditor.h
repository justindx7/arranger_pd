#pragma once

#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

class sliderLookAndFeel : public juce::LookAndFeel_V4
{   
public:
    sliderLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::trackColourId, juce::Colours::lightgrey);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::lightgrey);
    }

    void setTrackThickness(float thickness)
    {
        trackThickness = thickness;
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);

        // You can further customize drawing here if needed, using trackThickness
    }

private:
    float trackThickness = 40.0f;

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
