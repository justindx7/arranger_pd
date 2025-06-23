#include "OptionsPanel.h"

OptionsPanel::OptionsPanel(juce::AudioProcessorValueTreeState &ref, std::function<void()> onClose)
    : Reference(ref), onCloseCallback(onClose)
{
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this] { if (onCloseCallback) onCloseCallback(); };

    bpmLabel.setText("BPM", juce::dontSendNotification);
    addAndMakeVisible(bpmLabel);

    bpmSlider.setRange(40.0, 300.0, 1.0);
    bpmSlider.setTextValueSuffix(" BPM");
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(bpmSlider);

    bpmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uBPM", bpmSlider);

    addAndMakeVisible(bpmMinusButton);
    addAndMakeVisible(bpmPlusButton);
    addAndMakeVisible(bpmValueLabel);

    bpmValueLabel.setFont(juce::Font(28.0f, juce::Font::bold));
    bpmValueLabel.setJustificationType(juce::Justification::centred);
    bpmValueLabel.setText(juce::String((int)bpmSlider.getValue()), juce::dontSendNotification);

    bpmMinusButton.onClick = [this] {
        bpmSlider.setValue(bpmSlider.getValue() - 1.0);
    };
    bpmPlusButton.onClick = [this] {
        bpmSlider.setValue(bpmSlider.getValue() + 1.0);
    };
    bpmSlider.onValueChange = [this] {
        bpmValueLabel.setText(juce::String((int)bpmSlider.getValue()), juce::dontSendNotification);
    };
}

void OptionsPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(30.0f);
    g.drawFittedText("Options", getLocalBounds().removeFromTop(50),
                     juce::Justification::centred, 1);
}

void OptionsPanel::resized() {
    int y = 80, h = 60, w = 80;
    bpmLabel.setBounds(40, y - 40, 100, 30);
    bpmMinusButton.setBounds(40, y, w, h);
    bpmValueLabel.setBounds(40 + w, y, w, h);
    bpmPlusButton.setBounds(40 + 2*w, y, w, h);
    bpmSlider.setBounds(40, y + h + 10, getWidth() - 80, 40);
    closeButton.setBounds(getWidth() - 120, 10, 100, 50);

}
