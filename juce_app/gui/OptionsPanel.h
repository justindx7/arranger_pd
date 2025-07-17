#pragma once
#include <JuceHeader.h>
#include "../CustomStandaloneFilterWindow.h"


// OptionsPanel.h additions
class OptionsPanel : public juce::Component
{
public:
    OptionsPanel(juce::AudioProcessorValueTreeState &ref, std::function<void()> onClose);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    enum class Page {
        TEMPO_REVERB = 0,
        VOLUME_CONTROLS = 1,
        SAMPLE_VOLUMES = 2,
        AUDIO_SETTINGS = 3,
        TOTAL_PAGES = 4
    };

    Page currentPage = Page::TEMPO_REVERB;
    
    // Navigation
    juce::TextButton prevButton {"<"};
    juce::TextButton nextButton {">"};
    juce::Label pageLabel;
    
    // Existing members...
    juce::AudioProcessorValueTreeState &Reference;
    std::function<void()> onCloseCallback;
    juce::TextButton closeButton {"Close"};
    
    // Page 1: Tempo & Reverb
    juce::Label bpmLabel;
    juce::Slider bpmSlider;
    juce::TextButton bpmMinusButton {"-"};
    juce::TextButton bpmPlusButton {"+"};
    juce::Label bpmValueLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAttachment;
    
    juce::Label roomSizeLabel;
    juce::Slider roomSizeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;
    
    juce::Label dampingLabel;
    juce::Slider dampingSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;
    
    juce::Label widthLabel;
    juce::Slider widthSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    
    // Page 2: Volume Controls
    juce::Label highpassLabel;
    juce::Slider highpassSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highpassAttachment;
    
    juce::Label gainLabel;
    juce::Slider gainSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    
    // Page 3: Sample Volumes
    juce::Label gainSampleLabels[8];
    juce::Slider gainSampleSliders[8];
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainSampleAttachments[8];
    
    // Page 4: Audio Settings
    juce::TextButton audioSettingsButton {"Audio Settings"};
    juce::Label audioSettingsLabel;
    
    void updatePageVisibility();
    void switchToPage(Page newPage);
    juce::String getPageTitle() const;
};
