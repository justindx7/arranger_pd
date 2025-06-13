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

    std::function<void()> onSampleStopped;

private:
    void onSampleFinished();

    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::ResamplingAudioSource> resamplingSource;
    std::unique_ptr<juce::AudioFormatReaderSource> fileSource;

    juce::String fileName;
    juce::String currentlyLoadedFile;

    bool loaded = false;
    bool transportJustStopped = false;
    // TODO fix needsLoading to work after sample is done playing check if setSample is the same as file currently loaded?
    bool needsLoading = false;
    // TODO make an arranger helper that has all the buttons that it needs as one object that than can be loaded as a full component
};
