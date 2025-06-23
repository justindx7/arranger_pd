#pragma once

#include <JuceHeader.h>
#include "../service/PresetManager.h"

class MidiHandler {
public:
  void logMidiMessages(const juce::MidiBuffer &midiMessages, PresetManager &manager);
  void setRepaintCallback(std::function<void()> cb) { repaintCallback = std::move(cb); }

private:
  std::function<void()> repaintCallback;
};
