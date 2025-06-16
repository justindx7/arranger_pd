#include "ArrangerLogic.h"

ArrangerLogic::ArrangerLogic() {

  for (int i = 0; i < static_cast<int>(ArrangerSection::None); ++i) {
    ArrangerSection section = static_cast<ArrangerSection>(i);
    SectionInfo info;
    info.arrangerLogic = this;
    info.player = std::make_unique<AudioFileHandler>();
    info.thisSection = section;

    if (section >= ArrangerSection::Verse1 && section <= ArrangerSection::Verse4)
      info.isLoop = true;
        sections[section] = std::move(info);
  }
}

void ArrangerLogic::handleSectionEnd(ArrangerLogic::ArrangerSection next) {
        currentSection = next;
        if(currentSection != ArrangerSection::None) {
            sections[currentSection].play();
        }
}


void ArrangerLogic::handleSectionStart() {

    sections[currentSection].stop();
    currentSection = nextSection;

    sections[currentSection].play();
    sectionChangePending = false;

}

void ArrangerLogic::update() {

    if(sectionChangePending) {
        auto &cur = sections[currentSection];
        double pos = 0;
        if(cur.player) {
            pos = cur.player->getCurrentPosition() * 1000;
        }

        // Use a tolerance to check if pos is close to any bar location
        constexpr double toleranceMs = 4.0; // 10 milliseconds tolerance
        bool found = false;
        for (const auto& barLoc : cur.barLocations) {
            if (std::abs(barLoc - pos) <= toleranceMs) {
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
    sections[ArrangerSection::Intro].sampleButton = &intro;
    sections[ArrangerSection::Intro].nextSection = ArrangerSection::Verse1;

    // Assign verse buttons
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::Verse1) + i);
        sections[section].sampleButton = &((*versesPtr)[i]);
    }

    // Assign fill-in buttons
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::FillIn1) + i);
        sections[section].sampleButton = &((*fillInsPtr)[i]);
        // Set nextSection for fill-ins to corresponding verses
        sections[section].nextSection = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::Verse1) + i);
    }

    // Assign outro button
    sections[ArrangerSection::Outro].sampleButton = &outro;

    for (int i = static_cast<int>(ArrangerSection::Intro); i < static_cast<int>(ArrangerSection::None); ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(i); 
        sections[section].prepare();
    }
}
