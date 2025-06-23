#pragma once
#include <JuceHeader.h>

class OptionsPanel : public juce::Component {
public:
    OptionsPanel(juce::AudioProcessorValueTreeState& ref, std::function<void()> onClose);

    void paint(juce::Graphics&) override;
    void resized() override;
private:
    juce::AudioProcessorValueTreeState& Reference;
    std::function<void()> onCloseCallback;
    juce::TextButton closeButton;

    juce::Slider bpmSlider;
    juce::Label bpmLabel;
    juce::Label bpmValueLabel;
    juce::TextButton bpmMinusButton { "-" };
    juce::TextButton bpmPlusButton { "+" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bpmAttachment;

    // --- Added for Room Size, Damping, Width ---
    juce::Slider roomSizeSlider;
    juce::Label roomSizeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttachment;

    juce::Slider dampingSlider;
    juce::Label dampingLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dampingAttachment;

    juce::Slider widthSlider;
    juce::Label widthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
};
