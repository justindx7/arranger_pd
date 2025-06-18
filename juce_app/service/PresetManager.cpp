#include "PresetManager.h"

const juce::File PresetManager::defaultDirectory{juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("songdata").getChildFile("presets")};
const juce::String PresetManager::extension{"preset"};
const juce::String PresetManager::presetNameProperty{"presetName"};

PresetManager::PresetManager(juce::AudioProcessorValueTreeState &apvts): valueTreeState(apvts) {

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
