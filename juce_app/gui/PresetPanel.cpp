#include "PresetPanel.h"
#include "juce_gui_extra/juce_gui_extra.h"
#include "MidiAssignComponent.h" // Make sure this header exists and is in your include path

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
}

void PresetPanel::refreshPresetList()
{
    presets = PresetManager::getAllPresets();
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
        if (manager.getMidiProgramAssignments().contains(presets[i]))
        {
            int midiNum = manager.getMidiProgramAssignments()[presets[i]];
            displayText += "   [MIDI: " + juce::String(midiNum) + "]";
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
                [this, presetName = presets[i]](int midiNum) {
                    // Only assign if this midiNum is not already assigned to another preset
                    bool alreadyAssigned = false;
                    for (auto it = manager.getMidiProgramAssignments().begin(); it != manager.getMidiProgramAssignments().end(); ++it)
                    {
                        const juce::String& key = it.getKey();
                        int value = it.getValue();
                        if (value == midiNum && key != presetName)
                        {
                            alreadyAssigned = true;
                            break;
                        }
                    }
                    if (!alreadyAssigned) {
                            manager.assignMidiProgram(presetName, midiNum);

                    } else {
                        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                            "MIDI Program Assignment",
                            "This MIDI program number is already assigned to another preset.");
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

    auto callback = juce::ModalCallbackFunction::create([saveAlertWindow,this] (int result) {
        if(result == 1){
            auto userInput = saveAlertWindow->getTextEditorContents("text");
            auto fileName = juce::File::createLegalFileName(userInput);
            manager.savePreset(fileName);
            refreshPresetList();
        }
    });
    saveAlertWindow->enterModalState(true, callback, false);
}
