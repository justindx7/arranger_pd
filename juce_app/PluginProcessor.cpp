#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <memory>

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr) {
    for (auto i = 0; i < numParameters; i++) {
        parameters.createAndAddParameter(std::make_unique<juce::AudioParameterFloat>(parameterIDs[i],
                                                                                     parameterNames[i],
                                                                                     normalisableRanges[i],
                                                                                     startingValues[i]));   
    }

    parameters.replaceState(juce::ValueTree("Parameters"));

    bpmParameter = parameters.getRawParameterValue("uBPM");
    stretchParameter = parameters.getRawParameterValue("uStretch");              

    dryWetParameter = parameters.getRawParameterValue("uDryWet"); 

    roomSizeParameter = parameters.getRawParameterValue("uRoomSize"); 
    dampingParameter = parameters.getRawParameterValue("uDamping");
    widthParameter = parameters.getRawParameterValue("uWidth");
    highpassFreqParameter = parameters.getRawParameterValue("uHighpassFreq");

    parameters.state.setProperty(PresetManager::presetNameProperty, "", nullptr);
    parameters.state.setProperty("version", ProjectInfo::versionString, nullptr);
    presetManager = std::make_unique<PresetManager>(parameters);

    for(int i = 0; i < samplePlayers.max_size(); i++) {
        samplePlayers[i] = std::make_unique<AudioFileHandler>();
    }

}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{

}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    arrangerLogic.prepareToPlay(sampleRate,samplesPerBlock);

    for(auto &player : samplePlayers) {
        player->prepareToPlay(sampleRate,samplesPerBlock);
        arrangerLogic.getMixer().addInputSource(player->getSource(), false);
    }

    juce::dsp::ProcessSpec spec;

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    limiter.reset();
    limiter.prepare(spec);

    limiter.setThreshold(-0.1f); // dB
    limiter.setRelease(50.0f);   // ms


    reverb.reset();
    reverb.prepare(spec);

    highPass.prepare(spec);
    highPass.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highPass.setCutoffFrequency(highpassFreqParameter->load()); 

    wetBuffer.setSize(spec.numChannels, samplesPerBlock);
    dryBuffer.setSize(spec.numChannels, samplesPerBlock);
}


void AudioPluginAudioProcessor::releaseResources()
{

    for(auto &player : samplePlayers) {
        player->releaseSources();
    }
    arrangerLogic.releaseSources();
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    midiHandler.logMidiMessages(midiMessages, *presetManager);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = buffer.getNumChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    arrangerLogic.setBPM(bpmParameter->load());
    arrangerLogic.setStretch(stretchParameter->load());
    arrangerLogic.update();
    arrangerLogic.getMixer().getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));

    highPass.setCutoffFrequency(highpassFreqParameter->load()); 

    wetMix = (dryWetParameter->load()/100);
    dryMix = 1 - (dryWetParameter->load()/100);

    // Initialize reverb parameters
    reverbParams.roomSize = (roomSizeParameter->load() / 100);
    reverbParams.damping = (dampingParameter->load() / 100);
    reverbParams.width = (widthParameter->load() / 100);

    reverbParams.freezeMode = 0.0f;

    reverb.setParameters(reverbParams);

    auto numSamples = buffer.getNumSamples();
    // Store dry signal
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      dryBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    // Create wet signal by copying input and processing with reverb
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      wetBuffer.copyFrom(channel, 0, buffer, channel, 0, numSamples);
    }

    // Process wet signal through reverb
    // Note: We temporarily set dry level to 0 to get only wet signal

    juce::dsp::AudioBlock<float> wetBlock(wetBuffer);
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);
    reverb.process(wetContext);

    // Apply filtering to wet signal only
    highPass.process(wetContext);

    // Mix dry and processed wet signals
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
      auto *channelData = buffer.getWritePointer(channel);
      auto *dryData = dryBuffer.getReadPointer(channel);
      auto *wetData = wetBuffer.getReadPointer(channel);

      for (int sample = 0; sample < numSamples; ++sample) {
        channelData[sample] =
            (dryData[sample] * dryMix) + (wetData[sample] * wetMix);
      }
    }

    // Limiter
    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    limiter.process(context);

    //
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    //
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes){
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    
    if(xmlState.get() != nullptr)
        if(xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() {
  return new AudioPluginAudioProcessor();
}
