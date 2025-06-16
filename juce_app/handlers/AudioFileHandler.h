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
    void stopSample();
    void setLooping(bool isLooping);
    double getSampleLengthInSec();
    void setSample(const juce::String &fileLocation);
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    double getCurrentPosition() const { return transportSource.getCurrentPosition();}

    void setGain(double gain) { transportSource.setGain(gain);}

    std::function<void()> onSampleStopped;

     juce::ResamplingAudioSource* getSource() {return resamplingSource.get();}
     void setPlaybackSpeed(double speed) { playBackSpeed = speed; }

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
    bool needsLoading = false;
    bool shouldLoop = false;

    double playBackSpeed = 1.0;
    // TODO make an arranger helper that has all the buttons that it needs as one object that than can be loaded as a full component
};
