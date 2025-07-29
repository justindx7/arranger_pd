#include "PresetPanel.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "MidiAssignComponent.h" 

PresetPanel::PresetPanel(PresetManager& presetManager, std::function<void()> onClose)
    : manager(presetManager), onCloseCallback(onClose)
{
    setOpaque(true);

    addAndMakeVisible(closeButton);
    closeButton.setButtonText("Close");
    closeButton.onClick = [this] { if (onCloseCallback) onCloseCallback(); };

    addAndMakeVisible(saveButton);
    saveButton.setButtonText("Save Preset");
    saveButton.onClick = [this] { openSaveDialog(); };

    addAndMakeVisible(prevPageButton);
    prevPageButton.setButtonText("<");
    prevPageButton.onClick = [this] { changePage(-1); };

    addAndMakeVisible(nextPageButton);
    nextPageButton.setButtonText(">");
    nextPageButton.onClick = [this] { changePage(1); };

    refreshPresetList();



    addAndMakeVisible(categoryFilterButton);
    categoryFilterButton.setButtonText("All");
    categoryFilterButton.onClick = [this] { showCategoryMenu(); };

}

void PresetPanel::refreshPresetList(){
    auto allPresets = manager.getAllPresets();

    presets.clear();
    for (const auto &preset : allPresets) {
      auto cat = manager.getPresetCategory(preset);
      if (filterCategory == "All" || cat == filterCategory)
        presets.add(preset);
    }

    totalPages = std::max(1, (int)std::ceil((float)presets.size() / (float)presetsPerPage));
    currentPage = std::min(currentPage, totalPages - 1);
    selectedPreset = manager.getCurrentPreset();
    repaint();
}

void PresetPanel::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

    // Title
    g.setColour(juce::Colours::white);
    g.setFont(30.0f);
    g.drawFittedText("Presets", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

    // Page info
    g.setFont(18.0f);
    juce::String pageInfo = juce::String(currentPage + 1) + " / " + juce::String(totalPages);
    g.drawText(pageInfo, getLocalBounds().removeFromTop(100), juce::Justification::centred);

    // Draw presets
    int y = 100;
    int itemHeight = 60;
    int startIdx = currentPage * presetsPerPage;
    int endIdx = std::min(startIdx + presetsPerPage, presets.size());

    // Calculate available area for preset list
    int topBarHeight = 70;
    int bottomBarHeight = 80;
    int availableHeight = getHeight() - topBarHeight - bottomBarHeight;
    itemHeight = availableHeight / presetsPerPage;
    y = topBarHeight;

    presetItemAreas.clear();
    deleteButtonAreas.clear();
    midiButtonAreas.clear();
    int localIdx = 0;
    for (int i = startIdx; i < endIdx; ++i, ++localIdx)
    {
        juce::Rectangle<int> itemArea(40, y, getWidth() - 80, itemHeight - 10);

        bool isSelected = (presets[i] == selectedPreset);
        g.setColour(isSelected ? juce::Colours::orange : juce::Colours::lightgrey);
        g.fillRoundedRectangle(itemArea.toFloat(), 12.0f);

        g.setColour(juce::Colours::black);
        g.setFont(22.0f);

        // Draw preset name and MIDI assignment if present
        juce::String displayText = presets[i];
        if (manager.getMidiProgramAssignments().contains(presets[i])) {

          const auto &assignments = manager.getMidiProgramAssignments();
          auto it = assignments.find(presets[i]);
          if (it != assignments.end()) {
            int midiNum = it->second.first;
            int midiChan = it->second.second;

            displayText += "   [MIDI: PG: " + juce::String(midiNum) + " [" + juce::String(midiNum + 1) +
                           "]  CH: " + juce::String(midiChan) + "]";
          }
        }

        if (filterCategory == "All")
        {
          juce::String presetCategory = manager.getPresetCategory(presets[i]);
          if (presetCategory != "") {
            displayText += "   | " + presetCategory;
          }
        }

        g.drawText(displayText, itemArea.reduced(20, 0), juce::Justification::centredLeft);

        // Draw delete button (centered vertically)
        int delButtonSize = 40;
        int delButtonX = itemArea.getRight() - delButtonSize - 20;
        int delButtonY = itemArea.getY() + (itemArea.getHeight() - delButtonSize) / 2;
        juce::Rectangle<int> delArea(delButtonX, delButtonY, delButtonSize, delButtonSize);
        g.setColour(juce::Colours::red);
        g.fillEllipse(delArea.toFloat());
        g.setColour(juce::Colours::white);
        g.setFont(24.0f);
        g.drawText("X", delArea, juce::Justification::centred);

        // Draw MIDI program button (blue, left of delete)
        int midiBtnSize = 40;
        int midiBtnX = delButtonX - midiBtnSize - 10;
        int midiBtnY = delButtonY;
        juce::Rectangle<int> midiArea(midiBtnX, midiBtnY, midiBtnSize, midiBtnSize);
        g.setColour(juce::Colours::blue);
        g.fillEllipse(midiArea.toFloat());
        g.setColour(juce::Colours::white);
        g.setFont(18.0f);
        g.drawText("M", midiArea, juce::Justification::centred);

        midiButtonAreas.set(localIdx, midiArea); // Store clickable area for mouseUp

        presetItemAreas.set(localIdx, itemArea);
        deleteButtonAreas.set(localIdx, delArea);

        y += itemHeight;
    }

    // Draw popup if needed
    if (showDeletePopup)
    {
        juce::Rectangle<int> popup(getWidth() / 2 - 150, getHeight() / 2 - 80, 300, 160);
        g.setColour(juce::Colours::darkred.withAlpha(0.95f));
        g.fillRoundedRectangle(popup.toFloat(), 16.0f);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Delete preset '" + presetToDelete + "'?", popup.reduced(10, 40), juce::Justification::centred, 2);
        g.setFont(18.0f);
        g.drawFittedText("Are you sure?", popup.reduced(10, 80), juce::Justification::centred, 1);

        // Yes/No buttons
        g.setColour(juce::Colours::green);
        g.fillRoundedRectangle(popup.getX() + 30, popup.getBottom() - 50, 80, 40, 10.0f);
        g.setColour(juce::Colours::white);
        g.drawText("Yes", popup.getX() + 30, popup.getBottom() - 50, 80, 40, juce::Justification::centred);

        g.setColour(juce::Colours::grey);
        g.fillRoundedRectangle(popup.getRight() - 110, popup.getBottom() - 50, 80, 40, 10.0f);
        g.setColour(juce::Colours::white);
        g.drawText("No", popup.getRight() - 110, popup.getBottom() - 50, 80, 40, juce::Justification::centred);
    }
}

void PresetPanel::resized()
{
    int btnH = 50;
    closeButton.setBounds(getWidth() - 120, 10, 100, btnH);
    saveButton.setBounds(20, 10, 160, btnH);
    prevPageButton.setBounds(20, getHeight() - 70, 60, 50);
    nextPageButton.setBounds(getWidth() - 80, getHeight() - 70, 60, 50);

    categoryFilterButton.setBounds(getWidth()  - 340, 10, 160, btnH                     );
}

void PresetPanel::mouseUp(const juce::MouseEvent& e)
{
    if (showDeletePopup)
    {
        juce::Rectangle<int> popup(getWidth() / 2 - 150, getHeight() / 2 - 80, 300, 160);
        juce::Rectangle<int> yesBtn(popup.getX() + 30, popup.getBottom() - 50, 80, 40);
        juce::Rectangle<int> noBtn(popup.getRight() - 110, popup.getBottom() - 50, 80, 40);

        if (yesBtn.contains(e.getPosition()))
        {
            manager.deletePreset(presetToDelete);
            manager.removeMidiProgram(presetToDelete);

            showDeletePopup = false;
            refreshPresetList();
            return;
        }
        if (noBtn.contains(e.getPosition()))
        {
            showDeletePopup = false;
            repaint();
            return;
        }
        return;
    }

    // Only one loop, using localIdx for all clickable areas
    int startIdx = currentPage * presetsPerPage;
    int endIdx = std::min(startIdx + presetsPerPage, presets.size());
    int localIdx = 0;
    for (int i = startIdx; i < endIdx; ++i, ++localIdx)
    {
        if (deleteButtonAreas[localIdx].contains(e.getPosition()))
        {
            presetToDelete = presets[i];
            showDeletePopup = true;
            repaint();
            return;
        }

        if (midiButtonAreas[localIdx].contains(e.getPosition()))
        {
            auto* midiComp = new MidiAssignComponent(
                [this, presetName = presets[i]](int midiNum, int midiChan) {

                    // Only assign if this midiNum is not already assigned to another preset
                    bool alreadyAssigned = false;
                    for (auto it = manager.getMidiProgramAssignments().begin(); it != manager.getMidiProgramAssignments().end(); ++it)
                    {
                        const juce::String& key = it->first;
                        int value = it->second.first;
                        int chan = it->second.second;
                        if (value == midiNum && chan == midiChan && key != presetName)
                        {
                            alreadyAssigned = true;
                            break;
                        }
                    }
                    if (!alreadyAssigned) {
                            manager.assignMidiProgram(presetName, midiNum,midiChan);

                    } else {
                        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                            "MIDI Program Assignment",
                            "This MIDI program channel combo is already assigned to another preset.");
                    }
                    repaint();
                },
                [this, presetName = presets[i]] {
                    manager.removeMidiProgram(presetName);
                    repaint();
                }
            );

            juce::DialogWindow::LaunchOptions opts;
            opts.content.setOwned(midiComp);
            opts.dialogTitle = "Assign MIDI Program";
            opts.dialogBackgroundColour = juce::Colours::darkgrey;
            opts.escapeKeyTriggersCloseButton = true;
            opts.useNativeTitleBar = true;
            opts.resizable = false;
            opts.launchAsync();

            return;
        }
        // Remove 
        if (presetItemAreas[localIdx].contains(e.getPosition())) {
            manager.loadPreset(presets[i]);
            selectedPreset = presets[i];
            repaint();
            return;
        }
    }
}

void PresetPanel::changePage(int delta)
{
    int newPage = juce::jlimit(0, totalPages - 1, currentPage + delta);
    if (newPage != currentPage)
    {
        currentPage = newPage;
        repaint();
    }
}

void PresetPanel::openSaveDialog()
{
    auto saveAlertWindow = std::make_shared<juce::AlertWindow>("Saving Preset", "", juce::AlertWindow::NoIcon);
    saveAlertWindow->addTextEditor("text", "", "Enter name:", false);
    saveAlertWindow->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    saveAlertWindow->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    auto assignNextMidi = [this](const juce::String& fileName)
    {
        std::set<std::pair<int, int>> used;

        // get program channel pairs that are saved and insert those in the set so their marked as used
        for (const auto& kv : manager.getMidiProgramAssignments())
            used.insert(kv.second);

        bool assigned = false;
        for (int ch = 1; ch < 16 && !assigned; ++ch)
        {
            for (int pg = 0; pg < 128 && !assigned; ++pg)
            {
                if (used.find({pg, ch}) == used.end())
                {
                    manager.assignMidiProgram(fileName, pg, ch);
                    assigned = true;
                }
            }
        }
    };

    auto callback = juce::ModalCallbackFunction::create([saveAlertWindow, this, assignNextMidi](int result) {
        if (result == 1) {
            auto userInput = saveAlertWindow->getTextEditorContents("text");
            auto fileName = juce::File::createLegalFileName(userInput);

            // Check if preset already exists
            auto allPresets = manager.getAllPresets();
            bool exists = std::find(allPresets.begin(), allPresets.end(), fileName) != allPresets.end();

            auto doSave = [this, fileName]() {
                manager.savePreset(fileName);
                openCategoryDialog(fileName);
                refreshPresetList();
            };

            if (exists)
            {
                juce::AlertWindow::showOkCancelBox(
                    juce::AlertWindow::WarningIcon,
                    "Preset Exists",
                    "A preset with this name already exists. Do you want to overwrite it?",
                    "Overwrite",
                    "Cancel",
                    nullptr,
                    juce::ModalCallbackFunction::create([doSave](int overwriteResult) {
                        if (overwriteResult == 1) {
                            doSave(); // Only save and refresh, do NOT assign MIDI
                        }
                    })
                );
            }
            else
            {
                doSave();
                assignNextMidi(fileName); // Only assign MIDI for new presets
            }
        }
    });
    saveAlertWindow->enterModalState(true, callback, false);
}

void PresetPanel::openCategoryDialog(const juce::String& fileName) {

  auto *comp = new CategoryAssignComponent(
      presetCategories, [this, fileName](const juce::String &category) {
        manager.setPresetCategory(fileName, category);
        refreshPresetList();
      });

  juce::DialogWindow::LaunchOptions opts;
  opts.content.setOwned(comp);
  opts.dialogTitle = "Select Category";
  opts.dialogBackgroundColour = juce::Colours::darkgrey;
  opts.escapeKeyTriggersCloseButton = true;
  opts.useNativeTitleBar = false;
  opts.resizable = false;
  opts.launchAsync();
}
