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
};
