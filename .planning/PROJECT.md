# Project Serene

## What This Is

A story-driven psychological FPS horror game built in Unreal Engine 5.7. Players control a female police detective with PTSD who returns to work after surviving capture and torture by a serial killer. Her new case reveals a cult continuing the killer's work, and her trauma causes hallucinations that blur the line between real threats and paranoid visions.

## Core Value

The player experiences unreliable perception — they can never fully trust what they see, creating genuine paranoia and tension. The horror comes from within the protagonist, not just external threats.

## Requirements

### Validated

- ✓ First-person character controller — existing (UE5.7 FPS Horror template)
- ✓ Basic horror game framework — existing (HorrorCharacter, HorrorGameMode, HorrorUI)

### Active

- [ ] Sanity system affecting perception (light/dark, meds, story events)
- [ ] Hallucination spawning at low sanity thresholds
- [ ] Visual tells distinguishing hallucinations from real enemies (clarity tied to sanity)
- [ ] Combat system with pistol and improvised melee weapons
- [ ] Trigger discipline reward (not punishing hallucination kills, rewarding restraint on fakes)
- [ ] Investigation mechanics (examining crime scenes, gathering evidence)
- [ ] Crime scene environment(s) for demo
- [ ] Detective's apartment as home base
- [ ] Scripted paranoia/horror events tied to story beats
- [ ] Core narrative setup (detective backstory, new case introduction)

### Out of Scope

- Full city open exploration — demo focuses on discrete locations
- Multiple branching endings — demo has linear narrative
- Side investigations — demo focuses on main case slice
- Full cult network investigation — demo introduces concept only
- Multiplayer — single-player experience

## Context

**Starting point:** Unreal Engine 5.7 First Person Horror template variant. Existing codebase includes HorrorCharacter, HorrorGameMode, HorrorPlayerController, HorrorUI, and custom camera management.

**Protagonist:** Female detective, face scarred from torture. Captured by serial killer years ago, escaped when killer died in struggle. Cleared to return to work, lands case with similar MO. PTSD causes hallucinations under stress.

**Antagonist:** Cult followers continuing the dead killer's work. Real human enemies mixed with hallucinated threats.

**Sanity mechanics:**
- Darkness decreases sanity
- Light slowly restores sanity (capped)
- Medication provides sanity boost (mild or full restore)
- Story events can force sanity changes

**Demo goal:** One complete investigation slice demonstrating core mechanics and tone. If sanity system proves too complex, paranoia can be conveyed through scripted events instead.

## Constraints

- **Engine**: Unreal Engine 5.7 — project already initialized with this version
- **Template**: First Person Horror variant — build on existing framework, don't fight it
- **Scope**: Demo/vertical slice — one investigation, not full game
- **Platform**: PC primary (standard FPS controls)

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Sanity affects hallucination tell visibility | Rewards good sanity management with clearer combat | — Pending |
| Reward trigger discipline, not punish hallucination kills | Punishing players for defeating enemies feels bad | — Pending |
| Scripted paranoia as fallback if sanity system too complex | Demo scope management | — Pending |
| Grounded horror with cult, no supernatural | Keeps psychological horror authentic to PTSD narrative | — Pending |

---
*Last updated: 2026-01-19 after initialization*
