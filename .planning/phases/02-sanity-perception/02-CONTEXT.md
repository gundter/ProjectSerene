# Phase 2: Sanity & Perception - Context

**Gathered:** 2026-01-20
**Status:** Ready for planning

<domain>
## Phase Boundary

Darkness degrades sanity, triggering visual and audio distortion that makes the world feel unreliable. Player standing in darkness loses sanity; standing in light (environmental lights, not flashlight) recovers sanity up to a cap. Lower sanity means increasingly distorted perception through post-process effects and audio changes.

</domain>

<decisions>
## Implementation Decisions

### Light Detection
- Light source proximity-based detection (not ambient sampling)
- ~5-6m radius from light sources counts as "in light"
- Flashlight does NOT provide sanity protection - external lights only
- Binary detection: any stable light source works equally (intensity doesn't matter)
- Windows and moonlight count as protective light sources
- Flickering lights provide NO sanity protection - purely atmospheric
- 1-2 second grace period when leaving light before drain starts
- No visual markers for light zones - player learns naturally through observation
- Entering light triggers: subtle HUD glow + character audio cue (sigh of relief)

### Sanity Pressure
- Baseline drain rate: 60 seconds from 100% to critical (30%) in darkness
- Drain rate scales with difficulty setting
- Linear drain rate (no acceleration at lower levels)
- Events/jump scares can cause instant sanity hits (flat value reductions)
- Light recovery capped at 80% - cannot fully recover in the field
- Recovery rate: 60 seconds to reach cap (slow)
- "Safe rooms" concept noted for future - faster/full recovery possible there
- Sanity capped at minimum 5% - cannot reach 0 (possible future difficulty option for death at 0)
- No passive drain in lit areas - light = completely safe
- Warning icon appears below 30% sanity - fades in/out opacity
- Starting sanity is narrative-dependent (Phase 9 decides)
- Sanity persists across saves; resets to baseline on new level start

### Visual Distortion
- Combination of effects: grain, vignette, chromatic aberration, color shift
- Effects scale gradually from 100% sanity (never feels "normal")
- Constant baseline intensity with random spikes
- Post-process effects only - no geometry distortion
- HUD stays clean and readable - minimal HUD for immersion
- Cold/blue color tint at low sanity
- Accessibility consideration: colorblind options needed
- Visual glitches (static, frame issues) included at low sanity

### Audio Distortion
- Primary audio: heartbeat + breathing
- Heartbeat/breathing become audible below 50% sanity
- Whispers (unintelligible) at low sanity
- World audio muffling only at critical levels (below 20%)

### Claude's Discretion
- Exact distance values for light detection radius
- Grace period exact timing
- Post-process effect intensity curves
- Glitch timing and frequency
- Whisper audio design and timing
- MetaSounds procedural parameter implementation

</decisions>

<specifics>
## Specific Ideas

- "I like the idea of a simple icon that shows up in one of the corners of the HUD and goes in and out of opacity (almost like it's flashing)" for critical sanity warning
- Flashlight as hallucination revealer - noted for Phase 6 (Hallucinations)
- Keep HUD minimal for immersion

</specifics>

<deferred>
## Deferred Ideas

- Flashlight revealing real vs hallucinated enemies - Phase 6 (Hallucinations)
- Enemies destroying/disabling light sources - Phase 7 (AI & Enemies)
- Corrupted areas that drain sanity even in light - Phase 9 (Narrative) or Phase 10 (Level Design)
- Sprinting affecting sanity - future combat/chase phases
- Safe rooms with full sanity recovery - Phase 10 (Level Design)
- Death on 0 sanity for harder difficulties - future difficulty options

</deferred>

---

*Phase: 02-sanity-perception*
*Context gathered: 2026-01-20*
