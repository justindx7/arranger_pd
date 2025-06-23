#pragma once

#include <JuceHeader.h>

class SampleEditPopup : public juce::Component
{
public:
    SampleEditPopup(std::function<void()> onLoad, std::function<void()> onRemove)
    {
        loadButton.setButtonText("Load Sample");
        removeButton.setButtonText("Remove Sample");

        loadButton.onClick = [this, onLoad]() {
            if (onLoad) onLoad();
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };
        removeButton.onClick = [this, onRemove]() {
            if (onRemove) onRemove();
            if (auto* w = findParentComponentOfClass<juce::DialogWindow>())
                w->closeButtonPressed();
        };

        addAndMakeVisible(loadButton);
        addAndMakeVisible(removeButton);
    }

    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        loadButton.setBounds(area.removeFromTop(60).reduced(0, 10));
        removeButton.setBounds(area.removeFromTop(60).reduced(0, 10));
    }

private:
    juce::TextButton loadButton, removeButton;
};


class SampleButton : public juce::TextButton,public juce::ValueTree::Listener, private juce::Timer {
public:
    SampleButton(const juce::String& buttonText, juce::AudioProcessorValueTreeState& Reference, bool isArrangeButton = false);
    ~SampleButton () override;

    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void clicked() override;

    void setEditMode(bool shouldBeInEditMode);
    bool isInEditMode() const;
    void enterEditMode();

    void setFileSelectedCallback(std::function<void(juce::String)> callback);
    juce::String getSelectedFilePath() const;

    void setPlayingState(bool playing);
    bool getPlayingState() const;

    void setFile(const juce::String &newFile);
    void saveState();
    void restoreState();


    std::function<void()> onEditModeClick;
    std::function<void()> onNormalClick;

protected:
    void paintButton(juce::Graphics&, bool isMouseOverButton, bool isButtonDown) override;

private:
    void valueTreeChildAdded(juce::ValueTree& parent, juce::ValueTree& child) override;
    void valueTreeChildRemoved(juce::ValueTree& parent, juce::ValueTree& child, int) override;
    //void valueTreeChildChanged(juce::ValueTree& parent, juce::ValueTree& child)

    juce::String originalButtonText;
    juce::String APVTSName;


    juce::AudioProcessorValueTreeState& APVTSRef;

    juce::String audioFileWildcard = "*.wav;*.mp3;*.aiff;*.flac;*.ogg";
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool editMode = false;
    bool isPlaying = false;

    bool isArranger;

    bool flashOn = false;
    void timerCallback() override;
    void startFlashing();
    void stopFlashing();

    juce::String selectedFilePath;
    std::function<void(juce::String)> onFileSelected;

    juce::String makeValidXmlName(const juce::String &name) {
      juce::String result;
      for (int i = 0; i < name.length(); ++i) {
        juce::juce_wchar c = name[i];
        if ((i == 0 && (juce::CharacterFunctions::isLetter(c) || c == '_')) ||
            (i > 0 &&
             (juce::CharacterFunctions::isLetterOrDigit(c) || c == '_'))) {
          result += c;
        } else {
          result += '_'; // Replace illegal chars with underscore
        }
      }
      if (result.isEmpty())
        result = "_";
      return result;
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleButton )
};


