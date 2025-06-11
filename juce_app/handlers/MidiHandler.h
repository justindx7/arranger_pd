#pragma once

#include <JuceHeader.h>

class MidiHandler {
public:
  void logMidiMessages(const juce::MidiBuffer &midiMessages);
};
