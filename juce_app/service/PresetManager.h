#pragma once

#include <JuceHeader.h>
#include <unordered_map>

namespace std {
    template <>
    struct hash<std::pair<int, int>> {
        std::size_t operator()(const std::pair<int, int>& p) const noexcept {
            // Combines the hashes of the two integers
            return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
        }
    };
}

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

  void assignMidiProgram(const juce::String &presetName, int midiProgram,int midiChannel);

  void removeMidiProgram(const juce::String &presetName);
  juce::String getPresetNameForMidiProgram(int midiProgram, int midiChannel) const;
  
  void saveMidiAssignments();
  void loadMidiAssignments();

  static juce::StringArray getAllPresets();
  juce::String getCurrentPreset();
  
  const std::unordered_map<juce::String, std::pair<int,int>> &getMidiProgramAssignments() { return midiProgramAssignments; }

  void savePresetCategories();
  void loadPresetCategories();
  juce::String getPresetCategory(const juce::String& presetName) const;
  void setPresetCategory(const juce::String& presetName, const juce::String& category);

private:
  void valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) override;

  std::unordered_map<juce::String, std::pair<int,int> > midiProgramAssignments;
  std::unordered_map<std::pair<int,int>, juce::String> midiProgramToPreset;

  
  juce::HashMap<juce::String, juce::String> presetCategories; // presetName -> category
                                                              

  juce::AudioProcessorValueTreeState &valueTreeState;
  juce::Value currentPreset;
};
