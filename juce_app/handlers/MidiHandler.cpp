#include "MidiHandler.h"

  void MidiHandler::logMidiMessages(const juce::MidiBuffer &midiMessages, const PresetManager &manager) {
    for (const auto metadata : midiMessages) {
      const auto msg = metadata.getMessage();
      const auto samplePosition = metadata.samplePosition;

      if (msg.isProgramChange()) {
        int programNum = msg.getProgramChangeNumber();
        juce::String presetName = manager.getPresetNameForMidiProgram(programNum);

        DBG("Program Change: Channel "
            << msg.getChannel() << " Program " << programNum << " -> Preset: "
            << (presetName.isNotEmpty() ? presetName : "<none>")
            << " at sample " << samplePosition);

      }else if (msg.isNoteOn()) {
          DBG("Note On:  Channel " << msg.getChannel() << " Note "
                                   << msg.getNoteNumber() << " Velocity "
                                   << msg.getVelocity() << " at sample "
                                   << samplePosition);
        } else if (msg.isNoteOff()) {
        DBG("Note Off: Channel " << msg.getChannel() << " Note "
                                 << msg.getNoteNumber() << " at sample "
                                 << samplePosition);
      } else if (msg.isController()) {
        DBG("Controller: Channel " << msg.getChannel() << " Controller "
                                   << msg.getControllerNumber() << " Value "
                                   << msg.getControllerValue() << " at sample "
                                   << samplePosition);
      } else if (msg.isPitchWheel()) {
        DBG("Pitch Wheel: Channel " << msg.getChannel() << " Value "
                                    << msg.getPitchWheelValue() << " at sample "
                                    << samplePosition);
      } else {
        DBG("Other MIDI Message: " << msg.getDescription() << " at sample "
                                   << samplePosition);
      }
      }
  }
