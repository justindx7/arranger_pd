#pragma once

#include "PluginProcessor.h"
#include <array>
#include "arranger/ArrangerLogic.h"
#include "gui/SampleButton.h"
#include "gui/PresetPanel.h"

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
private: // This reference is provided as a quick way for your editor to access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    juce::AudioProcessorValueTreeState& Reference;


    juce::Slider gainSlider;
    juce::Label gainSliderLabel;
    std::unique_ptr<SliderAttachment> gainAttachment;

    SampleButton sample1 {"sample1.wav", Reference};
    SampleButton sample2{"sample2.wav", Reference};

    juce::ToggleButton editModeButton;

    SampleButton introButton {"Intro",  Reference};
    std::array<SampleButton, 4> verseButtons { SampleButton{"Verse 1", Reference}, SampleButton{"Verse 2", Reference}, SampleButton{"Verse 3", Reference}, SampleButton{"Verse 4", Reference} };
    std::array<SampleButton, 4> fillInButtons { SampleButton{"Fill-In 1", Reference}, SampleButton{"Fill-In 2", Reference}, SampleButton{"Fill-In 3", Reference}, SampleButton{"Fill-In 4", Reference} };
    SampleButton outroButton {"Outro", Reference};

    juce::TextButton showPresetPanelButton{"Presets"};
    std::unique_ptr<PresetPanel> presetPanel;

    void showPresetPanel() {
      presetPanel = std::make_unique<PresetPanel>(processorRef.getPresetManager(), [this] {
        presetPanel.reset();
        // Optionally repaint or update UI
      });
      addAndMakeVisible(*presetPanel);
      presetPanel->setBounds(getLocalBounds());
      presetPanel->toFront(true);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
