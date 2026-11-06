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
- `TurntablePluginEditor.h/.cpp` — hosts `TurntableComponent` at a larger
  640x640 window (was 420x420 — too small to read), plus a Play/Stop
  button.
- `TurntableComponent.h` — enlarged platter/tonearm rendering with real
  metal-plinth styling, plus working 33/45 speed toggle buttons wired to
  the processor's RPM. Tonearm target groove position is still a fixed
  resting spot, not driven by playback progress.
- `InnerGrooveLossProcessor.h` — unchanged from first draft.

Not yet addressed: the procedural vinyl noise engine (dust/scratch/rumble
impulse responses mixed pre-ApexStrip/ApexMaster) described in the
original blueprint, album-art loading, pitch-bend gesture. Confirmed
building and running on macOS Sonoma 14.5 as of the last build check.
