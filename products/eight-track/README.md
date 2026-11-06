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
- `EightTrackPluginEditor.h/.cpp` — hosts `EightTrackComponent`.
- `EightTrackComponent.h` — real cosmetic design: wood-tone chassis,
  cartridge slot, twin VU meters (L/R, needles driven by random jitter
  while "powered" — cosmetic only, not reading actual signal level),
  4-segment program indicator, transport button row. Power/Program
  buttons are wired; Auto/Eject/Fwd/Pause are drawn and clickable but
  not connected to anything.
- `EightTrackCartridgeEngine.h` — unchanged from first draft.

Not yet implemented: actual audio seek/playback tied to `programOffsets`
(computed but unused), cross-talk bleed between adjacent stereo pairs
(-36 dB per the original spec), tape-wear high-frequency roll-off, and
real (non-cosmetic) VU metering. Confirmed building and running on macOS
Sonoma 14.5 as of the last build check.
