# 8-Track (in draft)

Standalone instrument — real-science emulation of the 70s 8-track format:
4 concurrent stereo programs (Tracks 1-2/3-4/5-6/7-8), program-change
solenoid click transient, tape-head cross-talk, high-frequency tape
degradation over extended playback.

## Status

First-pass source landed in `Source/`, not yet compiled, wired into a
JUCE project, or tested:

- `EightTrackCartridgeEngine.h` — program switching + solenoid click
  transient only

Not yet implemented: actual audio seek/playback tied to `programOffsets`
(computed but unused), cross-talk bleed between adjacent stereo pairs
(-36 dB per the original spec), and tape-wear high-frequency roll-off.
See the header comment in the file for specifics.
