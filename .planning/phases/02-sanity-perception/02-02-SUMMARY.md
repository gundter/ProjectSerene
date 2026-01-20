---
phase: 02-sanity-perception
plan: 02
subsystem: gameplay
tags: [GAS, sanity, GameplayEffects, post-process, visual-distortion, perception]

# Dependency graph
requires:
  - phase: 02-01
    provides: "SanityPerceptionComponent, sanity threshold tags, 5% minimum floor"
provides:
  - "GE_SanityDrain specification (-0.1167 per 0.1s, 100->30% in 60s)"
  - "GE_SanityRegen specification (+0.0833 per 0.1s, 30->80% in 60s)"
  - "Visual distortion system (vignette, grain, chromatic aberration, saturation)"
  - "Post-process settings driven by sanity percentage"
  - "80% regen cap enforcement via CheckRegenCap"
affects: [02-03, visual-effects, audio-distortion, hallucinations]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "Camera FPostProcessSettings modification at runtime"
    - "GAS attribute change delegate for reactive visual effects"
    - "Override flags pattern for post-process properties"

key-files:
  created: []
  modified:
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.h
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.cpp
    - Content/Variant_Horror/Blueprints/GameplayEffects_Setup.md

key-decisions:
  - "Linear interpolation for visual effects (tunable in playtesting)"
  - "80% regen cap enforced in component, not in Gameplay Effect"
  - "Post-process on camera rather than volume for player-centric effects"
  - "Blueprint Gameplay Effects for tunability without recompilation"

patterns-established:
  - "Camera post-process modification: bOverride_* flags + value setting"
  - "Sanity-driven visual effects via OnSanityChanged delegate"
  - "Component-level regen cap enforcement pattern"

# Metrics
duration: 8min
completed: 2026-01-20
---

# Phase 02 Plan 02: GAS Effects & Visual Distortion Summary

**Sanity Gameplay Effect specifications and real-time post-process visual distortion driven by sanity percentage**

## Performance

- **Duration:** 8 min
- **Started:** 2026-01-20T17:00:00Z
- **Completed:** 2026-01-20T17:08:00Z
- **Tasks:** 3
- **Files modified:** 3

## Accomplishments
- Added Gameplay Effect specifications for GE_SanityDrain and GE_SanityRegen to documentation
- Implemented visual distortion system with vignette, film grain, chromatic aberration, and color saturation
- Created sanity change listener that updates visual effects in real-time
- Implemented 80% regen cap enforcement in CheckRegenCap method
- Configured post-process override flags for camera-based effects
- Updated BP_HorrorCharacter configuration instructions

## Task Commits

Each task was committed atomically:

1. **Task 2: Implement Visual Distortion System** - `04d587f` (feat)
2. **Task 1: Create Blueprint Gameplay Effects** - `f7427bc` (docs)
3. **Task 3: BP_HorrorCharacter Configuration** - included in `f7427bc` (docs)

Note: Task order in execution was 2, 1, 3 due to C++ implementation being prerequisite for accurate documentation.

## Files Created/Modified

### Modified
- `Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.h` - Added visual distortion configuration properties and methods
- `Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.cpp` - Implemented visual distortion system with camera post-process
- `Content/Variant_Horror/Blueprints/GameplayEffects_Setup.md` - Added GE_SanityDrain and GE_SanityRegen specifications, updated BP_HorrorCharacter config, added sanity testing checklist

## Decisions Made

1. **Linear interpolation for visual effects** - Used FMath::Lerp for smooth scaling between min/max values. Can be changed to curves in playtesting if non-linear response feels better.

2. **Component-level regen cap** - The 80% sanity regen cap is enforced by CheckRegenCap() in SanityPerceptionComponent rather than a custom Gameplay Effect Execution Calculation. Simpler implementation that matches Phase 1 patterns.

3. **Camera post-process over volumes** - Visual distortion uses UCameraComponent::PostProcessSettings directly rather than PostProcessVolumes. This ensures effects follow the player automatically without world placement.

4. **Blueprint Gameplay Effects** - GE_SanityDrain and GE_SanityRegen are documented as Blueprint assets rather than C++ classes. This allows designers to tune drain/regen rates without recompilation.

5. **Visual effect defaults** - Set conservative defaults (0.2-0.8 vignette, 0.5 max grain, 3.0 max chromatic aberration, 0.6 min saturation) that can be tuned in Blueprint.

## Deviations from Plan

None - plan executed as written.

## Issues Encountered

None - all tasks completed successfully.

## User Setup Required

The following Blueprint assets must be created in Unreal Editor:

### GE_SanityDrain
- Location: `Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityDrain`
- Duration Policy: Infinite
- Period: 0.1 seconds
- Modifier: SereneAttributeSet.Sanity, Add, -0.1167

### GE_SanityRegen
- Location: `Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/GE_SanityRegen`
- Duration Policy: Infinite
- Period: 0.1 seconds
- Modifier: SereneAttributeSet.Sanity, Add, +0.0833

### BP_HorrorCharacter Configuration
- Expand SanityPerceptionComponent in Class Defaults
- Set `Sanity Drain Effect` = GE_SanityDrain
- Set `Sanity Regen Effect` = GE_SanityRegen

Detailed instructions in: `Content/Variant_Horror/Blueprints/GameplayEffects_Setup.md`

## Next Phase Readiness

- **Ready for Plan 03:** Visual distortion implemented; sanity threshold tags available for audio effects
- **Ready for testing:** Once Blueprint Gameplay Effects are created in Editor
- **Note:** Runtime visual effects will only appear after sanity changes occur - requires light/darkness exposure to trigger

---
*Phase: 02-sanity-perception*
*Completed: 2026-01-20*
