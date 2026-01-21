---
phase: 02-sanity-perception
plan: 03
subsystem: gameplay
tags: [GAS, sanity, audio, MetaSound, HUD, warning-icon, light-detection, occlusion]

# Dependency graph
requires:
  - phase: 02-01
    provides: "SanityPerceptionComponent, sanity threshold tags"
  - phase: 02-02
    provides: "Visual distortion system, GE specifications"
provides:
  - "Audio distortion system (heartbeat, whispers, muffling via MetaSound parameters)"
  - "HUD sanity warning icon with edge-detection optimization"
  - "Robust light detection with cone checks and occlusion tracing"
  - "Complete Phase 2 sanity perception system"
affects: [hallucinations, flashlight-system, level-design]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "Light cone angle validation for spot lights"
    - "Line trace occlusion for light blocking"
    - "Edge detection for UI state change events"
    - "TActorIterator for component-based actor search"

key-files:
  created:
    - Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityDrain.uasset
    - Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityRegen.uasset
  modified:
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.h
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.cpp
    - Source/ProjectSerene/Variant_Horror/UI/HorrorUI.h
    - Source/ProjectSerene/Variant_Horror/UI/HorrorUI.cpp
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.h
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp
    - Content/Variant_Horror/Blueprints/BP_HorrorCharacter.uasset
    - Content/Variant_Horror/UI/UI_Horror.uasset

key-decisions:
  - "Light detection uses component search (UPointLightComponent/USpotLightComponent) not actor classes"
  - "Player's own actor excluded from light cache (flashlight doesn't protect sanity)"
  - "Spot light cone validation using OuterConeAngle"
  - "Line trace occlusion enabled by default (bCheckLightOcclusion)"
  - "Sanity warning uses edge detection (only fires on threshold crossing)"

patterns-established:
  - "IsIlluminatedByLight pattern: distance + cone + occlusion validation"
  - "Edge detection for UI events: track previous state, only fire on change"
  - "TActorIterator with component filtering for flexible actor discovery"

# Metrics
duration: ~45min (including bug fixes and verification)
completed: 2026-01-21
---

# Phase 02 Plan 03: Audio Distortion & HUD Warning Summary

**Complete Phase 2 sanity perception with audio feedback, HUD warning, and robust light detection**

## Performance

- **Duration:** ~45 min (extended due to bug fixes during verification)
- **Started:** 2026-01-20
- **Completed:** 2026-01-21
- **Tasks:** 3 (plus bug fixes)
- **Files modified:** 8

## Accomplishments

- Implemented audio distortion system with MetaSound parameter control (HeartbeatIntensity, WhisperIntensity, MuffleAmount)
- Added HUD sanity warning icon support with BlueprintImplementableEvent
- Optimized warning to use edge detection (only fires when crossing 30% threshold)
- Fixed critical bug: player spawning in darkness now correctly triggers sanity drain
- Upgraded light detection from actor-class search to component-based search
- Added spot light cone angle validation
- Added line trace occlusion checking (respects walls, floors, ceilings)
- Excluded player's own actor from light cache (flashlight doesn't provide sanity protection)
- User created GE_SanityDrain and GE_SanityRegen Blueprint assets
- User configured BP_HorrorCharacter with sanity effect references
- User implemented warning icon in UI_Horror widget Blueprint

## Task Commits

Each task was committed atomically:

1. **Task 1: Audio Distortion System** - `8788500` (feat)
2. **Task 2: HUD Warning Icon** - `d413e14` (feat)
3. **Bug Fix: Light Detection Improvements** - `5da99a2` (fix)
4. **User Blueprint Setup** - `bf24b18` (feat)
5. **Bug Fix: Warning Edge Detection** - `5c02289` (fix)

## Files Created/Modified

### Created (User in Editor)
- `Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityDrain.uasset` - Sanity drain effect
- `Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityRegen.uasset` - Sanity regen effect

### Modified
- `SanityPerceptionComponent.h/.cpp` - Audio distortion, improved light detection with cone/occlusion
- `HorrorUI.h/.cpp` - UpdateSanityWarning and BP_SanityWarningUpdated event
- `HorrorPlayerController.h/.cpp` - Sanity warning edge detection
- `BP_HorrorCharacter.uasset` - Sanity effect references configured
- `UI_Horror.uasset` - Warning icon widget implementation

## Decisions Made

1. **Component-based light detection** - Search for UPointLightComponent/USpotLightComponent rather than ALight actor classes. Catches all light types in UE5 regardless of how they're placed.

2. **Owner exclusion** - Player's own actor is excluded from light cache, ensuring the flashlight attached to the character doesn't provide sanity protection.

3. **Spot light cone validation** - Uses OuterConeAngle to determine if player is within the light's actual illumination cone, not just proximity.

4. **Line trace occlusion** - Enabled by default via bCheckLightOcclusion. Prevents lights on other floors or behind walls from affecting player.

5. **Edge detection for warnings** - UpdateSanityWarning only fires when crossing the 30% threshold, not on every sanity change. Prevents Blueprint animations from restarting constantly.

## Bug Fixes During Verification

| Bug | Cause | Fix |
|-----|-------|-----|
| Sanity not draining in darkness | Initial state handling missing | Added else branch in CheckLightProximity for spawn-in-darkness case |
| SpotLights not detected | Only searched for ALight actors | Changed to search for light components |
| Flashlight counted as protection | Owner not excluded | Skip owner actor in CacheLightActors |
| Light through floors | No occlusion checking | Added IsIlluminatedByLight with line trace |
| Warning animation restarting | Event fired on every sanity change | Added edge detection with bSanityWarningActive |

## Verification Results

| Feature | Status |
|---------|--------|
| Sanity Drain | Verified - drains in darkness |
| Sanity Regen | Verified - recovers in light, caps at 80% |
| Light Cone Detection | Verified - only lights actually illuminating player count |
| Light Occlusion | Verified - walls/floors block light detection |
| Visual Distortion | Verified - may need tuning |
| Audio Distortion | Pending - needs MetaSound asset creation |
| HUD Warning Icon | Verified - appears below 30%, animation works |

## Pending Items

- **Audio:** MetaSound asset (MS_SanityDistortion) needs to be created with HeartbeatIntensity, WhisperIntensity, MuffleAmount parameters
- **Visual tuning:** Distortion values may need adjustment during playtesting

## Phase 2 Complete

All Phase 2 success criteria met:
- [x] SANI-01: Darkness drains sanity
- [x] SANI-02: Light restores sanity (capped at 80%)
- [x] SANI-03: Threshold tags trigger at 50%, 30%, 20%
- [x] SANI-04: Post-process effects scale with sanity
- [x] SANI-05: Audio distortion system implemented (MetaSound pending)
- [x] HUD warning icon functional

## Next Phase Readiness

- **Ready for Phase 3:** Flashlight & Light System
- **Sanity system provides:** Light detection infrastructure, sanity attributes, threshold tags
- **Note:** Flashlight implementation can leverage existing light component detection

---
*Phase: 02-sanity-perception*
*Completed: 2026-01-21*
