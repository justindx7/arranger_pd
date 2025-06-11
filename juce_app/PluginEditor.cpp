#include "PluginProcessor.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& ref)
    : AudioProcessorEditor (&p), processorRef (p), Reference(ref)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(gainSlider);
    gainAttachment = std::make_unique<SliderAttachment>(Reference, "uGain", gainSlider);


    gainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    addAndMakeVisible(gainSlider);
    gainSliderLabel.setText("Gain", juce::dontSendNotification);
    gainSliderLabel.attachToComponent(&gainSlider, true);

    addAndMakeVisible(sample1);
    sample1.onClick = [&](){p.testPlayer.playSample();};

    addAndMakeVisible(sample2);
    sample2.onClick = [&](){p.testPlayer.playSample();};

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
    sample2.setBounds (50, JUCE_LIVE_CONSTANT(80), 100, 30);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
