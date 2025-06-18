#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& ref)
    : AudioProcessorEditor (&p), processorRef (p), Reference(ref)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    

    addAndMakeVisible(showPresetPanelButton);
    showPresetPanelButton.onClick = [this] () {showPresetPanel();};

    addAndMakeVisible(gainSlider);
    gainAttachment = std::make_unique<SliderAttachment>(Reference, "uGain", gainSlider);


    gainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    addAndMakeVisible(gainSlider);
    gainSliderLabel.setText("Gain", juce::dontSendNotification);
    gainSliderLabel.attachToComponent(&gainSlider, true);




    addAndMakeVisible(sample1);

    p.testPlayer.setSample(sample1.getSelectedFilePath());
    p.testPlayer.loadSample();

    // callback to set change current sample
    sample1.setFileSelectedCallback([&](juce::String filePath) {
            p.testPlayer.setSample(filePath);
            p.testPlayer.loadSample();
    });


    sample1.onNormalClick = [&]() {
        sample1.setPlayingState(true);
        p.testPlayer.playSample(); 
    };

    p.testPlayer.onSampleStopped = [this]() {
        sample1.setPlayingState(false);
    };


    addAndMakeVisible(sample2);

    p.testPlayer2.setSample(sample2.getSelectedFilePath());
    p.testPlayer2.loadSample();

    sample2.setFileSelectedCallback([&](juce::String filePath) {
            p.testPlayer2.setSample(filePath);
            p.testPlayer2.loadSample();
    });

    sample2.onNormalClick = [&]() {
        p.testPlayer2.playSample(); 
    };

    p.testPlayer2.onSampleStopped = [this]() {
        sample2.setPlayingState(false);
    };


    addAndMakeVisible(editModeButton);

    editModeButton.onClick = [&]() {
            sample1.setEditMode(editModeButton.getToggleState());
            sample2.setEditMode(editModeButton.getToggleState());
            introButton.setEditMode(editModeButton.getToggleState());
            for(auto &verseButton : verseButtons) {
                verseButton.setEditMode(editModeButton.getToggleState());
            }
            for(auto &fillInButton : fillInButtons) {
                fillInButton.setEditMode(editModeButton.getToggleState());
            }
            outroButton.setEditMode(editModeButton.getToggleState());
    };

    // Add arranger buttons to the editor
    addAndMakeVisible(introButton);
    for (auto& btn : verseButtons) addAndMakeVisible(btn);
    for (auto& btn : fillInButtons) addAndMakeVisible(btn);
    addAndMakeVisible(outroButton);
    p.arrangerLogic.initSections(introButton, verseButtons, fillInButtons, outroButton);
    setSize (700, 500);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Arranger", getLocalBounds(), juce::Justification::topLeft, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto sliderLeft = 120;
    gainSlider.setBounds(getWidth()/4, getHeight()/2, getWidth() - sliderLeft - 120, 120);

    showPresetPanelButton.setBounds (50, 10, 100, 30);
    sample1.setBounds (50, 40, 100, 30);
    sample2.setBounds (50, 80, 100, 30);
    editModeButton.setBounds (50, 120, 100, 30);

    // Arrange arranger buttons
    int x = 200, y = 40, w = 100, h = 30, gap = 10;
    introButton.setBounds(x, y, w, h);

    y += h + gap;
    for (auto& btn : verseButtons) {
        btn.setBounds(x, y, w, h);
        y += h + gap;
    }

    for (auto& btn : fillInButtons) {
        btn.setBounds(x + w + gap, y - (4 * (h + gap)), w, h); // Place fill-ins to the right
        y += h + gap;
    }

    outroButton.setBounds(x, y, w, h);
    if(presetPanel)
        presetPanel->setBounds(getLocalBounds());
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
