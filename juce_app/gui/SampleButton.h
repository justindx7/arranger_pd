#pragma once

#include <JuceHeader.h>

class SampleButton : public juce::TextButton, private juce::Timer {
public:
    SampleButton(const juce::String& buttonText);
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

    std::function<void()> onEditModeClick;
    std::function<void()> onNormalClick;

protected:
    void paintButton(juce::Graphics&, bool isMouseOverButton, bool isButtonDown) override;

private:
    juce::String audioFileWildcard = "*.wav;*.mp3;*.aiff;*.flac;*.ogg";
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool editMode = false;
    bool isPlaying = false;

    bool flashOn = false;
    void timerCallback() override;
    void startFlashing();
    void stopFlashing();

    juce::String selectedFilePath;
    std::function<void(juce::String)> onFileSelected;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleButton )
};
