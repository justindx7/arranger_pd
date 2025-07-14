// MidiAssignComponent.h
#pragma once
#include <JuceHeader.h>

class MidiAssignComponent : public juce::Component
{
public:
    MidiAssignComponent(std::function<void(int)> onAssign, std::function<void()> onRemove = nullptr)
    {
        addAndMakeVisible(editor);
        editor.setInputRestrictions(3, "0123456789");
        editor.setFont(juce::Font(28.0f));
        editor.setText("1");
        editor.setJustification(juce::Justification::centred);

        addAndMakeVisible(okButton);
        okButton.setButtonText("Assign");
        okButton.onClick = [this, onAssign] {
            int value = editor.getText().getIntValue();
            if (value >= 1 && value <= 128){

                int pg1IsZeroStandard = value - 1;

                if (onAssign) onAssign(pg1IsZeroStandard);
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
        setSize(320, 220);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        area.removeFromTop(36); // Reserve space for label
        editor.setBounds(area.removeFromTop(60));
        area.removeFromTop(20);

        // Increase button height for touch screens
        const int buttonHeight = 112;
        auto buttonArea = area.removeFromTop(buttonHeight);

        okButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 3).reduced(5));
        removeButton.setBounds(buttonArea.removeFromLeft(buttonArea.getWidth() / 2).reduced(5));
        cancelButton.setBounds(buttonArea.reduced(5));
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.setFont(20.0f);
        g.drawFittedText("Enter MIDI Program (1-127):", getLocalBounds().withHeight(36), juce::Justification::centred, 1);
    }

private:
    juce::TextEditor editor;
    juce::TextButton okButton, removeButton, cancelButton;
};
