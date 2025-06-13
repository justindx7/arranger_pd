#include "PluginProcessor.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p, juce::AudioProcessorValueTreeState& ref)
    : AudioProcessorEditor (&p), processorRef (p), Reference(ref)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    //Sample buttons
    int sampleNumber = 1;
    for (auto &button : sampleButtons) {
        addAndMakeVisible(button);
        button.setButtonText("Sample " + juce::String(sampleNumber++));
    }


    int fillNumber = 1;
    for (auto &button : fillsButtons) {
        addAndMakeVisible(button);
        button.setButtonText("Fill " + juce::String(fillNumber++));
    }

    int verseNumber = 1;
    for (auto &button : verseButtons) {
        addAndMakeVisible(button);
        button.setButtonText("Verse " + juce::String(verseNumber++));
    }
    // sample1.onClick = [&](){p.testPlayer.playSample();};

    // Add the intro and outro buttons
    addAndMakeVisible(introButton);
    introButton.setButtonText("Intro");

    addAndMakeVisible(outroButton);
    outroButton.setButtonText("Outro");

    // Add the edit and stop buttons
    addAndMakeVisible(editButton);
    editButton.setButtonText("Edit");
    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop");
    
    // Slider for Tempo
    // Add the tempo slider with a range of -10% to +10%, starting at 0%
    addAndMakeVisible(tempoSlider);
    tempoSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    // tempoSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 80, 20);
    tempoSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 0, 0);
    tempoSlider.setRange(-10.0, +10.0, 0.1);
    tempoSlider.setValue(0.0);
    tempoSlider.setTextValueSuffix("BPM");

    // Add tempo slider label
    addAndMakeVisible(tempoLabel);
    // tempoLabel.setText("Tempo", juce::dontSendNotification);
    tempoLabel.setJustificationType(juce::Justification::centred);
    tempoLabel.setText("0.0 BPM", juce::dontSendNotification);

    // update the label when slider value changes
    tempoSlider.onValueChange = [this] {
        double tempo = tempoSlider.getValue();
        tempoLabel.setText(juce::String(tempo, 1) + " BPM", juce::dontSendNotification);
    };


    // Slider for Reverb
    addAndMakeVisible(reverbSlider);
    reverbSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    reverbSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 0, 0);
    reverbSlider.setRange(0.0, 100.0, 0.1);
    reverbSlider.setValue(0.0); // Default value at 50%
    reverbSlider.setTextValueSuffix("%");

    // Add reverb slider label
    addAndMakeVisible(reverbLabel);
    reverbLabel.setText("Reverb 0%", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred);

    // update the label when slider value changes
    reverbSlider.onValueChange = [this] {
        double reverbValue = reverbSlider.getValue();
        reverbLabel.setText("Reverb " + juce::String(reverbValue, 1) + "%", juce::dontSendNotification);
    };
    
    
    // Set the size of the editor
    // Set the size of the editor to 90% of the screen size
    auto screenBounds = juce::Desktop::getInstance().getDisplays().getMainDisplay().userArea;
    int width = static_cast<int>(screenBounds.getWidth() * 0.9);
    int height = static_cast<int>(screenBounds.getHeight() * 0.9);
    setSize(width, height);
    // Set the editor to be resizable
    setResizable(true, true);
    // Enforce 16:9 aspect ratio constraint
    // Dynamically set the aspect ratio based on the screen's aspect ratio
    double screenAspectRatio = static_cast<double>(screenBounds.getWidth()) / static_cast<double>(screenBounds.getHeight());
    getConstrainer()->setFixedAspectRatio(screenAspectRatio);
    
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Arranger", getLocalBounds(), juce::Justification::topLeft, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
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

    auto sampleArea = getLocalBounds().removeFromTop(numRows * (buttonHeight + 2 * margin));

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
        reverbSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);

        // Position the reverb label above the reverb slider, proportional height
        int labelHeight = sliderHeight * 0.8;
        reverbLabel.setBounds(reverbSlider.getX(), reverbSlider.getY() - labelHeight - margin / 2, reverbSlider.getWidth(), labelHeight);
    }

    // =========== End Sample Buttons Layout ===========
        
    // =========== Fills and Verses Layout with FlexBox ===========

    // Define area for fills and verses at the bottom 40% of the editor
    int bottomAreaHeight = getHeight() * 0.52f;
    auto bottomArea = getLocalBounds().removeFromBottom(bottomAreaHeight);

    // Calculate margins and button size for 4x2 grid (fills on top row, verses on bottom row)
    const int numFills = 4;
    const int numVerses = 4;
    const int bottomNumCols = 4;
    const int bottomNumRows = 2;

    int bottomMargin = juce::roundToInt(getWidth() * 0.01f); // 1% of width
    int bottomTotalMarginWidth = (bottomNumCols + 1) * bottomMargin;
    int bottomTotalMarginHeight = (bottomNumRows + 1) * bottomMargin;

    int bottomAvailableWidth = bottomArea.getWidth() - bottomTotalMarginWidth;
    int bottomAvailableHeight = bottomArea.getHeight() - bottomTotalMarginHeight;

    int bottomButtonSizeByWidth = bottomAvailableWidth / bottomNumCols;
    int bottomButtonSizeByHeight = bottomAvailableHeight / bottomNumRows;
    int bottomButtonSize = std::min(bottomButtonSizeByWidth, bottomButtonSizeByHeight);

    int bottomButtonWidth = bottomButtonSize;
    int bottomButtonHeight = bottomButtonSize;

    // Fills row (top row)
    juce::FlexBox fillsFlexBox;
    fillsFlexBox.flexDirection = juce::FlexBox::Direction::row;
    fillsFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    fillsFlexBox.alignItems = juce::FlexBox::AlignItems::center;

    // Create buttons for fills
    // Ensure we only add as many buttons as available
    for (int i = 0; i < numFills && i < fillsButtons.size(); ++i)
    {
        fillsFlexBox.items.add(
            juce::FlexItem(fillsButtons[i])
                // Set the width and height of each fill button
                .withWidth(bottomButtonWidth)
                .withHeight(bottomButtonHeight)
                .withMargin(bottomMargin)
        );
    }

    // Verses row (bottom row)
    juce::FlexBox versesFlexBox;
    versesFlexBox.flexDirection = juce::FlexBox::Direction::row;
    versesFlexBox.justifyContent = juce::FlexBox::JustifyContent::center;
    versesFlexBox.alignItems = juce::FlexBox::AlignItems::center;

    // Create buttons for verses
    // Ensure we only add as many buttons as available
    for (int i = 0; i < numVerses && i < verseButtons.size(); ++i)
    {
        versesFlexBox.items.add(
            juce::FlexItem(verseButtons[i])
                // Set the width and height of each verse button
                .withWidth(bottomButtonWidth)
                .withHeight(bottomButtonHeight)
                .withMargin(bottomMargin)
        );
    }

    // Split bottomArea into two rows for fills and verses
    auto fillsRowArea = bottomArea.removeFromTop(static_cast<int>(bottomButtonHeight + 2 * margin));
    auto versesRowArea = bottomArea.removeFromTop(static_cast<int>(bottomButtonHeight + 2 * margin));

    // Perform layout for fills and verses using FlexBox
    fillsFlexBox.performLayout(fillsRowArea);
    versesFlexBox.performLayout(versesRowArea);

    // =========== End Fills and Verses Layout ===========


    // =========== Intro, Outro, Edit, and Stop Buttons Layout ===========

    // Calculate positions for intro, outro, edit, and stop buttons
    // Get the bounds of the first and last fill buttons
    int fillsX0 = fillsButtons.size() > 0 ? fillsButtons[0].getBounds().getX() : bottomArea.getX();
    int fillsY0 = fillsButtons.size() > 0 ? fillsButtons[0].getBounds().getY() : bottomArea.getY();
    int fillsButtonSize = bottomButtonWidth;

    // Intro and Outro buttons - scale with fill buttons, stay squared, and keep same spacing
    int introOutroButtonSize = bottomButtonWidth; // Keep square, same as fill button

    // Intro button on the left of the fills row, with same spacing as between fill buttons
    int introX = fillsX0 - bottomMargin - introOutroButtonSize;
    // If there are fills, position it before the first fill button
    // If no fills, position it at the same X as the first fill button
    int introY = fillsY0;
    introButton.setBounds(introX, introY, introOutroButtonSize, introOutroButtonSize);

    // Outro button on the right of the fills row, with same spacing as between fill buttons
    // If there are fills, position it after the last fill button
    int outroX = (fillsButtons.size() > 0)
        ? fillsButtons[numFills - 1].getBounds().getRight() + bottomMargin
        // If no fills, position it at the same X as intro button
        : fillsX0 + numFills * (fillsButtonSize + bottomMargin);
    int outroY = fillsY0;
    outroButton.setBounds(outroX, outroY, introOutroButtonSize, introOutroButtonSize);


    // Edit and Stop buttons

    // Edit button 
    // Calculate the size of the edit and stop buttons based on the bottom button width
    const int editStopButtonSize = bottomButtonWidth / 2;
    // Align edit button X with intro button, Y with verse row
    int editX = introButton.getX() ;
    int editY = versesRowArea.getY() + editStopButtonSize;
    editButton.setBounds(editX, editY, editStopButtonSize, editStopButtonSize);

    // Stop button
    // Align stop button X with outro button, Y with verse row

    int stopX = outroButton.getX() + editStopButtonSize;
    int stopY = versesRowArea.getY() + editStopButtonSize;
    stopButton.setBounds(stopX, stopY, editStopButtonSize, editStopButtonSize);

    // int stopX = outroButton.getX() + verseButtonSize - editStopButtonSize;
    // int stopY = verseY0 + editStopButtonSize - editSpacing;
    // stopButton.setBounds(stopX, stopY, editStopButtonSize, editStopButtonSize);
}