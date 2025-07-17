#include "ArrangerLogic.h"
#include <memory>

ArrangerLogic::ArrangerLogic() {
  for (int i = 0; i < static_cast<int>(ArrangerSection::None); ++i) {
    ArrangerSection section = static_cast<ArrangerSection>(i);
    auto info = std::make_unique<SectionInfo>();
    info->arrangerLogic = this;
    info->player = std::make_unique<AudioFileHandler>();
    info->thisSection = section;

    if (section >= ArrangerSection::Verse1 && section <= ArrangerSection::Verse4)
      info->isLoop = true;
        sections[section] = std::move(info);
  }
}

void ArrangerLogic::handleSectionEnd(ArrangerLogic::ArrangerSection next) {
        currentSection = next;
        if(currentSection != ArrangerSection::None) {
            sections[currentSection]->play();
        }
}


void ArrangerLogic::handleSectionStart() {

    sections[currentSection]->stop();
    currentSection = nextSection;

    sections[currentSection]->play();
    sectionChangePending = false;

}

void ArrangerLogic::update() {

// crossed a bar checking

    if(sectionChangePending) {
        auto &cur = sections[currentSection];
        double pos = 0;
        if(cur->player) {
            // get current play position and convert to MS
            pos = cur->player->getCurrentPosition() * 1000;
        }

        // Use a tolerance to check if pos is close to any bar location
        bool found = false;

        double prevPos = cur->lastCheckedPosition;
        double currPos = pos;
        cur->lastCheckedPosition = currPos;

        for (const auto& barLoc : cur->barLocations) {
            if (prevPos < barLoc && currPos >= barLoc) {
                found = true;
                break;
            }
        }

        if(found) {
            handleSectionStart();
        }
    }
}


void ArrangerLogic::initSections(SampleButton &intro, std::array<SampleButton, 4> &verses, std::array<SampleButton, 4> &fillIns, SampleButton &outro) {
    introPtr = &intro;
    versesPtr = &verses;
    fillInsPtr = &fillIns;
    outroPtr = &outro;

    // Assign intro button
    sections[ArrangerSection::Intro]->sampleButton = &intro;
    sections[ArrangerSection::Intro]->nextSection = ArrangerSection::Verse1;

    // Assign verse buttons
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::Verse1) + i);
        sections[section]->sampleButton = &((*versesPtr)[i]);
    }

    // Assign fill-in buttons
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::FillIn1) + i);
        sections[section]->sampleButton = &((*fillInsPtr)[i]);
        // Set nextSection for fill-ins to corresponding verses
        sections[section]->nextSection = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::Verse1) + i);
    }

    // Assign outro button
    sections[ArrangerSection::Outro]->sampleButton = &outro;

    for (int i = static_cast<int>(ArrangerSection::Intro); i < static_cast<int>(ArrangerSection::None); ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(i); 
        sections[section]->prepare();
    }
}
void ArrangerLogic::prepareToPlay(double sampleRate, int bufferSize) {
  mixer.removeAllInputs();
  mixer.prepareToPlay(bufferSize, sampleRate);


  for (auto &[section, info] : sections) {
    if (info->player) {
      info->player->prepareToPlay(sampleRate, bufferSize);
      mixer.addInputSource(info->player->getSource(), false);
    }
  }
}

void ArrangerLogic::releaseSources() { for (auto &[section, info] : sections) {
    if (info->player)
      info->player->releaseSources();
  }
}

void ArrangerLogic::setBPM(double newBPM) {
  constexpr double epsilon = 1e-6;
  if (std::abs(BPM - newBPM) > epsilon) {
    BPM = newBPM;
    for (auto &[section, info] : sections) {
      info->calculate();
    }
  }
}

void ArrangerLogic::setGain(float newGain) {
  for (auto &[section, info] : sections) {
    if (info->player) {
        info->player->setGain(newGain);
        }
    }
}

void ArrangerLogic::setStretch(double newBPMOffset) {
  constexpr double epsilon = 1e-6;
  if (std::abs(bpmOffset - newBPMOffset) > epsilon) {
    bpmOffset = newBPMOffset;

    double speed = (BPM + bpmOffset) / BPM;

    for (auto &[section, info] : sections) {
      info->setSpeed(speed);
    }
  }
}

void ArrangerLogic::stop() {
  if (currentSection != ArrangerSection::None) {
    sections[currentSection]->stop();
    currentSection = ArrangerSection::None;
  }

  for (auto &[section, info] : sections) {
    if (info->sampleButton) {
      juce::MessageManager::callAsync([safeButton = juce::Component::SafePointer<SampleButton>(info->sampleButton)] {
        if (safeButton) 
             safeButton->setColour(juce::TextButton::buttonColourId,juce::Colours::darkblue);
      });
    }
  }
}

void ArrangerLogic::handleColours() {
  for (auto &[section, info] : sections) {
    if (info->sampleButton) {
        info->sampleButton->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
    }
  }
}
