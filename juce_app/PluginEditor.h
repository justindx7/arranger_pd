#pragma once

#include "PluginProcessor.h"
#include <array>
#include "arranger/ArrangerLogic.h"
#include "gui/SampleButton.h"
#include "gui/PresetPanel.h"
#include "gui/SliderLookAndFeel.h"

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


    juce::Slider bpmSlider;
    juce::Label bpmSliderLabel;
    std::unique_ptr<SliderAttachment> bpmAttachment;


    juce::Slider stretchSlider;
    juce::Label stretchSliderLabel;
    std::unique_ptr<SliderAttachment> stretchAttachment;



    juce::ToggleButton editModeButton;

    SampleButton introButton {"Intro",  Reference};

    std::array<SampleButton, 4> verseButtons{
        SampleButton{"Verse 1", Reference}, SampleButton{"Verse 2", Reference},
        SampleButton{"Verse 3", Reference}, SampleButton{"Verse 4", Reference}};

    std::array<SampleButton, 4> fillsButtons{
        SampleButton{"Fill-In 1", Reference},
        SampleButton{"Fill-In 2", Reference},
        SampleButton{"Fill-In 3", Reference},
        SampleButton{"Fill-In 4", Reference}};

    SampleButton outroButton{"Outro", Reference};

    std::array<SampleButton, 8> sampleButtons{
        SampleButton{"Sample 1", Reference}, SampleButton{"Sample 2", Reference},
        SampleButton{"Sample 3", Reference}, SampleButton{"Sample 4", Reference}, 
        SampleButton{"Sample 5", Reference}, SampleButton{"Sample 6", Reference}, 
        SampleButton{"Sample 7", Reference}, SampleButton{"Sample 8", Reference}

    };

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

    TextButton editButton { "Edit" };
    TextButton stopButton { "Stop" };

    juce::Slider tempoSlider;
    juce::Slider reverbSlider;

    juce::Label tempoLabel { "Tempo", "Tempo" };
    SliderLookAndFeel sliderLookAndFeel;

    bool isEditMode = false;

    juce::Label reverbLabel { "Reverb", "Reverb" };



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
