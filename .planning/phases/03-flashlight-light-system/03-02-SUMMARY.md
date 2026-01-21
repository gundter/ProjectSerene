---
phase: 03
plan: 02
subsystem: flashlight
tags: [flashlight, timeline, perlin-noise, flicker, sprint-sway, state-machine]
dependencies:
  requires:
    - phase: 03-01
      provides: FlashlightComponent with Off/On states and GAS battery drain
  provides:
    - Extended flashlight state machine (WarmingUp, Flickering, DyingOut)
    - Timeline-based warm-up and death sequences
    - Perlin noise procedural flicker at low battery
    - Sprint sway via rotation offset
  affects: [03-03]
tech-stack:
  added: []
  patterns: [timeline-component-pattern, perlin-noise-flicker, sprint-sway-rotation]
key-files:
  created: []
  modified:
    - Source/ProjectSerene/Variant_Horror/Components/FlashlightComponent.h
    - Source/ProjectSerene/Variant_Horror/Components/FlashlightComponent.cpp
decisions:
  - key: "timeline-for-curves"
    choice: "UTimelineComponent for warm-up and death sequences"
    rationale: "Standard UE pattern for curve-driven animations, cleaner than manual lerp"
  - key: "perlin-noise-flicker"
    choice: "FMath::PerlinNoise1D for procedural flicker"
    rationale: "Organic variation without visible patterns, horror-appropriate"
  - key: "sprint-sway-tick"
    choice: "TickComponent for sprint sway, not timer"
    rationale: "Needs every-frame smoothness for rotation interpolation"
  - key: "flicker-timer-20hz"
    choice: "Timer at 0.05s (20Hz) for flicker updates"
    rationale: "Balance between visual smoothness and performance"
metrics:
  duration: "~4 minutes"
  completed: "2026-01-21"
---

# Phase 03 Plan 02: Battery States & Flicker Summary

**Timeline-driven warm-up/death sequences with Perlin noise flicker at 10% battery and sinusoidal sprint sway**

## Performance

- **Duration:** ~4 min
- **Started:** 2026-01-21T22:19:58Z
- **Completed:** 2026-01-21T22:24:20Z
- **Tasks:** 2
- **Files modified:** 2

## Accomplishments

- Extended EFlashlightState enum with WarmingUp, Flickering, DyingOut states
- Implemented UTimelineComponent integration for curve-driven warm-up and death
- Added Perlin noise-based procedural flicker at 10% battery threshold
- Implemented sprint sway via sinusoidal rotation offset checking State.Sprinting tag
- Clean state transition system with proper cleanup

## Task Commits

Each task was committed atomically:

1. **Task 1: Extend State Machine and Add Timeline-Based Warm-up/Death** - `dced187` (feat)
2. **Task 2: Document Curve Asset Creation and Final Configuration** - Documentation only (included in summary)

## Files Modified

| File | Lines Changed | Description |
|------|---------------|-------------|
| FlashlightComponent.h | +85 | Extended enum, added curves, flicker, sway properties |
| FlashlightComponent.cpp | +418/-84 | Full implementation of timelines, flicker, sway |

## Key Implementation Details

### State Machine

Five states with clean transitions:
- **Off**: Light disabled, no drain
- **WarmingUp**: Fading in via curve, tag added, no drain yet
- **On**: Full brightness, draining battery
- **Flickering**: Battery <10%, Perlin noise flicker, continues drain
- **DyingOut**: Battery depleted, dramatic flicker-death, tag removed

### Timeline Integration

```cpp
void UFlashlightComponent::SetupTimelines()
{
    // Warm-up timeline binds to OnWarmupTick/OnWarmupFinished
    // Death timeline binds to OnDeathTick/OnDeathFinished
    // Both created dynamically in BeginPlay if curves assigned
}
```

### Perlin Noise Flicker

```cpp
const float Noise = FMath::PerlinNoise1D(FlickerTime * FlickerFrequency);
float FlickerIntensity = FMath::Lerp(FlickerMinIntensity, 1.0f, (Noise + 1.0f) * 0.5f);

// 2% chance of brief full flicker per tick
if (FMath::FRand() < 0.02f) FlickerIntensity = 0.1f;
```

### Sprint Sway

```cpp
// In TickComponent when sprinting:
SwayTime += DeltaTime;
const float SwayX = FMath::Sin(SwayTime * 8.0f) * MaxSwayPitch;  // 3 degrees
const float SwayY = FMath::Sin(SwayTime * 6.0f) * MaxSwayYaw;    // 2 degrees
SpotLight->SetRelativeRotation(BaseRotation + FRotator(SwayX, SwayY, 0.0f));
```

## Decisions Made

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Timeline vs manual lerp | UTimelineComponent | Standard UE pattern, automatic curve support |
| Flicker algorithm | Perlin noise | Organic variation without visible patterns |
| Flicker update rate | 20Hz timer | Balance between smoothness and performance |
| Sprint sway method | TickComponent | Needs every-frame smoothness for rotation |
| IsActive rename | IsFlashlightActive | Conflict with UActorComponent::IsActive |

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Renamed IsActive to IsFlashlightActive**
- **Found during:** Task 1 (Initial compile)
- **Issue:** `IsActive` is already a UFUNCTION in UActorComponent base class
- **Fix:** Renamed to `IsFlashlightActive()` to avoid conflict
- **Files modified:** FlashlightComponent.h
- **Verification:** Build succeeds
- **Committed in:** dced187

---

**Total deviations:** 1 auto-fixed (1 blocking)
**Impact on plan:** Minor naming adjustment for UE compliance. No scope creep.

## Issues Encountered

None - implementation followed plan specifications.

## Blueprint Configuration Required

After C++ compiles, the following Blueprint configuration is required:

### 1. Create C_FlashlightWarmup Float Curve

**Location:** Content/Variant_Horror/Blueprints/Gameplay/Curves/

1. Right-click in Content Browser > Miscellaneous > Curve > CurveFloat
2. Save as: `C_FlashlightWarmup`
3. Open the curve editor
4. Add key at Time=0, Value=0
5. Add key at Time=0.5, Value=1.0
6. Set interpolation to "Auto" for smooth ease-in curve
7. Total duration: 0.5 seconds

### 2. Create C_FlashlightDeath Float Curve

**Location:** Content/Variant_Horror/Blueprints/Gameplay/Curves/

1. Create CurveFloat: `C_FlashlightDeath`
2. Add key at Time=0, Value=1.0
3. Add key at Time=1.5, Value=0.0
4. Use "Auto" interpolation for dramatic decay
5. Total duration: 1.5 seconds

### 3. Configure BP_HorrorCharacter FlashlightComponent

**Location:** Content/Variant_Horror/Blueprints/BP_HorrorCharacter

1. Open BP_HorrorCharacter
2. Select FlashlightComponent
3. Set **Warmup Curve:** C_FlashlightWarmup
4. Set **Death Curve:** C_FlashlightDeath
5. Verify default flicker settings:
   - Flicker Threshold: 0.10 (10%)
   - Flicker Frequency: 8.0
   - Flicker Min Intensity: 0.3
   - Max Sway Pitch: 3.0
   - Max Sway Yaw: 2.0
   - Base Intensity: 5000.0 (or match existing SpotLight intensity)
6. Compile and Save

### 4. Verification Checklist

1. Turn on flashlight - should fade in over ~0.5s
2. Let battery drain to <10% - should start subtle flickering
3. Let battery hit 0 - should have dramatic flicker-death over ~1.5s
4. Sprint with flashlight on - beam should sway/bounce
5. Stop sprinting - beam should smoothly return to center
6. Try toggling during warm-up - should ignore (can only turn off from On/Flickering)

## Next Phase Readiness

**Plan 03 (Hidden Detail Reveal via UV Mode)** can proceed:

- FlashlightComponent has stable state machine
- Light intensity control via SetLightIntensity() available
- Can add UV mode state or modify existing states for reveal mechanic

**Phase 3 overall status:**
- Plan 01: Core toggle with battery drain - COMPLETE
- Plan 02: Battery states and flicker - COMPLETE
- Plan 03: Hidden detail reveal - READY

---
*Phase: 03-flashlight-light-system*
*Completed: 2026-01-21*
