#pragma once

#include "handlers/AudioFileHandler.h"
#include "handlers/MidiHandler.h"
#include "arranger/ArrangerLogic.h"
#include "service/PresetManager.h"
#include <JuceHeader.h>
#include <memory>

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
    std::array<std::unique_ptr<AudioFileHandler>,8>& getSamplePlayers() { return samplePlayers; }

    ArrangerLogic& getArrangerLogic(){return arrangerLogic;}
    MidiHandler& getMidiHandler(){return midiHandler;}


  private:
    //==============================================================================
    std::array<std::unique_ptr<AudioFileHandler>,8> samplePlayers;     

    ArrangerLogic arrangerLogic;
    std::shared_ptr<PresetManager> presetManager;

    MidiHandler midiHandler;
    
    juce::AudioProcessorValueTreeState parameters;

    static constexpr int numParameters = 8; 

    std::atomic<float>* bpmParameter = nullptr;
    std::atomic<float>* stretchParameter = nullptr;

    std::atomic<float>* dryWetParameter = nullptr;
    std::atomic<float>* roomSizeParameter = nullptr;
    std::atomic<float>* dampingParameter = nullptr;
    std::atomic<float>* widthParameter = nullptr;
    std::atomic<float>* highpassFreqParameter = nullptr; 
    std::atomic<float>* gainParameter = nullptr; 

    juce::NormalisableRange<float> bpmRange = {20.0f, 200.0f, 1.0f};
    juce::NormalisableRange<float> stretchRange = {-10.0f, 10.0f, 1.0f};
    juce::NormalisableRange<float> drywetRange = {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> roomSizeRange = {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> dampingRange = {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> widthRange = {0.0f, 100.0f, 1.0f};
    juce::NormalisableRange<float> highpassFreqRange = {20.0f, 20000.0f, 1.0f}; 
    juce::NormalisableRange<float> gainRange = {-24.0f, 6.0f, 0.1f}; 


    float startingValues[numParameters] = {100.f, 0.f, 0.f, 50.f, 50.0f, 100.0f, 20.0f, 0.0f};

    juce::NormalisableRange<float> normalisableRanges[numParameters] = {
      bpmRange, stretchRange, drywetRange, roomSizeRange, dampingRange, widthRange, highpassFreqRange, gainRange
    };

    std::string parameterNames[numParameters] = {
      "BPM", "Stretch", "Reverb", "RoomSize", "Damping", "Width", "HighpassFreq", "Gain" 
    };

    juce::ParameterID bpmParam{"uBPM", 1};
    juce::ParameterID stretchParam{"uStretch", 1};
    juce::ParameterID drywetParam{"uDryWet", 1};
    juce::ParameterID roomSizeParam{"uRoomSize", 1};
    juce::ParameterID dampingParam{"uDamping", 1};
    juce::ParameterID widthParam{"uWidth", 1};
    juce::ParameterID highpassFreqParam{"uHighpassFreq", 1}; 
    juce::ParameterID gainParam{"uGain", 1}; 

    juce::ParameterID parameterIDs[numParameters] = {
      bpmParam, stretchParam, drywetParam, roomSizeParam, dampingParam, widthParam, highpassFreqParam, gainParam
    };

    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    juce::dsp::StateVariableTPTFilter<float> lowShelfFilter;
    juce::dsp::StateVariableTPTFilter<float> peakingFilter;
    juce::dsp::StateVariableTPTFilter<float> highPass;
    

    // Temporary buffers for wet/dry separation
    juce::AudioBuffer<float> wetBuffer;
    juce::AudioBuffer<float> dryBuffer;
    
    // Mix levels
    float dryMix = 1.0f;
    float wetMix = 0.0f;

    juce::dsp::Limiter<float> limiter;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
