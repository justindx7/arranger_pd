#pragma once

#include <JuceHeader.h>

class PresetManager : juce::ValueTree::Listener {
public:
  static const juce::File defaultDirectory;
  static const juce::String extension;
  static const juce::String presetNameProperty;
  static const juce::File midiAssignFile;
  static const juce::File presetCategoryFile;

  explicit PresetManager(juce::AudioProcessorValueTreeState &);

  void savePreset(const juce::String &presetName);
  void deletePreset(const juce::String &presetName);
  void loadPreset(const juce::String &presetName);

  int loadNextPreset();
  int loadPreviousPreset();

  void assignMidiProgram(const juce::String &presetName, int midiProgram);
  void removeMidiProgram(const juce::String &presetName);
  juce::String getPresetNameForMidiProgram(int midiProgram) const;
  
  void saveMidiAssignments();
  void loadMidiAssignments();

  static juce::StringArray getAllPresets();
  juce::String getCurrentPreset();
  
  const juce::HashMap<juce::String, int> &getMidiProgramAssignments() { return midiProgramAssignments; }

  void savePresetCategories();
  void loadPresetCategories();
  juce::String getPresetCategory(const juce::String& presetName) const;
  void setPresetCategory(const juce::String& presetName, const juce::String& category);

private:
  void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;
  juce::HashMap<juce::String, int> midiProgramAssignments;
  juce::HashMap<int, juce::String> midiProgramToPreset;

  
  juce::HashMap<juce::String, juce::String> presetCategories; // presetName -> category
                                                              

  juce::AudioProcessorValueTreeState &valueTreeState;
  juce::Value currentPreset;
};
