#include "SampleButton.h"
#include "juce_core/system/juce_PlatformDefs.h"

// TODO add playing state to this button

SampleButton::SampleButton(const juce::String& buttonText)
    : juce::TextButton(buttonText)
{
    //setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    //setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    //setColour(juce::TextButton::textColourOffId, juce::Colours::white);
}

SampleButton::~SampleButton() = default;

void SampleButton::mouseEnter(const juce::MouseEvent& event)
{
    //setColour(juce::TextButton::buttonColourId, juce::Colours::deepskyblue);
    repaint();
    juce::TextButton::mouseEnter(event);
}

void SampleButton::mouseExit(const juce::MouseEvent& event)
{
    //setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    repaint();
    juce::TextButton::mouseExit(event);
}


void SampleButton::setEditMode(bool shouldBeInEditMode) {
    editMode = shouldBeInEditMode;
}

bool SampleButton::isInEditMode() const {
    return editMode;
}

void SampleButton::enterEditMode() {

fileChooser = std::make_unique<FileChooser> ("Please select the audio file you want to load...",
                                               File::getSpecialLocation (File::userHomeDirectory),
                                               audioFileWildcard);
 
    auto fileChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(fileChooserFlags, [this](
                                                   const FileChooser &chooser) {
      File selectedFile(chooser.getResult());

      if (selectedFile.existsAsFile()) {
        selectedFilePath = selectedFile.getFullPathName();

        setButtonText(selectedFile.getFileName());
        repaint();
        DBG( "Selected file path: " << selectedFilePath.toStdString());

        // Call the callback if it's set
        if (onFileSelected)
          onFileSelected(selectedFilePath);
      } else {
        DBG("No file selected");
      }
    });
}

void SampleButton::clicked()
{
    if (editMode)
    {
        enterEditMode();
        DBG("Opening Edit Mode");
        if (onEditModeClick)
            onEditModeClick();
            
    }
    else
    {
        if (onNormalClick)
            onNormalClick();
    }
}

void SampleButton::setFileSelectedCallback(std::function<void(juce::String)> callback) {
  onFileSelected = callback;
}

juce::String SampleButton::getSelectedFilePath() const {
  return selectedFilePath;
}

void SampleButton::setPlayingState(bool playing) {
  if (isPlaying != playing) {
    isPlaying = playing;
    repaint();
  }

  if(isPlaying) {
    DBG("SampleButton PLAY");
  } else {
    DBG("SampleButton STOPPED");
  }
}

bool SampleButton::getPlayingState() const {
    return isPlaying;
}

