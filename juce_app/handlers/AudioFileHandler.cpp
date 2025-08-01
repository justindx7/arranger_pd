#include "AudioFileHandler.h"

AudioFileHandler::AudioFileHandler() {
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
}

void AudioFileHandler::prepareToPlay(double sampleRate, int bufferSize) {
    transportSource.prepareToPlay(bufferSize, sampleRate);
    resamplingSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);
    resamplingSource->prepareToPlay(bufferSize, sampleRate);
    loadSample();

}

void AudioFileHandler::releaseSources() {
    if(resamplingSource)
        resamplingSource->releaseResources();
    transportSource.releaseResources();
}


double AudioFileHandler::getSampleLengthInSec() {
    return transportSource.getLengthInSeconds();
}

void AudioFileHandler::getNextAudioBlock(const juce::AudioSourceChannelInfo &AudioSource) {
    if(resamplingSource) {
        resamplingSource->getNextAudioBlock(AudioSource);
    }
    else {
        AudioSource.clearActiveBufferRegion();
    }
}

void AudioFileHandler::onSampleFinished() {
    if (onSampleStopped) {
      onSampleStopped();
    }

    if (needsLoading && shouldLoadOnStop) {
      loadSample();
    }
}

void AudioFileHandler::changeListenerCallback(juce::ChangeBroadcaster *source) {
  if (source == &transportSource) {
    if (!transportSource.isPlaying() && transportJustStopped) {
      onSampleFinished();
      transportJustStopped = false;
    } else if (transportSource.isPlaying()) {
      transportJustStopped = true;
    }
  }
}


void AudioFileHandler::setSample(const juce::String &fileLocation) {
    fileName = fileLocation;
    if(fileName != currentlyLoadedFile) {
        needsLoading = true;
    }
}

void AudioFileHandler::loadSample() {
if(fileName != currentlyLoadedFile || needsLoading) {

    if (!transportSource.isPlaying()) {
        loaded = false;
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        //auto testFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                            //.getChildFile("songdata/test.wav");

        auto testFile = juce::File(fileName);
        DBG("Currently Loaded: " << currentlyLoadedFile);
        DBG("Now Loading: " << fileName);

        if (testFile.existsAsFile()) {
            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(testFile));
            if (reader) {
                readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader.release(), true);
                transportSource.setSource(readerSource.get(), 0, nullptr,
                                          readerSource->getAudioFormatReader()->sampleRate);
                if(resamplingSource)
                    resamplingSource->setResamplingRatio(playBackSpeed);

                loaded = true;
                needsLoading = false;
                currentlyLoadedFile = fileName;
            }
        }
    }
}
}

void AudioFileHandler::playSample() {
    if (loaded && !transportSource.isPlaying() && currentlyLoadedFile !="") {
        DBG("PLAYING: " << currentlyLoadedFile << " " << transportSource.getTotalLength());
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}


void AudioFileHandler::stopSample() {
    transportSource.setSource(nullptr);
    //transportSource.stop();
    if (readerSource != nullptr && readerSource->getAudioFormatReader() != nullptr) {
    transportSource.setSource(readerSource.get(), 0, nullptr,readerSource->getAudioFormatReader()->sampleRate);
    }
}

void AudioFileHandler::shouldloadOnStop(bool loadOnStop){
    shouldLoadOnStop = loadOnStop;
}

void AudioFileHandler::setPlaybackSpeed(double speed) { 
    playBackSpeed = speed;

    if (resamplingSource)
      resamplingSource->setResamplingRatio(playBackSpeed);
}

