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
    
    
    // Set the size of the app with a fixed aspect ratio (e.g., 16:9) and lock resizing to that ratio
    const int width = 1412;
    const int height = juce::roundToInt(width * 9.0 / 16.0);
    setSize(width, height);
    setResizable(true, false); // Allow resizing, but only horizontally

    // Enforce fixed aspect ratio (16:9)
    getConstrainer()->setFixedAspectRatio(16.0 / 9.0);

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

    // Sample buttons in 2 rows of 4 using FlexBox, keeping square ratio
    const int numRows = 2;
    const int numCols = 4;

    // Calculate the maximum square size that fits in the available area
    int margin = juce::roundToInt(getWidth() * 0.01f); // 1% of the width
    int totalMarginWidth = (numCols + 1) * margin;
    int totalMarginHeight = (numRows + 1) * margin;

    int availableWidth = getWidth() - totalMarginWidth;
    int availableHeight = getHeight() * 0.4f - totalMarginHeight; // 20% of height for sample buttons

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
}

    // // Fills buttons
    // const int fillsButtonSize = 180;
    // const int numFills = fillsButtons.size();
    // int totalFillsWidth = numFills * fillsButtonSize + (numFills - 1) * fillSpacing;
    // int fillsX0 = (getWidth() - totalFillsWidth) / 2;
    // int fillsY0 = getHeight() - 2.5 * (fillsButtonSize + fillSpacing);

    // for (int i = 0; i < numFills; ++i)
    // {
    //     int x = fillsX0 + i * (fillsButtonSize + fillSpacing);
    //     int y = fillsY0;
    //     fillsButtons[i].setBounds(x, y, fillsButtonSize, fillsButtonSize);
    // }

    // // Verse buttons
    // const int verseButtonSize = 180;
    // const int numVerses = verseButtons.size();
    // int totalVersesWidth = numVerses * verseButtonSize + (numVerses - 1) * verseSpacing;
    // int verseX0 = (getWidth() - totalVersesWidth) / 2;
    // int verseY0 = fillsY0 + fillsButtonSize + verseSpacing;

    // for (int i = 0; i < numVerses; ++i)
    // {
    //     int x = verseX0 + i * (verseButtonSize + verseSpacing);
    //     int y = verseY0;
    //     verseButtons[i].setBounds(x, y, verseButtonSize, verseButtonSize);
    // }

    // // Intro and Outro buttons
    // const int introOutroButtonWidth = 180;
    // const int introOutroButtonHeight = fillsButtonSize;

    // // Intro button to the left of the first fill button
    // int introX = fillsX0 - introOutroButtonWidth - fillSpacing;
    // int introY = fillsY0;
    // introButton.setBounds(introX, introY, introOutroButtonWidth, introOutroButtonHeight);

    // // Outro button to the right of the last fill button
    // int outroX = fillsX0 + numFills * (fillsButtonSize + fillSpacing);
    // int outroY = fillsY0;
    // outroButton.setBounds(outroX, outroY, introOutroButtonWidth, introOutroButtonHeight);

    // // Edit and Stop buttons
    // // Edit button below the intro button
    // const int editStopButtonSize = 100;
    // int editX = introX;
    // int editY = verseY0 + editStopButtonSize - editSpacing;
    // editButton.setBounds(editX, editY, editStopButtonSize, editStopButtonSize);

    // // Stop button below the outro button
    // int stopX = outroX + verseButtonSize - editStopButtonSize;;
    // int stopY = verseY0 + editStopButtonSize - editSpacing;
    // stopButton.setBounds(stopX, stopY, editStopButtonSize, editStopButtonSize);
