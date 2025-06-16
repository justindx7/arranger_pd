#pragma once

#include <JuceHeader.h>
#include "../handlers/AudioFileHandler.h"
#include "../gui/SampleButton.h"

class ArrangerLogic {
public:
    ArrangerLogic();
    // Needs references to all button vectors
    void initSections(SampleButton &intro, std::array<SampleButton,4> &verses, std::array<SampleButton,4> &fillIns,SampleButton &outro);

   // return function to getNextAudioBlock of all the players form the sections 

    void prepareToPlay(double sampleRate, int bufferSize) {
        mixer.removeAllInputs(); 
        mixer.prepareToPlay(bufferSize, sampleRate); 

        for (auto& [section, info] : sections) {
            if (info.player) {
                info.player->prepareToPlay(sampleRate, bufferSize);
                mixer.addInputSource(info.player->getSource(), false);
            }
        }
    }

    void releaseSources() {
        for (auto& [section, info] : sections) {
            if (info.player)
                info.player->releaseSources();
        }
    }

    juce::MixerAudioSource &getMixer() {
        return mixer;
    }

private:
  enum class ArrangerSection {
    Intro,
    Verse1,
    Verse2,
    Verse3,
    Verse4,
    FillIn1,
    FillIn2,
    FillIn3,
    FillIn4,
    Outro,
    None
  };

// Store pointers to the SampleButtons for local reference
    SampleButton* introPtr = nullptr;
    std::array<SampleButton, 4>* versesPtr = nullptr;
    std::array<SampleButton, 4>* fillInsPtr = nullptr;
    SampleButton* outroPtr = nullptr;

  struct SectionInfo {
    std::unique_ptr<AudioFileHandler> player; // Use unique_ptr
    int barAmount = 0;
    bool isLoop = false;
    ArrangerSection nextSection = ArrangerSection::None;
    std::function<void()> onButtonPressed;
    bool isPlaying = false;
  };

  std::map<ArrangerSection,SectionInfo> sections;
  ArrangerSection currentSection = ArrangerSection::None;
  ArrangerSection nextSection = ArrangerSection::None;

  void introPressed() {
    sections[ArrangerSection::Intro].isPlaying = true;
    sections[ArrangerSection::Intro].player->playSample();
    introPtr->setPlayingState(true);

    sections[ArrangerSection::Intro].player->onSampleStopped = [this]() {
        introPtr->setPlayingState(false);
    };
  }

  void outroPressed() {
      sections[ArrangerSection::Outro].isPlaying = true;
      sections[ArrangerSection::Outro].player->playSample();
  }
  void versePressed();
  void fillInPressed();

  juce::MixerAudioSource mixer;


void registerInfoCallbacks(SampleButton &intro,std::array<SampleButton, 4> &verses, std::array<SampleButton, 4> &fillIns,SampleButton &outro);
};
