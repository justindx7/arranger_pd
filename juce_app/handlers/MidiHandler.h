#pragma once

#include <JuceHeader.h>
#include "../service/PresetManager.h"

class MidiHandler {
public:
  void logMidiMessages(const juce::MidiBuffer &midiMessages, PresetManager &manager);
};
