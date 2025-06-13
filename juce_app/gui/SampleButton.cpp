#include "SampleButton.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"

SampleButton::SampleButton(const juce::String& buttonText)
    : juce::TextButton(buttonText)
{
         
    originalButtonText = buttonText;
    setButtonText(originalButtonText);
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
    auto* popup = new SampleEditPopup(
        [this]() {
            fileChooser = std::make_unique<FileChooser>(
                "Please select the audio file you want to load...",
                File::getSpecialLocation(File::userHomeDirectory),
                audioFileWildcard);

            auto fileChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

            fileChooser->launchAsync(fileChooserFlags, [this](const FileChooser& chooser) {
                File selectedFile(chooser.getResult());

                if (selectedFile.existsAsFile()) {
                    selectedFilePath = selectedFile.getFullPathName();
                    setButtonText(selectedFile.getFileName());
                    repaint();
                    DBG("Selected file path: " << selectedFilePath.toStdString());

                    if (onFileSelected)
                        onFileSelected(selectedFilePath);
                } else {
                    DBG("No file selected");
                }
            });
        },
        [this]() {
            selectedFilePath = "";
            setButtonText(originalButtonText);
            repaint();
            if (onFileSelected)
                onFileSelected(selectedFilePath);
        }
    );

    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(popup);
    options.content->setSize(300, 180);
    options.dialogTitle = "Sample Options";
    options.dialogBackgroundColour = juce::Colours::darkgrey;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = false;
    options.resizable = false;

    options.launchAsync();

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
  if (isPlaying != playing && selectedFilePath != "") {
    isPlaying = playing;

    if (isPlaying) {
      startFlashing();
    } else {
      stopFlashing();
    }
    repaint();
  }
}

bool SampleButton::getPlayingState() const {
    return isPlaying;
}


void SampleButton::setFile(const juce::String &newFile) { 
    selectedFilePath = newFile;
    juce::File selectedFile(newFile);
    setButtonText(selectedFile.getFileName());
    repaint();
}

void SampleButton::timerCallback() {
    flashOn = !flashOn;
    repaint();
}

void SampleButton::startFlashing() {
    flashOn = false;
    startTimer(300); // Flash every 300ms
}

void SampleButton::stopFlashing() {
    stopTimer();
    flashOn = false;
    repaint();
}

void SampleButton::paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    // Draw the normal button
    juce::TextButton::paintButton(g, isMouseOverButton, isButtonDown);

    // Overlay a white flash if playing and flashOn
    if (isPlaying && flashOn && selectedFilePath != "")
    {
        g.setColour(juce::Colours::white.withAlpha(0.7f)); // 0.7 = 70% opacity, adjust as needed
        g.fillRect(getLocalBounds());
    }

    if(selectedFilePath == "") {
        setButtonText(originalButtonText);
        g.setColour(juce::Colours::darkgrey.withAlpha(0.7f)); // 0.7 = 70% opacity, adjust as needed
        g.fillRect(getLocalBounds());
    }
}
