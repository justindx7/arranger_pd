#pragma once

#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener {
public:
  static const juce::File defaultDirectory;
  static const juce::String extension;
  static const juce::String presetNameProperty;

  explicit PresetManager(juce::AudioProcessorValueTreeState &);

  void savePreset(const juce::String &presetName);
  void deletePreset(const juce::String &presetName);
  void loadPreset(const juce::String &presetName);
  int loadNextPreset();
  int loadPreviousPreset();

  static juce::StringArray getAllPresets();
  juce::String getCurrentPreset();

private:
  void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;

  juce::AudioProcessorValueTreeState &valueTreeState;
  juce::Value currentPreset;
};
