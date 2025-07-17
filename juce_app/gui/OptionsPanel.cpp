#include "OptionsPanel.h"


OptionsPanel::OptionsPanel(juce::AudioProcessorValueTreeState &ref, std::function<void()> onClose)
    : Reference(ref), onCloseCallback(onClose)
{
    // Close button (always visible)
    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this] { if (onCloseCallback) onCloseCallback(); };

    // Navigation buttons
    addAndMakeVisible(prevButton);
    addAndMakeVisible(nextButton);
    addAndMakeVisible(pageLabel);
    
    prevButton.onClick = [this] {
        int newPage = static_cast<int>(currentPage) - 1;
        if (newPage < 0) newPage = static_cast<int>(Page::TOTAL_PAGES) - 1;
        switchToPage(static_cast<Page>(newPage));
    };
    
    nextButton.onClick = [this] {
        int newPage = (static_cast<int>(currentPage) + 1) % static_cast<int>(Page::TOTAL_PAGES);
        switchToPage(static_cast<Page>(newPage));
    };
    
    pageLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    pageLabel.setJustificationType(juce::Justification::centred);

    // PAGE 1: Tempo & Reverb Controls
    bpmLabel.setText("BPM", juce::dontSendNotification);
    addChildComponent(bpmLabel);

    bpmSlider.setTextValueSuffix(" BPM");
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addChildComponent(bpmSlider);

    bpmAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uBPM", bpmSlider);

    addChildComponent(bpmMinusButton);
    addChildComponent(bpmPlusButton);
    addChildComponent(bpmValueLabel);

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

    roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    addChildComponent(roomSizeLabel);
    roomSizeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addChildComponent(roomSizeSlider);
    roomSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uRoomSize", roomSizeSlider);

    dampingLabel.setText("Damping", juce::dontSendNotification);
    addChildComponent(dampingLabel);
    dampingSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addChildComponent(dampingSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uDamping", dampingSlider);

    widthLabel.setText("Width", juce::dontSendNotification);
    addChildComponent(widthLabel);
    widthSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    addChildComponent(widthSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uWidth", widthSlider);

    // PAGE 2: Volume Controls
    highpassLabel.setText("Highpass (Hz)", juce::dontSendNotification);
    addChildComponent(highpassLabel);
    highpassSlider.setRange(20.0, 2000.0, 1.0);
    highpassSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    highpassSlider.setTextValueSuffix(" Hz");
    addChildComponent(highpassSlider);
    highpassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uHighpassFreq", highpassSlider);

    gainLabel.setText("Arranger Volume", juce::dontSendNotification);
    addChildComponent(gainLabel);
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setTextValueSuffix(" dB");
    addChildComponent(gainSlider);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        Reference, "uGain", gainSlider);

    // PAGE 3: Sample Volumes
    for (int i = 0; i < 8; ++i) {
        auto name = "Sample" + std::to_string(i + 1) + " Volume";
        auto paramID = "uGain" + std::to_string(i + 1);

        gainSampleLabels[i].setText(name, juce::dontSendNotification);
        addChildComponent(gainSampleLabels[i]);

        gainSampleSliders[i].setSliderStyle(juce::Slider::LinearHorizontal);
        gainSampleSliders[i].setTextValueSuffix(" dB");
        addChildComponent(gainSampleSliders[i]);

        gainSampleAttachments[i] = std::make_unique<
            juce::AudioProcessorValueTreeState::SliderAttachment>(
            Reference, paramID, gainSampleSliders[i]);
    }

    // PAGE 4: Audio Settings
    addChildComponent(audioSettingsButton);
    audioSettingsButton.onClick = []() {
        juce::StandalonePluginHolder::getInstance()->showAudioSettingsDialog();
    };
    
    audioSettingsLabel.setText("Click to open audio device settings", juce::dontSendNotification);
    audioSettingsLabel.setJustificationType(juce::Justification::centred);
    addChildComponent(audioSettingsLabel);

    // Initialize page visibility
    updatePageVisibility();
}

void OptionsPanel::updatePageVisibility() {
    // Hide all page components first
    bpmLabel.setVisible(false);
    bpmSlider.setVisible(false);
    bpmMinusButton.setVisible(false);
    bpmPlusButton.setVisible(false);
    bpmValueLabel.setVisible(false);
    roomSizeLabel.setVisible(false);
    roomSizeSlider.setVisible(false);
    dampingLabel.setVisible(false);
    dampingSlider.setVisible(false);
    widthLabel.setVisible(false);
    widthSlider.setVisible(false);
    
    highpassLabel.setVisible(false);
    highpassSlider.setVisible(false);
    gainLabel.setVisible(false);
    gainSlider.setVisible(false);
    
    for (int i = 0; i < 8; ++i) {
        gainSampleLabels[i].setVisible(false);
        gainSampleSliders[i].setVisible(false);
    }
    
    audioSettingsButton.setVisible(false);
    audioSettingsLabel.setVisible(false);
    
    // Show components for current page
    switch (currentPage) {
        case Page::TEMPO_REVERB:
            bpmLabel.setVisible(true);
            bpmSlider.setVisible(true);
            bpmMinusButton.setVisible(true);
            bpmPlusButton.setVisible(true);
            bpmValueLabel.setVisible(true);
            roomSizeLabel.setVisible(true);
            roomSizeSlider.setVisible(true);
            dampingLabel.setVisible(true);
            dampingSlider.setVisible(true);
            widthLabel.setVisible(true);
            widthSlider.setVisible(true);
            highpassLabel.setVisible(true);
            highpassSlider.setVisible(true);
            break;
            
        case Page::VOLUME_CONTROLS:
            gainLabel.setVisible(true);
            gainSlider.setVisible(true);
            for (int i = 0; i < 8; ++i) {
                gainSampleLabels[i].setVisible(true);
                gainSampleSliders[i].setVisible(true);
            }
            break;
            
        case Page::AUDIO_SETTINGS:
            audioSettingsButton.setVisible(true);
            audioSettingsLabel.setVisible(true);
            break;
            
        default:
            break;
    }
    
    pageLabel.setText(getPageTitle(), juce::dontSendNotification);
}

void OptionsPanel::switchToPage(Page newPage) {
    currentPage = newPage;
    updatePageVisibility();
    resized(); // Trigger layout update
}

juce::String OptionsPanel::getPageTitle() const {
    switch (currentPage) {
        case Page::TEMPO_REVERB: return "Tempo & Reverb (1/3)";
        case Page::VOLUME_CONTROLS: return "Volume Controls (2/3)";
        case Page::AUDIO_SETTINGS: return "Audio Settings (3/3)";
        default: return "Options";
    }
}

void OptionsPanel::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.setFont(30.0f);
    g.drawFittedText("Options", getLocalBounds().removeFromTop(50),
                     juce::Justification::centred, 1);
}

void OptionsPanel::resized() {
    const int margin = 20;
    const int buttonHeight = 40;
    const int sliderHeight = 40;
    const int labelHeight = 30;
    const int spacing = 10;
    
    // Navigation area at bottom
    int navY = getHeight() - 60;
    prevButton.setBounds(margin, navY, 100, buttonHeight);
    nextButton.setBounds(getWidth() - margin - 100, navY, 100, buttonHeight);
    pageLabel.setBounds(margin + 110, navY, getWidth() - 2 * margin - 220, buttonHeight);
    
    // Close button
    closeButton.setBounds(getWidth() - 120, 10, 100, 50);
    
    // Content area
    int contentY = 80;
    int contentHeight = navY - contentY - margin;
    int sliderWidth = getWidth() - 2 * margin - 140;
    
    switch (currentPage) {
        case Page::TEMPO_REVERB: {
            // BPM controls
            int bpmY = contentY;
            bpmLabel.setBounds(margin, bpmY - 40, 100, labelHeight);
            
            int buttonW = 60;
            bpmMinusButton.setBounds(margin, bpmY, buttonW, buttonHeight);
            bpmValueLabel.setBounds(margin + buttonW, bpmY, buttonW, buttonHeight);
            bpmPlusButton.setBounds(margin + 2 * buttonW, bpmY, buttonW, buttonHeight);
            bpmSlider.setBounds(margin, bpmY + buttonHeight + spacing, getWidth() - 2 * margin, sliderHeight);
            
            // Reverb controls
            int reverbY = bpmY + buttonHeight + sliderHeight + 40;
            roomSizeLabel.setBounds(margin, reverbY, 120, labelHeight);
            roomSizeSlider.setBounds(margin + 140, reverbY, sliderWidth, sliderHeight);
            
            dampingLabel.setBounds(margin, reverbY + 50, 120, labelHeight);
            dampingSlider.setBounds(margin + 140, reverbY + 50, sliderWidth, sliderHeight);
            
            widthLabel.setBounds(margin, reverbY + 100, 120, labelHeight);
            widthSlider.setBounds(margin + 140, reverbY + 100, sliderWidth, sliderHeight);

            highpassLabel.setBounds(margin, reverbY + 150, 120, labelHeight);
            highpassSlider.setBounds(margin + 140, reverbY + 150, sliderWidth, sliderHeight);

            break;
        }
        
        case Page::VOLUME_CONTROLS: {
            int y = contentY;
            
            gainLabel.setBounds(margin, y, 120, labelHeight);
            gainSlider.setBounds(margin + 140, y, sliderWidth, sliderHeight);
            
            y = y + 70;
            int itemHeight = 50;
            for (int i = 0; i < 8; ++i) {
                gainSampleLabels[i].setBounds(margin, y + i * itemHeight, 140, labelHeight);
                gainSampleSliders[i].setBounds(margin + 150, y + i * itemHeight, sliderWidth - 10, sliderHeight);
            }
            break;
        }
        
        case Page::AUDIO_SETTINGS: {
            int y = contentY + 50;
            audioSettingsLabel.setBounds(margin, y, getWidth() - 2 * margin, labelHeight);
            audioSettingsButton.setBounds((getWidth() - 200) / 2, y + 60, 200, buttonHeight * 1.5);
            break;
        }
        
        default:
            break;
    }
}
