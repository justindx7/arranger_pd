#include "PresetManager.h"
#include <utility>

const juce::File PresetManager::defaultDirectory{juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("songdata").getChildFile("presets")};
const juce::String PresetManager::extension{"preset"};
const juce::String PresetManager::presetNameProperty{"presetName"};

const juce::File PresetManager::midiAssignFile{juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("songdata")
        .getChildFile("midi_assignments.settings")};

const juce::File PresetManager::presetCategoryFile{
    juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("songdata")
        .getChildFile("preset_categories.settings")
};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState &apvts): valueTreeState(apvts) {
  loadMidiAssignments();
  loadPresetCategories();

  if (!defaultDirectory.exists()) {
    const auto makeDir = defaultDirectory.createDirectory();
    if (makeDir.failed()) {
      DBG("Could not create Directory: " + makeDir.getErrorMessage());
      jassertfalse;
    }
  }

  valueTreeState.state.addListener(this);
  currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
}

void PresetManager::savePreset(const juce::String &presetName) {
  if (presetName.isEmpty())
    return;

  currentPreset.setValue(presetName);
  const auto xml = valueTreeState.copyState().createXml();
  const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);

  if (!xml->writeTo(presetFile)) {
    DBG("Could not create preset: " + presetFile.getFullPathName());
    jassertfalse;
  }
  currentPreset = presetName;
}

void PresetManager::deletePreset(const juce::String &presetName) {
  if (presetName.isEmpty())
    return;

  const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);

  if (!presetFile.existsAsFile()) {
    DBG("Preset file: " + presetFile.getFullPathName() + " does not exist");
    jassertfalse;
    return;
  }
  if (!presetFile.deleteFile()) {
    DBG("Preset file: " + presetFile.getFullPathName() +
        " could not be deleted");
    jassertfalse;
    return;
  }
  currentPreset.setValue("");
  if (presetCategories.contains(presetName)) {
    presetCategories.remove(presetName);
    savePresetCategories();
  }
}

void PresetManager::loadPreset(const juce::String &presetName) {
  if (presetName.isEmpty())
    return;

  const auto presetFile =
      defaultDirectory.getChildFile(presetName + "." + extension);

  if (!presetFile.existsAsFile()) {
    DBG("Preset file: " + presetFile.getFullPathName() + " does not exist");
    jassertfalse;
    return;
  }
  juce::XmlDocument xmlDocument{presetFile};
  const auto newValueTreeState =
      juce::ValueTree::fromXml(*xmlDocument.getDocumentElement());

  for (int i = 0; i < newValueTreeState.getNumChildren(); i++) {
    const auto childToLoad = newValueTreeState.getChild(i);

    // If this is a parameter node (has "id"), update existing node
    if (childToLoad.hasProperty("id")) {
      const auto parameterId = childToLoad.getProperty("id");
      auto parameterTree =
          valueTreeState.state.getChildWithProperty("id", parameterId);

      if (parameterTree.isValid())
        parameterTree.copyPropertiesFrom(childToLoad, nullptr);
    }
    // Otherwise, it's a sample path or other custom node
    else {
      auto type = childToLoad.getType();
      auto stateChild = valueTreeState.state.getChildWithName(type);

      // Remove duplicates if any
      while (stateChild.isValid()) {
        valueTreeState.state.removeChild(stateChild, nullptr);
        stateChild = valueTreeState.state.getChildWithName(type);
      }
// Add a copy of the node from the preset
      valueTreeState.state.appendChild(childToLoad.createCopy(), nullptr);
    }
  }

  currentPreset.setValue(presetName);
}

int PresetManager::loadNextPreset() {
  const auto allPresets = getAllPresets();

  if (allPresets.isEmpty())
    return -1;

  const auto currentIndex = allPresets.indexOf(currentPreset.toString());
  const auto nextIndex =
      currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
  loadPreset(allPresets.getReference(nextIndex));
  return nextIndex;
}

int PresetManager::loadPreviousPreset() {
  const auto allPresets = getAllPresets();

  if (allPresets.isEmpty())
    return -1;

  const auto currentIndex = allPresets.indexOf(currentPreset.toString());
  const auto previousIndex = currentIndex - 1 < 0 ? (allPresets.size() - 1) : currentIndex - 1;
  loadPreset(allPresets.getReference(previousIndex));

  return previousIndex;
}

juce::StringArray PresetManager::getAllPresets() {
  juce::StringArray presets;

  const auto fileArray = defaultDirectory.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*." + extension);
  for (const auto &file : fileArray) {
    presets.add(file.getFileNameWithoutExtension());
  }
  presets.sort(true);
  return presets;
}

juce::String PresetManager::getCurrentPreset() {
  return currentPreset.toString();
}

void PresetManager::valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) {
    currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
}

void PresetManager::assignMidiProgram(const juce::String &presetName, int midiProgram, int midiChannel) {
    removeMidiProgram(presetName);
    
    midiProgramAssignments.insert_or_assign(presetName, std::make_pair(midiProgram, midiChannel));
    midiProgramToPreset.insert_or_assign(std::make_pair(midiProgram, midiChannel), presetName);
    saveMidiAssignments();
}

void PresetManager::removeMidiProgram(const juce::String &presetName) {
    // inserts 0 if presetName doesnt exist but then removes it
    // might be cleaner to use at() and catch if the presetName doesnt exist
    auto pair = midiProgramAssignments[presetName];
    midiProgramAssignments.erase(presetName);
    midiProgramToPreset.erase(pair);
    saveMidiAssignments();
}

juce::String PresetManager::getPresetNameForMidiProgram(int midiProgram, int midiChannel) const {
    std::pair<int, int> key = {midiProgram, midiChannel};

    auto it = midiProgramToPreset.find(key);
    if (it != midiProgramToPreset.end())
        return it->second;
    return juce::String();
}

void PresetManager::saveMidiAssignments() {
  juce::PropertiesFile::Options options;
  options.applicationName = "Arranger";
  options.filenameSuffix = "settings";
  options.folderName = "";

  juce::PropertiesFile props(midiAssignFile, options);
  // Remove all existing keys before saving current assignments
  auto keys = props.getAllProperties().getAllKeys();
  for (const auto& key : keys) {
    props.removeValue(key);
  }

  // Save assignments
  for (auto it = midiProgramAssignments.begin(); it != midiProgramAssignments.end(); ++it) {
    auto pair = it->second;
    // convert pair to string so i can store it with setValue
    juce::String valueStr = juce::String(pair.first) + ":" + juce::String(pair.second);
    props.setValue(it->first, valueStr);
  }

  props.saveIfNeeded();
}

void PresetManager::loadMidiAssignments(){

  juce::PropertiesFile::Options options;
  options.applicationName = "Arranger";
  options.filenameSuffix = "settings";
  options.folderName = "";

  juce::PropertiesFile props(midiAssignFile,options);
  midiProgramAssignments.clear();
  midiProgramToPreset.clear();

  auto keys = props.getAllProperties();
  for (int i = 0; i < keys.size(); ++i) {
    auto key = keys.getAllKeys()[i];
    juce::String valueStr = props.getValue(key, "");
    auto tokens = juce::StringArray::fromTokens(valueStr, ":", "");
    if (tokens.size() == 2) {
        int program = tokens[0].getIntValue();
        int channel = tokens[1].getIntValue();
        midiProgramAssignments.insert_or_assign(key, std::make_pair(program, channel));
        midiProgramToPreset.insert_or_assign(std::make_pair(program, channel), key);
    }
  }
}

void PresetManager::savePresetCategories() {
    juce::PropertiesFile::Options options;
    options.applicationName = "Arranger";
    options.filenameSuffix = "settings";
    options.folderName = "";

    juce::PropertiesFile props(presetCategoryFile, options);
    auto keys = props.getAllProperties().getAllKeys();
    for (const auto& key : keys)
        props.removeValue(key);

    for (auto it = presetCategories.begin(); it != presetCategories.end(); ++it)
        props.setValue(it.getKey(), it.getValue());

    props.saveIfNeeded();
}

void PresetManager::loadPresetCategories() {
    juce::PropertiesFile::Options options;
    options.applicationName = "Arranger";
    options.filenameSuffix = "settings";
    options.folderName = "";

    juce::PropertiesFile props(presetCategoryFile, options);
    presetCategories.clear();

    auto keys = props.getAllProperties();
    for (int i = 0; i < keys.size(); ++i) {
        auto key = keys.getAllKeys()[i];
        auto category = props.getValue(key, "");
        if (category.isNotEmpty())
            presetCategories.set(key, category);
    }
}

void PresetManager::setPresetCategory(const juce::String& presetName, const juce::String& category) {
    presetCategories.set(presetName, category);
    savePresetCategories();
}

juce::String PresetManager::getPresetCategory(const juce::String& presetName) const {
    return presetCategories[presetName];
}
