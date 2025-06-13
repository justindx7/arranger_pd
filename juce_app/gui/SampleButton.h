#pragma once

#include <JuceHeader.h>

class SampleButton : public juce::TextButton {
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

    std::function<void()> onEditModeClick;
    std::function<void()> onNormalClick;

private:
    juce::String audioFileWildcard = "*.wav;*.mp3;*.aiff;*.flac;*.ogg";
    std::unique_ptr<juce::FileChooser> fileChooser;
    bool editMode = false;
    bool isPlaying = false;

    juce::String selectedFilePath;
    std::function<void(juce::String)> onFileSelected;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleButton )
};
