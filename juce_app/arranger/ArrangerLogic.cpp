#include "ArrangerLogic.h"

ArrangerLogic::ArrangerLogic() {
  for (int i = 0; i < static_cast<int>(ArrangerSection::None); ++i) {
    ArrangerSection section = static_cast<ArrangerSection>(i);
    SectionInfo info;
    info.player = std::make_unique<AudioFileHandler>();
    if (section >= ArrangerSection::Verse1 && section <= ArrangerSection::Verse4)
      info.isLoop = true;
        sections[section] = std::move(info);
  }
}

void ArrangerLogic::registerInfoCallbacks(SampleButton &intro,std::array<SampleButton, 4> &verses, std::array<SampleButton, 4> &fillIns,SampleButton &outro) {
    // Intro
    sections[ArrangerSection::Intro].onButtonPressed = [this]() {
        DBG("Intro pressed!");
        introPressed();
    };
    intro.onNormalClick = sections[ArrangerSection::Intro].onButtonPressed;

    intro.setFileSelectedCallback([this](juce::String filePath) {
        auto& player = sections[ArrangerSection::Intro].player;
        if (player) {
            player->setSample(filePath);
            player->loadSample();
        }
    });

    // Verses
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::Verse1) + i);
        sections[section].onButtonPressed = [this, section]() {
            DBG("Verse pressed: " << (int)section);
        };
        verses[i].onNormalClick = sections[section].onButtonPressed;
        verses[i].setFileSelectedCallback([this, section](juce::String filePath) {
            auto& player = sections[section].player;
            if (player) {
                player->setSample(filePath);
                player->loadSample();
            }
        });
    }

    // Fill-Ins
    for (int i = 0; i < 4; ++i) {
        ArrangerSection section = static_cast<ArrangerSection>(static_cast<int>(ArrangerSection::FillIn1) + i);
        sections[section].onButtonPressed = [this, section]() {
            DBG("Fill-In pressed: " << (int)section - 4);
        };
        fillIns[i].onNormalClick = sections[section].onButtonPressed;
        fillIns[i].setFileSelectedCallback([this, section](juce::String filePath) {
            auto& player = sections[section].player;
            if (player) {
                player->setSample(filePath);
                player->loadSample();
            }
        });
    }

    // Outro
    sections[ArrangerSection::Outro].onButtonPressed = [this]() {
        DBG("Outro pressed!");
        outroPressed();
    };
    outro.onNormalClick = sections[ArrangerSection::Outro].onButtonPressed;
    outro.setFileSelectedCallback([this](juce::String filePath) {
        auto& player = sections[ArrangerSection::Outro].player;
        if (player) {
            player->setSample(filePath);
            player->loadSample();
        }
    });
}

void ArrangerLogic::initSections(SampleButton &intro, std::array<SampleButton, 4> &verses, std::array<SampleButton, 4> &fillIns, SampleButton &outro) {
    // Store references to the buttons as members
    introPtr = &intro;
    versesPtr = &verses;
    fillInsPtr = &fillIns;
    outroPtr = &outro;

    // Initialize all sections and their players
    registerInfoCallbacks(intro, verses, fillIns, outro);
}
