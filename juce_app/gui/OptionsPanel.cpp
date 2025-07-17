#include "OptionsPanel.h"


OptionsPanel::OptionsPanel(juce::AudioProcessorValueTreeState &ref, std::function<void()> onClose)
    : Reference(ref), onCloseCallback(onClose)
{
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this] { if (onCloseCallback) onCloseCallback(); };

    bpmLabel.setText("BPM", juce::dontSendNotification);
    addAndMakeVisible(bpmLabel);

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

    // --- Room Size Slider ---
    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    addAndMakeVisible(roomSizeLabel);
    roomSizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(roomSizeSlider);
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uRoomSize", roomSizeSlider);

    // --- Damping Slider ---
    dampingLabel.setText("Damping", juce::dontSendNotification);
    addAndMakeVisible(dampingLabel);
    dampingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(dampingSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uDamping", dampingSlider);

    // --- Width Slider ---
    widthLabel.setText("Width", juce::dontSendNotification);
    addAndMakeVisible(widthLabel);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addAndMakeVisible(widthSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uWidth", widthSlider);

    // --- Highpass Frequency Slider ---
    highpassLabel.setText("Highpass (Hz)", juce::dontSendNotification);
    addAndMakeVisible(highpassLabel);
    highpassSlider.setRange(20.0, 2000.0, 1.0);
    highpassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highpassSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(highpassSlider);
    highpassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uHighpassFreq", highpassSlider);

    // Arranger section kit gain
    //
    gainLabel.setText("Arranger Volume", juce::dontSendNotification);
    addAndMakeVisible(gainLabel);
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uGain", gainSlider);

    // Audio settings button
    addAndMakeVisible(audioSettingsButton);
    audioSettingsButton.onClick = []() {
    juce::StandalonePluginHolder::getInstance()->showAudioSettingsDialog();
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

    int sliderY = y + h + 60;
    int sliderH = 40;
    roomSizeLabel.setBounds(40, sliderY, 120, 30);
    roomSizeSlider.setBounds(160, sliderY, getWidth() - 200, sliderH);

    dampingLabel.setBounds(40, sliderY + 50, 120, 30);
    dampingSlider.setBounds(160, sliderY + 50, getWidth() - 200, sliderH);

    widthLabel.setBounds(40, sliderY + 100, 120, 30);
    widthSlider.setBounds(160, sliderY + 100, getWidth() - 200, sliderH);

    highpassLabel.setBounds(40, sliderY + 150, 120, 30);
    highpassSlider.setBounds(160, sliderY + 150, getWidth() - 200, sliderH);

    gainLabel.setBounds(40, sliderY + 250, 120, 30);
    gainSlider.setBounds(160, sliderY + 250, getWidth() - 200, sliderH);

    audioSettingsButton.setBounds(40, sliderY + 400, w*2, h);

    closeButton.setBounds(getWidth() - 120, 10, 100, 50);
}
