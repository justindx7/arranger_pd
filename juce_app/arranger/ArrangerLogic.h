#pragma once
#include <JuceHeader.h>
#include "../handlers/AudioFileHandler.h"
#include "../gui/SampleButton.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"

class ArrangerLogic {
public:
    ArrangerLogic();
    // Needs references to all button vectors
    void initSections(SampleButton &intro, std::array<SampleButton,4> &verses, std::array<SampleButton,4> &fillIns,SampleButton &outro);

    void prepareToPlay(double sampleRate, int bufferSize);

    void releaseSources();
    void handleColours();

    void update();

    juce::MixerAudioSource &getMixer() {
        return mixer;
    }

    void setBPM(double newBPM);
    void setStretch(double newBPMOffset);

    void stop();

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

    double BPM = 0;
    double bpmOffset = 0;

    bool sectionChangePending = false;

    void handleSectionEnd(ArrangerLogic::ArrangerSection next);
    void handleSectionStart();

  struct SectionInfo {
    std::unique_ptr<AudioFileHandler> player; // Use unique_ptr
    SampleButton* sampleButton = nullptr;
    ArrangerLogic* arrangerLogic = nullptr;

    std::unordered_set<double> barLocations;

    int barAmount = 0;
    bool isLoop = false;

    ArrangerSection nextSection = ArrangerSection::None;

    bool isPlaying = false;
    ArrangerSection thisSection;

    void setSpeed (double speed) {
       player->setPlaybackSpeed(speed);
    }

    void calculate() {
      barLocations.clear(); // Clear previous bar locations
      if (arrangerLogic) {
        double beatLengthMS = 60000 / arrangerLogic->BPM;
        double barLengthMS = 4 * beatLengthMS;
        double bars = (player->getSampleLengthInSec() * 1000) / barLengthMS;
        barAmount = bars;

        DBG("Amount of Bars for: " << arrangerLogic->BPM << "BPM: " << bars);

        for (int i = 1; i < bars; i++) {
          barLocations.insert(barLengthMS * i);
        }
      }
    }

    void play() {
    if (sampleButton->getSelectedFilePath() != "") {
    arrangerLogic->currentSection = thisSection;
    arrangerLogic->nextSection = nextSection;
    isPlaying = true;
    player->playSample();

    juce::MessageManager::callAsync([buttonPtr = sampleButton] {
            if (buttonPtr)
                buttonPtr->setPlayingState(true);
                buttonPtr->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
            });

        } else {
            arrangerLogic->currentSection = ArrangerSection::None;
        }
    }

    void stop() {
      isPlaying = false;
      player->stopSample();

      if(isLoop)
        player->loadSample();

      juce::MessageManager::callAsync([buttonPtr = sampleButton] {
        if (buttonPtr)
          buttonPtr->setPlayingState(false);
      });
    }

    void prepare() {
        if (sampleButton && player) {
            player->setSample(sampleButton->getSelectedFilePath());
            player->loadSample();
            player->shouldloadOnStop(!isLoop);
    
            sampleButton->onNormalClick = [&]() {
                arrangerLogic->handleColours();

              if (arrangerLogic->currentSection == ArrangerSection::None) {
                  play();
              } else {
                arrangerLogic->nextSection = thisSection;
                arrangerLogic->sectionChangePending = true;
                sampleButton->setColour(juce::TextButton::buttonColourId, juce::Colours::green);
              }
            };
            player->onSampleStopped = [&]() { 
                isPlaying = false; 
                sampleButton->setPlayingState(false);
              if (arrangerLogic->sectionChangePending) {
                arrangerLogic->handleSectionEnd(arrangerLogic->nextSection);
                arrangerLogic->sectionChangePending = false;
                if(isLoop){player->loadSample();}

              } else if (isLoop) {
                arrangerLogic->handleSectionEnd(thisSection);
              } else {
                arrangerLogic->handleSectionEnd(nextSection);
              }


            };

            sampleButton->setFileSelectedCallback([&](juce::String filePath) {
              player->setSample(filePath);
              player->loadSample();
              calculate();
            });
        }
        calculate();
    }

  };

  std::map<ArrangerSection,SectionInfo> sections;
  ArrangerSection currentSection = ArrangerSection::None;
  ArrangerSection nextSection = ArrangerSection::None;
  juce::MixerAudioSource mixer;
};
