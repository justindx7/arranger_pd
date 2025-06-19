#pragma once

#include "handlers/AudioFileHandler.h"
#include "handlers/MidiHandler.h"
#include "arranger/ArrangerLogic.h"
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

    static constexpr int numParameters = 2;

    std::atomic<float>* bpmParameter = nullptr;
    std::atomic<float>* stretchParameter = nullptr;

    juce::NormalisableRange<float> bpmRange = {20.0f, 200.0, 1.0f};
    juce::NormalisableRange<float> stretchRange = {-10.0f, 10.0, 1.0f};

    float startingValues[numParameters] = {100.f, 0.f};

    juce::NormalisableRange<float> normalisableRanges[numParameters] = {bpmRange,stretchRange};

    std::string parameterNames[numParameters] = {"BPM", "Stretch"};

    juce::ParameterID bpmParam{"uBPM", 1};
    juce::ParameterID stretchParam{"uStretch", 1};

    juce::ParameterID parameterIDs[numParameters] = {bpmParam, stretchParam};

      
    juce::dsp::Limiter<float> limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
