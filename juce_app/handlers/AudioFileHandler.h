#pragma once

#include <JuceHeader.h>

class AudioFileHandler {
public:
    AudioFileHandler();

    void prepareToPlay(double sampleRate, int bufferSize);
    void releaseSources();
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &AudioSource);
    void loadSample();
    void playSample();

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;
    juce::String fileName;
    bool loaded = false;
};
