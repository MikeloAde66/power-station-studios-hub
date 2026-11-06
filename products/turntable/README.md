# Turntable (in draft)

Standalone instrument — nostalgic vinyl-capture emulation, not a DJ tool.
Motorized arm, album/center label art, 33/45 speed switching, record-noise
presets.

## Status

Now a real (untested) JUCE plugin target via the root `CMakeLists.txt`:

- `TurntablePluginProcessor.h/.cpp` — `juce::AudioProcessor` wrapping
  `InnerGrooveLossProcessor`. Playback `progress` is currently a
  placeholder ramp (~3 min sweep), not real transport position — there's
  no audio file loading/playback source yet.
- `TurntablePluginEditor.h/.cpp` — hosts `TurntableComponent` (platter +
  tonearm) with a plain Play/Stop button. No 33/45 switch, no album-art
  loading, no noise-preset controls.
- `TurntableComponent.h` — platter rendering + tonearm animation
  (unchanged from first draft; RPM still not wired to a UI control,
  tonearm target position still hardcoded).
- `InnerGrooveLossProcessor.h` — unchanged from first draft.

Not yet addressed: the procedural vinyl noise engine (dust/scratch/rumble
impulse responses mixed pre-ApexStrip/ApexMaster) described in the
original blueprint — no code for that yet. Not built or run locally;
needs verification in Xcode.
