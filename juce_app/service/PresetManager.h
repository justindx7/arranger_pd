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

  void assignMidiProgram(const juce::String &presetName, int midiProgram);
  void removeMidiProgram(const juce::String &presetName);
  juce::String getPresetNameForMidiProgram(int midiProgram) const;

  static juce::StringArray getAllPresets();
  juce::String getCurrentPreset();
  
  const juce::HashMap<juce::String, int> &getMidiProgramAssignments() { return midiProgramAssignments; }

private:
  void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;
  juce::HashMap<juce::String, int> midiProgramAssignments;
  juce::HashMap<int, juce::String> midiProgramToPreset;

  juce::AudioProcessorValueTreeState &valueTreeState;
  juce::Value currentPreset;
};
