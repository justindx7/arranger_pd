#pragma once

#include "PluginProcessor.h"
#include "gui/SampleButton.h"

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


    juce::Slider gainSlider;
    juce::Label gainSliderLabel;
    std::unique_ptr<SliderAttachment> gainAttachment;

    SampleButton sample1 {"sample1.wav"};
    juce::TextButton sample2{"sample2.wav"};

    juce::ToggleButton editModeButton;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
