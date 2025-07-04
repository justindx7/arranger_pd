#include "PluginProcessor.h"
#include "juce_core/system/juce_PlatformDefs.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& ref)
    : AudioProcessorEditor (&p), processorRef (p), Reference(ref)
{
    #if JUCE_LINUX
        openGLContext.attachTo(*this);
    #endif

    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //
    addAndMakeVisible(showPresetPanelButton);
    showPresetPanelButton.onClick = [this] () {showPresetPanel();};

    addAndMakeVisible(showOptionsPanelButton);
    showOptionsPanelButton.onClick = [this] () {showOptionsPanel();};

    //Sample buttons
    for (int i = 0; i < sampleButtons.size(); ++i) {
      auto &button = sampleButtons[i];
      addAndMakeVisible(button);

      // Only set sample if a file is selected
       p.getSamplePlayers()[i]->setSample(button.getSelectedFilePath());
       p.getSamplePlayers()[i]->loadSample();

       button.setFileSelectedCallback([&,i](juce::String filePath) {
         p.getSamplePlayers()[i]->setSample(filePath);
         p.getSamplePlayers()[i]->loadSample();
       });

       button.onNormalClick = [&, i]() {
         button.setPlayingState(true);
         p.getSamplePlayers()[i]->playSample();
       };

       p.getSamplePlayers()[i]->onSampleStopped = [&button]() {
         button.setPlayingState(false);
       };
    }

    for (auto &button : fillsButtons) {
        addAndMakeVisible(button);
    }

    for (auto &button : verseButtons) {
        addAndMakeVisible(button);
    }

    // Add the intro button
    addAndMakeVisible(introButton);

    // Add the outro button
    addAndMakeVisible(outroButton);


    addAndMakeVisible(editButton);
    editButton.onClick = [&]() {
      isEditMode = !isEditMode;

      if(isEditMode){
          stopButton.onClick();
      }

      introButton.setEditMode(isEditMode);
      outroButton.setEditMode(isEditMode);
      for (auto &button : sampleButtons) {
          button.setEditMode(isEditMode);
      }
      for (auto &button : verseButtons) {
          button.setEditMode(isEditMode);
      }

      for (auto &button : fillsButtons) {
          button.setEditMode(isEditMode);
      }
    };


    p.getArrangerLogic().initSections(introButton, verseButtons, fillsButtons, outroButton);

    editButton.setButtonText("Edit");

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [&] () {
        p.getArrangerLogic().stop();

        for (size_t i = 0; i < sampleButtons.size(); ++i) {
            auto& button = sampleButtons[i];
            if (button.getPlayingState()) {
                button.setPlayingState(false);
                p.getSamplePlayers()[i]->stopSample();
            }
        }
    };
    

    // Slider for Tempo
    // Add the tempo slider with a range of -10% to +10%, starting at 0%
    addAndMakeVisible(tempoSlider);

    stretchAttachment = std::make_unique<SliderAttachment>(Reference, "uStretch", tempoSlider);

    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 0, 0);
    tempoSlider.setTextValueSuffix("BPM");
    // Set the look and feel for the tempo slider
    tempoSlider.setLookAndFeel(&sliderLookAndFeel); // Set custom look and feel for the slider


    // Add tempo slider label
    addAndMakeVisible(tempoLabel);
    // tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setJustificationType(juce::Justification::centred);
    tempoLabel.setText("0 BPM", juce::dontSendNotification);


    // update the label when slider value changes
    tempoSlider.onValueChange = [this] {
        double tempo = tempoSlider.getValue();
        double bpm = processorRef.getArrangerLogic().getBPM();
        juce::String tempoStr = (tempo > 0 ? "+" : "") + juce::String(tempo, 1);
        tempoLabel.setText(juce::String(bpm + tempo, 1) + " | " +
                               tempoStr + " BPM",
                           juce::dontSendNotification);
    };
    tempoSlider.onValueChange();

    // Slider for Reverb
    addAndMakeVisible(reverbSlider);
    reverbAttachment = std::make_unique<SliderAttachment>(Reference, "uDryWet", reverbSlider);
    reverbSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    reverbSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 0, 0);
    reverbSlider.setTextValueSuffix("%");
    // Set the look and feel for the reverb slider
    reverbSlider.setLookAndFeel(&sliderLookAndFeel); // Set custom look and feel for the slider

    // Add reverb slider label
    addAndMakeVisible(reverbLabel);
    reverbLabel.setText("Reverb 0%", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred);

    // update the label when slider value changes
    reverbSlider.onValueChange = [this] {
        double reverbValue = reverbSlider.getValue();
        reverbLabel.setText("Reverb " + juce::String(reverbValue, 1) + "%", juce::dontSendNotification);
    };
    
    reverbSlider.onValueChange();
    
    
    // Set the size of the editor
    // Set the size of the editor to 90% of the screen size
    auto screenBounds = juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    int width = static_cast<int>(screenBounds.getWidth() * 0.9);
    int height = static_cast<int>(screenBounds.getHeight() * 0.9);
    setSize(width, height);
    // Set the editor to be resizable
    setResizable(true, false);
    // Enforce 16:9 aspect ratio constraint
    // Dynamically set the aspect ratio based on the screen's aspect ratio
    //
    //double screenAspectRatio = static_cast<double>(screenBounds.getWidth()) / static_cast<double>(screenBounds.getHeight());
    //getConstrainer()->setFixedAspectRatio(screenAspectRatio);
    
    processorRef.getMidiHandler().setRepaintCallback([this] {
      juce::MessageManager::callAsync([this] { this->repaint(); });
      if(presetPanel)
        presetPanel->refreshPresetList();
    });
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {

  tempoSlider.setLookAndFeel(nullptr);
  reverbSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (20.0f);
    juce::String currentPreset = processorRef.getPresetManager().getCurrentPreset();
    g.drawFittedText (currentPreset, getLocalBounds(), juce::Justification::topLeft, 1);

    tempoSlider.onValueChange();
    reverbSlider.onValueChange();
    //color for easy debugging
    // g.fillRect(tempoSlider.getBounds().toFloat().reduced(2.0f));

}

void AudioPluginAudioProcessorEditor::resized()
{

    // Calculate the height of the "Arranger" text
    // Define individual spacings for each group

    const int fillSpacing = 30;
    const int verseSpacing = 30;
    const int editSpacing = 15;

    // =========== Sample Buttons Layout ===========
    // Sample buttons are arranged in a grid of 2 rows and 4 columns
    // Sample buttons in 2 rows of 4 using FlexBox, keeping square ratio
    const int numRows = 2;
    const int numCols = 4;

    // Calculate the maximum square size that fits in the available area
    int margin = juce::roundToInt(getWidth() * 0.01f); // 1% of the width
    int totalMarginWidth = (numCols + 1) * margin;
    int totalMarginHeight = (numRows + 1) * margin;

    int availableWidth = getWidth() - totalMarginWidth;
    int availableHeight = getHeight() * 0.4f - totalMarginHeight; // Use 40% of the height for sample buttons

    int buttonSizeByWidth = availableWidth / numCols;
    int buttonSizeByHeight = availableHeight / numRows;
    int buttonSize = std::min(buttonSizeByWidth, buttonSizeByHeight);

    int buttonWidth = buttonSize;
    int buttonHeight = buttonSize;
    int halfButtonSize = buttonSize / 2;

    auto sampleArea = getLocalBounds().removeFromTop(numRows * (buttonHeight + 2 * margin));


    int titleFontHeight = 15;
    int titleMargin = 10;
    juce::Rectangle<int> titleBounds(10, 10, 200, titleFontHeight + 4);
    // Place the showPresetPanelButton below the "Arranger" text
    int buttonX = titleBounds.getX();
    int buttonY = titleBounds.getBottom() + titleMargin;
    showPresetPanelButton.setBounds(buttonX, buttonY, buttonSize, halfButtonSize);
    showOptionsPanelButton.setBounds(buttonX, buttonY + halfButtonSize +  titleMargin, buttonSize, halfButtonSize);


    for (int row = 0; row < numRows; ++row)
    {
        juce::FlexBox rowFlexBox;
        rowFlexBox.flexDirection = juce::FlexBox::Direction::row;
        rowFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
        rowFlexBox.alignItems = juce::FlexBox::AlignItems::center;

        for (int col = 0; col < numCols; ++col)
        {
            int idx = row * numCols + col;
            if (idx < sampleButtons.size())
            {
                rowFlexBox.items.add(
                    juce::FlexItem(sampleButtons[idx])
                        .withWidth(buttonWidth)
                        .withHeight(buttonHeight)
                        .withMargin(margin)
                );
            }
        }
        // Calculate bounds for this row
        auto rowBounds = sampleArea.removeFromTop(buttonHeight + 2 * margin);
        rowFlexBox.performLayout(rowBounds);
    }


// Tempo slider
// Set the size of the tempo slider to match the sample button width and 1/4 of its height);
// Place the tempo slider next to sample 4 (index 3), scaling proportionally with sample button size
if (sampleButtons.size() >= 4)
{
    // Get the bounds of sample button 4 (index 3)
    auto& sample4 = sampleButtons[3];
    // Get the bounds of the sample button to position the tempo slider
    auto bounds = sample4.getBounds();
    // Slider width and height proportional to sample button size
    int sliderWidth = buttonWidth * 2;
    int sliderHeight = buttonHeight * 0.25;
    int sliderX = bounds.getRight() + margin;
    int sliderY = bounds.getY() + (bounds.getHeight() - sliderHeight) / 2;
    
    // Get the component's total bounds to check boundaries
    auto componentBounds = getLocalBounds();
    
    // Check if slider would go out of bounds horizontally and adjust width only
    if (sliderX + sliderWidth > componentBounds.getWidth())
    {
        sliderWidth = componentBounds.getWidth() - sliderX - margin;
        // Ensure minimum width
        if (sliderWidth < 50)
        {
            sliderWidth = 50;
        }
    }
    
    tempoSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    
    // Position the tempo label above the tempo slider, proportional height
    int labelHeight = sliderHeight * 0.8;
    tempoLabel.setBounds(tempoSlider.getX(), tempoSlider.getY() - labelHeight - margin / 2, tempoSlider.getWidth(), labelHeight);
}

// Reverb slider
// Place the reverb slider to the right of sample 8 (index 7), scaling proportionally
if (sampleButtons.size() >= 8)
{
    // Get the bounds of sample button 8 (index 7)
    auto& sample8 = sampleButtons[7];
    auto bounds = sample8.getBounds();
    int sliderWidth = buttonWidth * 2;
    int sliderHeight = buttonHeight * 0.25;
    int sliderX = bounds.getRight() + margin;
    int sliderY = bounds.getY() + (bounds.getHeight() - sliderHeight) / 2;
    
    // Get the component's total bounds to check boundaries
    auto componentBounds = getLocalBounds();
    
    // Check if slider would go out of bounds horizontally and adjust width only
    if (sliderX + sliderWidth > componentBounds.getWidth())
    {
        sliderWidth = componentBounds.getWidth() - sliderX - margin;
        // Ensure minimum width
        if (sliderWidth < 50)
        {
            sliderWidth = 50;
        }
    }
    
    reverbSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
    
    // Position the reverb label above the reverb slider, proportional height
    int labelHeight = sliderHeight * 0.8;
    reverbLabel.setBounds(reverbSlider.getX(), reverbSlider.getY() - labelHeight - margin / 2, reverbSlider.getWidth(), labelHeight);
}

    // =========== Fills and Verses Layout with FlexBox ===========

    // Define area for fills and verses at the bottom 40% of the editor
    int bottomAreaHeight = getHeight() * 0.52f;
    auto bottomArea = getLocalBounds().removeFromBottom(bottomAreaHeight);

    // Calculate margins and button size for 4x2 grid (fills on top row, verses
    // on bottom row)
    const int numFills = 4;
    const int numVerses = 4;
    const int bottomNumCols = 4;
    const int bottomNumRows = 2;

    int bottomMargin = juce::roundToInt(getWidth() * 0.005f); // 1% of width
    bottomMargin = std::max(bottomMargin, 2); // Minimum margin of 2 pixels

    // Reserve space for intro/outro buttons (6 total columns: intro + 4 fills +
    // outro)
    const int totalCols = 6;
    int bottomTotalMarginWidth = (totalCols + 1) * bottomMargin;

    // Add extra margin at the bottom for better spacing
    int extraBottomMargin = bottomMargin * 2; // Reduced from 5 to 2
    int bottomTotalMarginHeight =
        (bottomNumRows + 1) * bottomMargin + extraBottomMargin;

    // Calculate the maximum square size that fits in the available area
    int bottomAvailableWidth = bottomArea.getWidth() - bottomTotalMarginWidth;
    int bottomAvailableHeight =
        bottomArea.getHeight() - bottomTotalMarginHeight;

    // Ensure minimum available space
    bottomAvailableWidth = std::max(bottomAvailableWidth,
                                    totalCols * 20); // Minimum 20px per column
    bottomAvailableHeight = std::max(
        bottomAvailableHeight, bottomNumRows * 20); // Minimum 20px per row

    int bottomButtonSizeByWidth = bottomAvailableWidth / totalCols;
    int bottomButtonSizeByHeight = bottomAvailableHeight / bottomNumRows;
    int bottomButtonSize =
        std::min(bottomButtonSizeByWidth, bottomButtonSizeByHeight);

    // Ensure minimum button size
    bottomButtonSize = std::max(bottomButtonSize, 15);

    int bottomButtonWidth = bottomButtonSize;
    int bottomButtonHeight = bottomButtonSize;

    // Calculate the actual space needed for the button grid
    int actualGridWidth =
        totalCols * bottomButtonWidth + (totalCols + 1) * bottomMargin;
    int actualGridHeight = bottomNumRows * bottomButtonHeight +
                           (bottomNumRows + 1) * bottomMargin +
                           extraBottomMargin;

    // Center the grid if there's extra space
    int gridStartX =
        bottomArea.getX() + (bottomArea.getWidth() - actualGridWidth) / 2;
    int gridStartY =
        bottomArea.getY() + (bottomArea.getHeight() - actualGridHeight) / 2;

    // Ensure grid doesn't go outside bounds
    gridStartX = std::max(gridStartX, bottomArea.getX());
    gridStartY = std::max(gridStartY, bottomArea.getY());

    // Create areas for each row within the centered grid
    auto fillsRowArea =
        juce::Rectangle<int>(gridStartX, gridStartY + bottomMargin,
                             actualGridWidth, bottomButtonHeight);

    auto versesRowArea = juce::Rectangle<int>(
        gridStartX,
        gridStartY + bottomMargin + bottomButtonHeight + bottomMargin,
        actualGridWidth, bottomButtonHeight);

    // =========== Position Intro and Outro Buttons ===========

    // Intro button - positioned at the start of the grid
    int introX = gridStartX + bottomMargin;
    int introY = fillsRowArea.getY();
    int introOutroButtonSize = bottomButtonWidth;

    // Ensure intro button stays within bounds
    introX = std::max(introX, bottomArea.getX());
    introX = std::min(introX, bottomArea.getRight() - introOutroButtonSize);

    introButton.setBounds(introX, introY, introOutroButtonSize,
                          introOutroButtonSize);

    // Outro button - positioned at the end of the grid
    int outroX =
        gridStartX + actualGridWidth - bottomMargin - introOutroButtonSize;
    int outroY = fillsRowArea.getY();

    // Ensure outro button stays within bounds
    outroX = std::max(outroX, bottomArea.getX());
    outroX = std::min(outroX, bottomArea.getRight() - introOutroButtonSize);

    outroButton.setBounds(outroX, outroY, introOutroButtonSize,
                          introOutroButtonSize);

    // =========== Fills FlexBox Layout ===========

    juce::FlexBox fillsFlexBox;
    fillsFlexBox.flexDirection = juce::FlexBox::Direction::row;
    fillsFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    fillsFlexBox.alignItems = juce::FlexBox::AlignItems::center;

    // Create area for fills (between intro and outro buttons)
    auto fillsArea = juce::Rectangle<int>(
        introButton.getRight() + bottomMargin, fillsRowArea.getY(),
        outroButton.getX() - introButton.getRight() - 2 * bottomMargin,
        fillsRowArea.getHeight());

    // Create buttons for fills
    for (int i = 0; i < numFills && i < fillsButtons.size(); ++i) {
        fillsFlexBox.items.add(
            juce::FlexItem(fillsButtons[i])
                .withWidth(bottomButtonWidth)
                .withHeight(bottomButtonHeight)
                .withMargin(bottomMargin /
                            2) // Smaller margin within the fills area
        );
    }

    // Perform layout for fills
    fillsFlexBox.performLayout(fillsArea);

    // =========== Verses FlexBox Layout ===========

    juce::FlexBox versesFlexBox;
    versesFlexBox.flexDirection = juce::FlexBox::Direction::row;
    versesFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    versesFlexBox.alignItems = juce::FlexBox::AlignItems::center;

    // Create area for verses (full width, centered)
    auto versesArea = juce::Rectangle<int>(
        gridStartX + bottomMargin, versesRowArea.getY(),
        actualGridWidth - 2 * bottomMargin, versesRowArea.getHeight());

    // Create buttons for verses
    for (int i = 0; i < numVerses && i < verseButtons.size(); ++i) {
        versesFlexBox.items.add(juce::FlexItem(verseButtons[i])
                                    .withWidth(bottomButtonWidth)
                                    .withHeight(bottomButtonHeight)
                                    .withMargin(bottomMargin / 2));
    }

    // Perform layout for verses
    versesFlexBox.performLayout(versesArea);

    // =========== Edit and Stop Buttons Layout ===========

    // Calculate the size of the edit and stop buttons
    const int editStopButtonSize =
        std::max(bottomButtonWidth / 3, 15); // Minimum 15px

    // Position edit button aligned with intro button
    int editX = introButton.getX();
    int editY = versesRowArea.getBottom() + bottomMargin;

    // Ensure edit button stays within bottom area bounds
    editY = std::min(editY, bottomArea.getBottom() - editStopButtonSize);
    editX = std::max(editX, bottomArea.getX());
    editX = std::min(editX, bottomArea.getRight() - editStopButtonSize);

    editButton.setBounds(editX, editY, editStopButtonSize, editStopButtonSize);

    // Position stop button aligned with outro button
    int stopX = outroButton.getRight() - editStopButtonSize;
    int stopY = versesRowArea.getBottom() + bottomMargin;

    // Ensure stop button stays within bottom area bounds
    stopY = std::min(stopY, bottomArea.getBottom() - editStopButtonSize);
    stopX = std::max(stopX, bottomArea.getX());
    stopX = std::min(stopX, bottomArea.getRight() - editStopButtonSize);

    stopButton.setBounds(stopX, stopY, editStopButtonSize, editStopButtonSize);

    // =========== End Layout ===========
}
