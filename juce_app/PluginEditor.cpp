#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& ref)
    : AudioProcessorEditor (&p), processorRef (p), Reference(ref)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    juce::String sample1Path = processorRef.getAPVTS().state.getProperty("sample1Path").toString();
    sample1.setFile(sample1Path);
    p.testPlayer.setSample(sample1Path);
    p.testPlayer.loadSample();
    

    addAndMakeVisible(gainSlider);
    gainAttachment = std::make_unique<SliderAttachment>(Reference, "uGain", gainSlider);


    gainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    addAndMakeVisible(gainSlider);
    gainSliderLabel.setText("Gain", juce::dontSendNotification);
    gainSliderLabel.attachToComponent(&gainSlider, true);


    addAndMakeVisible(sample1);

    // callback to set change current sample
    sample1.setFileSelectedCallback([&](juce::String filePath) {
            p.testPlayer.setSample(filePath);
            p.testPlayer.loadSample();
            processorRef.getAPVTS().state.setProperty("sample1Path", filePath, nullptr);
    });

    sample1.onNormalClick = [&]() {
        p.testPlayer.playSample(); 
        sample1.setPlayingState(true);
    };

    p.testPlayer.onSampleStopped = [this]() {
        sample1.setPlayingState(false);
    };


    addAndMakeVisible(sample2);
    sample2.onClick = [&](){p.testPlayer.playSample();};

    addAndMakeVisible(editModeButton);
    editModeButton.onClick = [&]() {
            sample1.setEditMode(editModeButton.getToggleState());
    };

    setSize (400, 300);
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

    sample1.setBounds (50, 40, 100, 30);
    sample2.setBounds (50, 80, 100, 30);
    editModeButton.setBounds (50, 120, 100, 30);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
