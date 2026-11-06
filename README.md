# Power Station Studios

Part of the **Virtual Analog Sounds** product line. Power Station Studios is the
central hub — an Analog Patch Bay housed in a "Black Box" UI — that routes any
of the standalone products into ApexMaster for final mastering/output.

This repo is a **monorepo with shared core**: it hosts the hub itself, shared
code referenced by the standalone products, and the packaging/bundling logic
that assembles the Advanced Pack and Enterprise tiers. Each standalone product
(Power Tape, ApexStrip, ApexMaster, Vintage Receiver, Turntable, 8-Track)
continues to ship and sell independently; nothing here changes that.

## Status

Early draft source, not a working build. Landed so far:

- `hub/Source/` — `juce::AudioProcessorGraph` host (`PowerStationStudioHub.h`),
  manual patch-cable UI (`PatchCableComponent.h`, `PatchJackComponent.h`,
  `PatchBayMatrixComponent.h`), and connection helpers plus a single
  hardcoded example preset (`GraphRoutingHelpers.h`, explicitly NOT the
  real AI routing engine)
- `products/turntable/Source/` — platter/tonearm UI and inner-groove-loss
  DSP
- `products/eight-track/Source/` — program-switch engine with solenoid
  click transient

None of this has been compiled, assembled into a JUCE project (no
CMakeLists/Projucer file exists yet), or tested. Each file's header
comment lists what's missing before it's build-ready — see those for
specifics rather than assuming anything here works end-to-end. The AI
auto-routing engine itself is still unscoped; see `docs/ai-routing-design.md`.

## Planned layout

- `hub/` — Power Station itself: patch bay UI, cable/routing engine, the
  "Black Box" open/close interaction, AI-assisted auto-routing (see
  `docs/ai-routing-design.md`, to be written as its own design doc before
  implementation starts).
- `core/` — shared DSP/UI code referenced by hub and standalone products.
- `products/` — per-product references/notes for how each standalone plugin
  (Power Tape, ApexStrip, ApexMaster, Vintage Receiver, Turntable, 8-Track)
  plugs into the hub. Each product's own source stays in its own repo;
  nothing here duplicates that code.
- `packaging/` — tier assembly: Standalone, Advanced Pack (excludes ApexStrip
  and Vintage Receiver), Enterprise (everything + Power Station hub).
- `docs/` — design docs, starting with the AI auto-routing spec.

## Product line (for reference)

**Standalone:** Power Tape, ApexStrip, ApexMaster, Vintage Receiver,
Turntable (planned), 8-Track (planned)

**Advanced Pack:** bundle, excludes ApexStrip and Vintage Receiver

**Enterprise:** everything, plus Power Station (hub) with AI-assisted
auto-routing into ApexMaster
