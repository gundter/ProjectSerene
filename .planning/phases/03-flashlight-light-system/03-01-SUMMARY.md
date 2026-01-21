---
phase: 03
plan: 01
subsystem: flashlight
tags: [flashlight, GAS, battery, input, components]
dependencies:
  requires: [01-01, 01-02, 01-03]
  provides: [FlashlightComponent, State.FlashlightOn, toggle-input]
  affects: [03-02, 03-03]
tech-stack:
  added: []
  patterns: [component-delegation, GAS-effect-handles, input-action-binding]
key-files:
  created:
    - Source/ProjectSerene/Variant_Horror/Components/FlashlightComponent.h
    - Source/ProjectSerene/Variant_Horror/Components/FlashlightComponent.cpp
  modified:
    - Source/ProjectSerene/Variant_Horror/HorrorCharacter.h
    - Source/ProjectSerene/Variant_Horror/HorrorCharacter.cpp
    - Source/ProjectSerene/GAS/SereneGameplayTags.h
    - Source/ProjectSerene/GAS/SereneGameplayTags.cpp
decisions:
  - key: "component-owns-behavior"
    choice: "FlashlightComponent controls SpotLight via Initialize()"
    rationale: "Separation of concerns - light attached to camera, behavior in component"
  - key: "battery-depletion-auto-off"
    choice: "Automatic shutoff via OnBatteryChanged delegate"
    rationale: "GAS-native approach, matches stamina pattern from Phase 1"
metrics:
  duration: "~4 minutes"
  completed: "2026-01-21"
---

# Phase 03 Plan 01: Core Flashlight Toggle Summary

FlashlightComponent with Off/On state machine and GAS battery drain integration

## One-liner

FlashlightComponent with GAS battery drain, toggle input binding, and State.FlashlightOn tag management.

## What Was Built

### FlashlightComponent (New)

Created `UFlashlightComponent` ActorComponent following the established SanityPerceptionComponent pattern:

- **State Machine**: Off/On enum (extensible for WarmingUp/Flickering/DyingOut in Plan 02)
- **GAS Integration**:
  - Binds to Battery attribute changes via `GetGameplayAttributeValueChangeDelegate`
  - Applies/removes `BatteryDrainEffect` infinite Gameplay Effect
  - Auto-turns off when Battery reaches 0
- **Gameplay Tag**: Adds/removes `State.FlashlightOn` tag on state transitions
- **Audio**: Support for toggle on/off sounds via `ToggleOnSound`/`ToggleOffSound`
- **Light Control**: Controls existing SpotLight visibility via Initialize()

### HorrorCharacter Integration

- Creates FlashlightComponent in constructor
- Passes SpotLight reference via Initialize()
- Binds `ToggleFlashlightAction` input to `ToggleFlashlight()` method
- Added `GetFlashlightComponent()` accessor

### Gameplay Tag

- Added `State.FlashlightOn` to SereneGameplayTags (declaration and definition)
- Documented in tag hierarchy comment

## Commits

| Commit | Description |
|--------|-------------|
| 9456cd6 | feat(03-01): create FlashlightComponent with GAS battery integration |
| 1ff3cfb | feat(03-01): integrate FlashlightComponent into HorrorCharacter |

## Decisions Made

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Light ownership | SpotLight stays on Character, Component controls it | Camera attachment requires Character ownership; behavior separation is cleaner |
| Battery monitoring | GAS delegate `OnBatteryChanged` | Matches established pattern from stamina system |
| Auto-off on depletion | Via attribute delegate, not manual check | GAS-native, no tick required |
| State machine simplicity | Off/On only in Plan 01 | Flicker/dying states deferred to Plan 02 for battery thresholds |

## Deviations from Plan

None - plan executed exactly as written.

## Blueprint Configuration Required

After C++ compiles, the following Blueprint configuration is required:

### 1. Create IA_ToggleFlashlight Input Action

**Location:** Content/Variant_Horror/Input/

1. Right-click > Input > Input Action
2. Name: `IA_ToggleFlashlight`
3. Value Type: `Digital (Bool)`
4. No triggers needed (default press trigger)
5. Save

### 2. Add to Input Mapping Context

**Location:** Content/Variant_Horror/Input/IMC_Horror

1. Open IMC_Horror
2. Add new mapping
3. Action: `IA_ToggleFlashlight`
4. Key: `F` (Keyboard)
5. Save

### 3. Create GE_BatteryDrain Gameplay Effect

**Location:** Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/

1. Right-click > Blueprint Class > GameplayEffect
2. Name: `GE_BatteryDrain`
3. Configure:
   - **Duration Policy:** Infinite
   - **Period:** 0.5 seconds
   - **Execute Periodic Effect on Application:** true
   - **Modifiers:**
     - Attribute: `SereneAttributeSet.Battery`
     - Modifier Op: `Add`
     - Modifier Magnitude > Magnitude Calculation Type: `Scalable Float`
     - Scalable Float Magnitude: `-1.67`

**Note:** -1.67 per 0.5s = -3.34/sec = ~30 seconds from 100 to 0. Adjust as needed.

### 4. Configure BP_HorrorCharacter

**Location:** Content/Variant_Horror/Blueprints/BP_HorrorCharacter

1. Open BP_HorrorCharacter
2. In Components panel, select `FlashlightComponent`
3. In Details panel:
   - **Battery Drain Effect:** `GE_BatteryDrain`
   - **Toggle On Sound:** (optional - mechanical click sound)
   - **Toggle Off Sound:** (optional - mechanical click sound)
4. Select the root `HorrorCharacter` component
5. In Details panel under Input:
   - **Toggle Flashlight Action:** `IA_ToggleFlashlight`
6. Compile and Save

### 5. Verification Steps

1. Play in Editor (PIE)
2. Press **F** key - flashlight should turn on, illuminating the scene
3. Observe Battery HUD - should start decreasing (~3.3%/sec)
4. Press **F** again - flashlight should turn off, battery stops draining
5. Turn flashlight on and wait - should auto-off when battery hits 0
6. When off with 0 battery, press **F** - should NOT turn on

## Files Changed

| File | Lines | Change |
|------|-------|--------|
| FlashlightComponent.h | 160 | New - component header |
| FlashlightComponent.cpp | 276 | New - component implementation |
| HorrorCharacter.h | +15 | Added component, input action, method |
| HorrorCharacter.cpp | +22 | Added component creation and input binding |
| SereneGameplayTags.h | +3 | Added State_FlashlightOn declaration |
| SereneGameplayTags.cpp | +2 | Added State_FlashlightOn definition |

## Next Phase Readiness

**Plan 02 (Battery States & Flicker)** can proceed immediately:

- FlashlightComponent state machine ready for WarmingUp/Flickering/DyingOut states
- OnBatteryChanged delegate ready for threshold checks (<20% flicker)
- BaseIntensity property available for intensity manipulation

**Required for Plan 02:**
- Add battery threshold checks in OnBatteryChanged
- Implement flicker using timers + random intensity variation
- Add State.LowBattery tag integration (already exists in GAS)

## Test Checklist

After Blueprint configuration:

- [ ] F key toggles flashlight on/off
- [ ] Battery HUD decreases while flashlight is on
- [ ] Flashlight auto-turns off at 0% battery
- [ ] Cannot turn on flashlight with 0% battery
- [ ] State.FlashlightOn tag present when on (debug: `showdebug abilitysystem`)
- [ ] Toggle sounds play (if configured)
