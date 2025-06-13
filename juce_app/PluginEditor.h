#pragma once

#include "PluginProcessor.h"
#include <array>
#include "arranger/ArrangerLogic.h"
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
    SampleButton sample2{"sample2.wav"};

    juce::ToggleButton editModeButton;

    SampleButton introButton {"Intro"};
    std::array<SampleButton, 4> verseButtons { SampleButton{"Verse 1"}, SampleButton{"Verse 2"}, SampleButton{"Verse 3"}, SampleButton{"Verse 4"} };
    std::array<SampleButton, 4> fillInButtons { SampleButton{"Fill-In 1"}, SampleButton{"Fill-In 2"}, SampleButton{"Fill-In 3"}, SampleButton{"Fill-In 4"} };
    SampleButton outroButton {"Outro"};



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
