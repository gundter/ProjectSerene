---
phase: 02-sanity-perception
plan: 01
subsystem: gameplay
tags: [GAS, sanity, perception, light-detection, UActorComponent, GameplayTags]

# Dependency graph
requires:
  - phase: 01-core-attributes
    provides: "GAS infrastructure, SereneAttributeSet with Sanity attribute, threshold tag pattern"
provides:
  - "Multi-level sanity threshold tags (50%, 30%, 20%)"
  - "Sanity minimum floor (5%) and regen cap value (80%)"
  - "SanityPerceptionComponent for light-based sanity management"
  - "Timer-based light proximity detection system"
  - "GAS effect application/removal pattern for sanity drain/regen"
affects: [02-02, 02-03, 06-hallucinations, audio-distortion, visual-effects]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "Multi-threshold tag management with UpdateSanityThresholdTags"
    - "ActorComponent with timer-based periodic checks"
    - "Light caching with weak object pointers"
    - "Grace period pattern for state transitions"

key-files:
  created:
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.h
    - Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.cpp
  modified:
    - Source/ProjectSerene/GAS/SereneGameplayTags.h
    - Source/ProjectSerene/GAS/SereneGameplayTags.cpp
    - Source/ProjectSerene/GAS/SereneAttributeSet.h
    - Source/ProjectSerene/GAS/SereneAttributeSet.cpp
    - Source/ProjectSerene/Variant_Horror/HorrorCharacter.h
    - Source/ProjectSerene/Variant_Horror/HorrorCharacter.cpp

key-decisions:
  - "Multi-level sanity tags (50%, 30%, 20%) instead of single threshold"
  - "5% sanity floor - player cannot reach 0 sanity"
  - "80% regen cap enforced via GetSanityRegenCapValue for Gameplay Effect use"
  - "550cm light detection radius (~5-6m) with 1.5s grace period"
  - "Timer-based detection (0.1s interval) instead of Tick for performance"
  - "Actor tags for light filtering (FlickeringLight, ProtectiveLight)"

patterns-established:
  - "Multi-threshold tag management: UpdateSanityThresholdTags called from PostGameplayEffectExecute"
  - "ActorComponent for modular gameplay systems: SanityPerceptionComponent pattern"
  - "Light caching with TArray<TWeakObjectPtr<AActor>> for safe actor references"
  - "Grace period state machine: bInLight + bInGracePeriod + timers"

# Metrics
duration: 12min
completed: 2026-01-20
---

# Phase 02 Plan 01: Sanity Core Foundation Summary

**Light-based sanity system with multi-threshold tags, 5% minimum floor, timer-based proximity detection, and GAS effect management in SanityPerceptionComponent**

## Performance

- **Duration:** 12 min
- **Started:** 2026-01-20T17:00:00Z
- **Completed:** 2026-01-20T17:12:00Z
- **Tasks:** 3
- **Files modified:** 8

## Accomplishments
- Extended sanity threshold tags to three levels (50%, 30%, 20%) for perception/audio effects
- Implemented sanity minimum floor at 5% - player can never reach 0 sanity
- Created SanityPerceptionComponent with timer-based light proximity detection
- Attached component to HorrorCharacter as default subobject
- Ready to receive Blueprint Gameplay Effects for drain/regen in Plan 02

## Task Commits

Each task was committed atomically:

1. **Task 1: Expand Sanity Threshold Tags and AttributeSet** - `7d0307d` (feat)
2. **Task 2: Create SanityPerceptionComponent** - `a664a3a` (feat)
3. **Task 3: Attach Component to HorrorCharacter** - `d3b9d0f` (feat)

## Files Created/Modified

### Created
- `Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.h` - Light detection component header with GAS effect management
- `Source/ProjectSerene/Variant_Horror/Components/SanityPerceptionComponent.cpp` - Implementation: light caching, proximity checks, effect apply/remove

### Modified
- `Source/ProjectSerene/GAS/SereneGameplayTags.h` - Added State_Sanity50, State_Sanity30, State_Sanity20 declarations
- `Source/ProjectSerene/GAS/SereneGameplayTags.cpp` - Added tag definitions with comments
- `Source/ProjectSerene/GAS/SereneAttributeSet.h` - Added SanityRegenCap, SanityMinimumPercent constants, GetSanityRegenCapValue(), UpdateSanityThresholdTags()
- `Source/ProjectSerene/GAS/SereneAttributeSet.cpp` - Modified PreAttributeChange for 5% minimum, added threshold tag management
- `Source/ProjectSerene/Variant_Horror/HorrorCharacter.h` - Added SanityPerceptionComponent member and accessor
- `Source/ProjectSerene/Variant_Horror/HorrorCharacter.cpp` - Create component in constructor

## Decisions Made

1. **Multi-level sanity tags** - Added State_Sanity50, State_Sanity30, State_Sanity20 in addition to legacy State_LowSanity. Enables perception effects at 50%, hallucination intensification at 30%, and audio muffling at 20%.

2. **5% sanity minimum floor** - Enforced in PreAttributeChange with SanityMinimumPercent constant. Player can never reach 0 sanity per CONTEXT.md design.

3. **80% regen cap via helper method** - GetSanityRegenCapValue() returns MaxSanity * 0.8 for Gameplay Effect Execution Calculation use. Allows medication to restore past 80% while light regen is capped.

4. **Timer-based light detection** - 0.1s interval via FTimerHandle instead of Tick for better performance. Sufficient accuracy for gameplay feel.

5. **550cm detection radius** - Approximately 5-6 meters from light sources per CONTEXT.md specification.

6. **1.5s grace period** - Delay before drain starts after leaving light. Prevents jarring sanity spikes at light boundaries.

7. **Actor tags for filtering** - FlickeringLight tag excludes atmospheric lights. ProtectiveLight tag includes non-light actors (windows, moonlight zones).

## Deviations from Plan

None - plan executed exactly as written.

## Issues Encountered

None - all tasks completed successfully.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

- **Ready for Plan 02:** SanityPerceptionComponent has TSubclassOf<UGameplayEffect> properties for SanityDrainEffect and SanityRegenEffect - needs Blueprint Gameplay Effects created in Editor
- **Ready for Plan 03:** Threshold tags available for post-process and audio systems to query
- **Note:** Runtime testing requires Blueprint character derived from HorrorCharacter with GE assets assigned

---
*Phase: 02-sanity-perception*
*Completed: 2026-01-20*
