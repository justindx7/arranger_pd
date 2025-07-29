#pragma once

#include <JuceHeader.h>
#include "../service/PresetManager.h"
#include "MidiAssignComponent.h"
#include "CategoryAssignComponent.h"
#include "juce_gui_basics/juce_gui_basics.h"

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

   juce::TextButton categoryFilterButton;
   juce::String filterCategory = "All";

   const juce::StringArray presetCategories{
       "Ballad", "Disco", "Hiphop", "House", "Orchestral",
       "Pop",    "Latin", "Reggae", "Rock",  "Waltz"};

// currently not used but still here to maybe implement
   void showCategoryMenu() {
     juce::PopupMenu menu;
     // Add "All" option
     menu.addItem(1, "All", true, filterCategory == "All");
     menu.addSeparator();
     // Add categories
     int itemId = 2;
     for (const auto &cat : presetCategories) {
       menu.addItem(itemId, cat, true, filterCategory == cat);
       itemId++;
     }

     menu.showMenuAsync(
         juce::PopupMenu::Options()
             .withTargetComponent(&categoryFilterButton)
             .withStandardItemHeight(40), // Larger items for touch
         [this](int result) {
           if (result == 0)
             return; // User cancelled

           if (result == 1) {
             filterCategory = "All";
             categoryFilterButton.setButtonText("All");
           } else {
             int categoryIndex = result - 2;
             if (categoryIndex >= 0 &&
                 categoryIndex < presetCategories.size()) {
               filterCategory = presetCategories[categoryIndex];
               categoryFilterButton.setButtonText(filterCategory);
             }
           }

           refreshPresetList();
         });
   }

   void cycleCategoryFilter() {
     juce::StringArray allOptions;
     allOptions.add("All");
     for (const auto &cat : presetCategories)
       allOptions.add(cat);

     int currentIndex = allOptions.indexOf(filterCategory);
     int nextIndex = (currentIndex + 1) % allOptions.size();

     filterCategory = allOptions[nextIndex];
     categoryFilterButton.setButtonText(filterCategory);
     refreshPresetList();
   }
};
