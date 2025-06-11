#pragma once

#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"

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
