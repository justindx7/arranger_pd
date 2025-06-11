#pragma once

#include <JuceHeader.h>

class AudioFileHandler : public juce::ChangeListener{
public:
    AudioFileHandler();

    void prepareToPlay(double sampleRate, int bufferSize);
    void releaseSources();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &AudioSource);
    void loadSample();
    void playSample();
    double getSampleLengthInSec();
    void setSample(const juce::String &fileLocation);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    void onSampleFinished();

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    juce::String fileName;
    bool loaded = false;
    bool transportJustStopped = false;
    bool needsLoading = false;
};
