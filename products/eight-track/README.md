# 8-Track (in draft)

Standalone instrument — real-science emulation of the 70s 8-track format:
4 concurrent stereo programs (Tracks 1-2/3-4/5-6/7-8), program-change
solenoid click transient, tape-head cross-talk, high-frequency tape
degradation over extended playback.

## Status

Now a real (untested) JUCE plugin target via the root `CMakeLists.txt`:

- `EightTrackPluginProcessor.h/.cpp` — `juce::AudioProcessor` wrapping
  `EightTrackCartridgeEngine`. Passes audio through plus the solenoid
  click transient on program switch; does not yet play distinct audio
  per program.
- `EightTrackPluginEditor.h/.cpp` — **placeholder UI only**: a button and
  a "Program N / 4" label. No cartridge visual design exists yet (unlike
  Turntable, which has a real component).
- `EightTrackCartridgeEngine.h` — unchanged from first draft.

Not yet implemented: actual audio seek/playback tied to `programOffsets`
(computed but unused), cross-talk bleed between adjacent stereo pairs
(-36 dB per the original spec), and tape-wear high-frequency roll-off.
Not built or run locally; needs verification in Xcode.
