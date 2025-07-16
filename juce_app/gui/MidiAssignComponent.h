// MidiAssignComponent.h
#pragma once
#include <JuceHeader.h>

class MidiAssignComponent : public juce::Component
{
public:
    MidiAssignComponent(std::function<void(int, int)> onAssign, std::function<void()> onRemove = nullptr)
    {
        addAndMakeVisible(programEditor);
        programEditor.setInputRestrictions(3, "0123456789");
        programEditor.setFont(juce::Font(28.0f));
        programEditor.setText("1");
        programEditor.setJustification(juce::Justification::centred);

        addAndMakeVisible(channelEditor);
        channelEditor.setInputRestrictions(2, "0123456789");
        channelEditor.setFont(juce::Font(28.0f));
        channelEditor.setText("1");
        channelEditor.setJustification(juce::Justification::centred);

        addAndMakeVisible(okButton);
        okButton.setButtonText("Assign");
        okButton.onClick = [this, onAssign] {
            int programValue = programEditor.getText().getIntValue();
            int channelValue = channelEditor.getText().getIntValue();
            if (programValue >= 1 && programValue <= 128 && channelValue >= 1 && channelValue <= 16){
                int pg1IsZeroOffsetStandard = programValue - 1;
                if (onAssign) onAssign(pg1IsZeroOffsetStandard , channelValue);
            }
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };

        addAndMakeVisible(removeButton);
        removeButton.setButtonText("Remove");
        removeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        removeButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        removeButton.onClick = [this, onRemove] {
            if (onRemove) onRemove();
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };

        addAndMakeVisible(cancelButton);
        cancelButton.setButtonText("Cancel");
        cancelButton.onClick = [this] {
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };
        setSize(320, 280);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);

        // Program label and editor
        auto programLabelArea = area.removeFromTop(24);
        programEditor.setBounds(area.removeFromTop(40));

        area.removeFromTop(10);

        // Channel label and editor
        auto channelLabelArea = area.removeFromTop(24);
        channelEditor.setBounds(area.removeFromTop(40));

        area.removeFromTop(10);

        // Buttons at the bottom (increased height for touch screens)
        auto buttonArea = area.removeFromBottom(96); // Increased from 48 to 72
        int buttonWidth = buttonArea.getWidth() / 3;
        okButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
        removeButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
        cancelButton.setBounds(buttonArea.reduced(5));

        // Store label areas for paint
        programLabelBounds = programLabelArea;
        channelLabelBounds = channelLabelArea;
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(18.0f);

        g.drawFittedText("Enter MIDI Program (1-128):", programLabelBounds, juce::Justification::centred, 1);
        g.drawFittedText("Enter MIDI Channel (1-16):", channelLabelBounds, juce::Justification::centred, 1);
    }

private:
    juce::TextEditor programEditor;
    juce::TextEditor channelEditor;
    juce::TextButton okButton, removeButton, cancelButton;

    juce::Rectangle<int> programLabelBounds, channelLabelBounds;
};
