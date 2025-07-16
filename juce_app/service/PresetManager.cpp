#include "PresetManager.h"

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
  return presets;
}

juce::String PresetManager::getCurrentPreset() {
  return currentPreset.toString();
}

void PresetManager::valueTreeRedirected(juce::ValueTree &treeWhichHasBeenChanged) {
    currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
}

void PresetManager::assignMidiProgram(const juce::String &presetName,int midiProgram) {

  midiProgramAssignments.set(presetName, midiProgram);
  for (auto it = midiProgramAssignments.begin();it != midiProgramAssignments.end(); ++it) {
    midiProgramToPreset.set(it.getValue(), it.getKey());
  }
  saveMidiAssignments();
}

void PresetManager::removeMidiProgram(const juce::String &presetName) {
  midiProgramAssignments.remove(presetName);

  for (auto it = midiProgramAssignments.begin();it != midiProgramAssignments.end(); ++it) {
    midiProgramToPreset.set(it.getValue(), it.getKey());
  }
  saveMidiAssignments();
}

juce::String PresetManager::getPresetNameForMidiProgram(int midiProgram) const {
    return midiProgramToPreset[midiProgram];
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

  for (auto it = midiProgramAssignments.begin(); it != midiProgramAssignments.end(); ++it) {
    props.setValue(it.getKey(), it.getValue());
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
    int midiNum = props.getIntValue(key, -1);

    if (midiNum >= 0) {
      midiProgramAssignments.set(key, midiNum);
      midiProgramToPreset.set(midiNum,key);
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
