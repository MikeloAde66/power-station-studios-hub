# Turntable (in draft)

Standalone instrument — nostalgic vinyl-capture emulation, not a DJ tool.
Motorized arm, album/center label art, 33/45 speed switching, record-noise
presets.

## Status

First-pass source landed in `Source/`, not yet compiled, wired into a
JUCE project, or tested:

- `TurntableComponent.h` — platter rendering + tonearm animation (UI/visual
  only, 60fps timer-driven)
- `InnerGrooveLossProcessor.h` — DSP for inner-groove high-frequency loss,
  not yet connected to the component above

See the header comments in each file for the specific gaps (RPM switch
not wired to UI, tonearm target position hardcoded, no pitch-bend gesture
yet, DSP/UI progress not synced).

Not yet addressed: the procedural vinyl noise engine (dust/scratch/rumble
impulse responses mixed pre-ApexStrip/ApexMaster) described in the
original blueprint — no code for that yet.
