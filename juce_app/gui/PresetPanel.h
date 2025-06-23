#pragma once

#include <JuceHeader.h>
#include "../service/PresetManager.h"
#include "MidiAssignComponent.h"
#include "CategoryAssignComponent.h"

class PresetPanel : public juce::Component
{
public:
    PresetPanel(PresetManager& presetManager, std::function<void()> onClose);

    void refreshPresetList();
    void paint(juce::Graphics&) override;
    void resized() override;
    void mouseUp(const juce::MouseEvent&) override;

private:
    PresetManager& manager;
    std::function<void()> onCloseCallback;

    juce::TextButton closeButton, saveButton, prevPageButton, nextPageButton;

    juce::StringArray presets;
    juce::String selectedPreset;
    int currentPage = 0;
    int totalPages = 1;
    static constexpr int presetsPerPage = 7;

    juce::HashMap<int, juce::Rectangle<int>> presetItemAreas, deleteButtonAreas;
    juce::HashMap<int, juce::Rectangle<int>> midiButtonAreas;

    // Delete popup
    bool showDeletePopup = false;
    juce::String presetToDelete;

    void changePage(int delta);
    void openSaveDialog();
    void openCategoryDialog(const juce::String& fileName);

   juce::ComboBox categoryFilterBox;
   juce::String filterCategory = "All";

   const juce::StringArray presetCategories{
       "Ballad", "Disco", "Hiphop", "House", "Orchestral",
       "Pop",    "Latin", "Reggae", "Rock",  "Waltz"};
};
