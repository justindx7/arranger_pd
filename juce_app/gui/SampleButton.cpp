#include "SampleButton.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "juce_graphics/juce_graphics.h"

SampleButton::SampleButton(const juce::String& buttonText, juce::AudioProcessorValueTreeState& Reference, bool isArrangeButton)
    : juce::TextButton(buttonText), APVTSRef(Reference), isArranger(isArrangeButton)
{
         

    originalButtonText = buttonText;
    setButtonText(originalButtonText);
    APVTSName = makeValidXmlName(originalButtonText) + "Path";
    DBG(APVTSName);

    APVTSRef.state.addListener(this);
    APVTSRef.state.getOrCreateChildWithName(APVTSName, nullptr);
    restoreState();

    if(isArranger) 
        setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);

    //updater.addAnimator(flashAnimator);
    
    //setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    //setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    //setColour(juce::TextButton::textColourOffId, juce::Colours::white);
}

SampleButton::~SampleButton() {
    
    APVTSRef.state.removeListener(this);
}

void SampleButton::mouseEnter(const juce::MouseEvent& event)
{
    //setColour(juce::TextButton::buttonColourId, juce::Colours::deepskyblue);
    juce::TextButton::mouseEnter(event);
}

void SampleButton::mouseExit(const juce::MouseEvent& event)
{
    //setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    juce::TextButton::mouseExit(event);
}


void SampleButton::setEditMode(bool shouldBeInEditMode) {
    editMode = shouldBeInEditMode;
    repaint();
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
                    setFile(selectedFile.getFullPathName());
                    DBG("Selected file path: " << selectedFilePath.toStdString());

                } else {
                    DBG("No file selected");
                }
            });
        },
        [this]() {
            setFile("");
            setPlayingState(false);
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
    repaint();

    if (isPlaying) {
        startFlashing();
    } else {
        stopFlashing();
    }
  }
}

bool SampleButton::getPlayingState() const {
    return isPlaying;
}

void SampleButton::saveState() {
    auto buttonNode = APVTSRef.state.getOrCreateChildWithName(APVTSName, nullptr);
    if (!buttonNode.hasProperty("filePath"))
        buttonNode.setProperty("filePath", "", nullptr);

    buttonNode.setProperty("filePath", selectedFilePath, nullptr);

}

void SampleButton::restoreState() {
    auto buttonNode = APVTSRef.state.getChildWithName(APVTSName);


    if (buttonNode.isValid())
    {
      if (!buttonNode.hasProperty("filePath"))
        buttonNode.setProperty("filePath", "", nullptr);

      setFile(buttonNode["filePath"].toString());
    }
}

void SampleButton::setFile(const juce::String &newFile) {
if(selectedFilePath != newFile) {
    if (!newFile.isEmpty() && juce::File::isAbsolutePath(newFile)) {
        juce::File selectedFile(newFile);

        if (selectedFile.existsAsFile()) {
            selectedFilePath = newFile;
            if(!isArranger)
                setButtonText(selectedFile.getFileName());
            saveState();
        }

    } else {
      // Handle invalid file path (e.g., set to nullptr, show "No Sample", etc.)
        selectedFilePath = "";
        setButtonText(originalButtonText);
        saveState();
    }


    if (onFileSelected)
      onFileSelected(selectedFilePath);
    repaint();
    }
  }

void SampleButton::timerCallback() {
    flashOn = !flashOn;
    repaint();
}

void SampleButton::startFlashing() {
    flashOn = false;
    if (isArranger && isPlaying) {
      // GET BPM
      float BPM = APVTSRef.getRawParameterValue("uBPM")->load();
      int ms = (60000.f / BPM);
      startTimer(ms);

    } else if (isPlaying) {
      startTimer(600); 
    }
}

void SampleButton::stopFlashing() {
    stopTimer();
    flashOn = false;
    repaint();
}

void SampleButton::valueTreeChildAdded(juce::ValueTree& parent, juce::ValueTree& child)
{
    if (parent == APVTSRef.state && child.hasType(APVTSName))
    {
        restoreState();
    }
}

void SampleButton::valueTreeChildRemoved(juce::ValueTree& parent, juce::ValueTree& child, int)
{
    if (parent == APVTSRef.state && child.hasType(APVTSName))
    {
        restoreState();
    }
}

void SampleButton::paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown){

    setBufferedToImage(true);
    // Draw the normal button
    juce::TextButton::paintButton(g, isMouseOverButton, isButtonDown);

    if(flashOn) {
        return;
    }


    auto bounds = getLocalBounds();
    // Overlay a white flash if playing and flashOn
    
    // Skip if no file
    if (selectedFilePath.isEmpty() && !editMode) {
        g.setColour(juce::Colours::darkgrey); // Solid color = faster
        g.fillRect(bounds);
        g.setColour(juce::Colours::lightgrey);
        g.setFont (14.0f);
        g.drawFittedText (getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
        return;
    }

    if (isPlaying) {
      // One fillRect only
      auto overlayColour = isArranger ? juce::Colours::darkgreen : juce::Colours::blue;
      g.setColour(overlayColour); // Solid color = faster
      g.fillRect(bounds);

    } else if (editMode) {
      g.setColour(juce::Colours::white); // Solid white, no alpha
      g.fillRect(bounds);
      g.setColour (juce::Colours::black);
      g.setFont (20.0f);
      g.drawFittedText (getButtonText(), getLocalBounds(), juce::Justification::centred, 1);
    }
}
