#pragma once

#include "handlers/AudioFileHandler.h"
#include "handlers/MidiHandler.h"
#include "arranger/ArrangerLogic.h"
#include "juce_dsp/juce_dsp.h"
#include "service/PresetManager.h"
#include <JuceHeader.h>

//==============================================================================
class AudioPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    PresetManager& getPresetManager(){return *presetManager;}

    AudioFileHandler testPlayer;
    AudioFileHandler testPlayer2;

    ArrangerLogic arrangerLogic;

  private:
    //==============================================================================
     

    std::unique_ptr<PresetManager> presetManager;

    MidiHandler midiHandler;
    
    juce::AudioProcessorValueTreeState parameters;

    static constexpr int numParameters = 1;

    std::atomic<float>* gainParameter = nullptr;

    juce::NormalisableRange<float> gainRange = {0.0f, 1.0, 0.01f};

    float startingValues[numParameters] = {0.f};

    juce::NormalisableRange<float> normalisableRanges[numParameters] = {gainRange};

    std::string parameterNames[numParameters] = {"Gain"};

    juce::ParameterID gainParam{"uGain", 1};

    juce::ParameterID parameterIDs[numParameters] = {gainParam};

      
    juce::dsp::Limiter<float> limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
