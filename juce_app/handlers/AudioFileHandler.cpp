#include "AudioFileHandler.h"

AudioFileHandler::AudioFileHandler() {
    formatManager.registerBasicFormats();
}

void AudioFileHandler::prepareToPlay(double sampleRate, int bufferSize) {
    transportSource.prepareToPlay(bufferSize, sampleRate);
    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
    resamplingSource->prepareToPlay(bufferSize, sampleRate);
    loadSample();
}

void AudioFileHandler::releaseSources() {
    resamplingSource->releaseResources();
    transportSource.releaseResources();
}

void AudioFileHandler::getNextAudioBlock(const juce::AudioSourceChannelInfo &AudioSource) {
    resamplingSource->getNextAudioBlock(AudioSource);
}

void AudioFileHandler::loadSample() {
    if (!transportSource.isPlaying()) {
        loaded = false;
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        auto testFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            .getChildFile("songdata/test.wav");

        if (testFile.existsAsFile()) {
            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(testFile));
            if (reader) {
                readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
                transportSource.setSource(readerSource.get(), 0, nullptr,
                                          readerSource->getAudioFormatReader()->sampleRate);

                resamplingSource->setResamplingRatio(0.5);
                loaded = true;
            }
        }
    }
}

void AudioFileHandler::playSample() {
    if (loaded && !transportSource.isPlaying()) {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}
