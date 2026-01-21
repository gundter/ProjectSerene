# Phase 3: Flashlight & Light System - Context

**Gathered:** 2026-01-21
**Status:** Ready for planning

<domain>
## Phase Boundary

Player controls their primary light source — the flashlight — which provides visibility in dark areas but does NOT protect sanity. Environmental lights are the only source of sanity protection. Creates a risk/reward loop between needing visibility (flashlight + battery management) and needing sanity (finding environmental light sources). The flashlight can also reveal hidden details like evidence or messages.

</domain>

<decisions>
## Implementation Decisions

### Flashlight Behavior
- Press F to toggle on/off (not hold-to-use)
- Realistic warm-up: light fades in over ~0.5s when turned on
- Works while sprinting but beam is unstable (bounces/sways more)
- When battery dies: dramatic flicker-out over 1-2 seconds then cuts out completely

### Battery Depletion
- Linear drain rate (predictable, player can plan)
- Flicker starts below 10% battery (late warning, stressful)
- Flicker style: subtle dimming with occasional brief flickers (not aggressive strobe)
- No gradual dim before flicker threshold — Claude's discretion on brightness curve

### Light Cone & Sanity
- **CRITICAL:** Flashlight does NOT protect sanity — visibility tool only
- Only environmental lights (point lights, spot lights) provide sanity protection
- Fixed wide beam (broader illumination, less intense center)
- Flashlight reveals hidden details (UV-style: hidden messages, blood stains, evidence)

### Visual/Audio Feedback
- Mechanical click sound on toggle (satisfying, tactile)
- Dusty/atmospheric beam with visible light rays (volumetric feel)
- Minimal diegetic HUD — no flashlight indicator, rely on visual/audio cues
- Enemies will react to flashlight beam (noted for Phase 7 AI integration)

### Claude's Discretion
- Exact fade-in timing for warm-up (around 0.5s)
- Brightness curve as battery depletes (before 10% threshold)
- Specific flicker pattern implementation
- Volumetric beam intensity and dust particle density
- Sprint sway amount and pattern

</decisions>

<specifics>
## Specific Ideas

- Flashlight creates interesting tension: you need it to see, but it doesn't help sanity AND it attracts enemies
- The "visibility only, no sanity protection" design means players can't just keep flashlight on to feel safe
- Hidden detail reveal (UV-style) supports investigation gameplay in Phase 5
- Diegetic feedback (no HUD indicator) matches horror immersion goals

</specifics>

<deferred>
## Deferred Ideas

- Enemy AI reacting to flashlight beam — Phase 7 (AI & Enemies)
- What specific hidden details are revealed — Phase 5 (Investigation System)

</deferred>

---

*Phase: 03-flashlight-light-system*
*Context gathered: 2026-01-21*
