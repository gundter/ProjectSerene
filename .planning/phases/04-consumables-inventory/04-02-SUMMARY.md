---
phase: 04-consumables-inventory
plan: 02
subsystem: interaction
tags: [interaction, interface, pickup, line-trace, timer, enhanced-input]

# Dependency graph
requires:
  - phase: 04-01
    provides: ItemDataAsset, InventoryComponent, Item.* gameplay tags
provides:
  - IInteractableTarget interface for all interactable world objects
  - AConsumablePickup actor for world item pickups
  - Line trace interaction detection on HorrorPlayerController
  - Interaction prompt UI integration via HorrorUI
affects: [05-investigation, doors, containers, evidence-pickups]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "IInteractableTarget interface with BlueprintNativeEvent methods"
    - "Timer-based line trace checking (0.1s interval)"
    - "Custom depth stencil for outline highlighting"
    - "Execute_* static methods for interface calls"

key-files:
  created:
    - Source/ProjectSerene/Inventory/InteractableInterface.h
    - Source/ProjectSerene/Variant_Horror/Pickups/ConsumablePickup.h
    - Source/ProjectSerene/Variant_Horror/Pickups/ConsumablePickup.cpp
  modified:
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.h
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp
    - Source/ProjectSerene/Variant_Horror/UI/HorrorUI.h
    - Source/ProjectSerene/Variant_Horror/UI/HorrorUI.cpp
    - Source/ProjectSerene/ProjectSerene.Build.cs

key-decisions:
  - "Timer-based interaction checking at 0.1s interval for performance (not Tick)"
  - "GetPlayerViewPoint for line trace origin matches camera exactly"
  - "WeakObjectPtr for CurrentInteractable to handle destroyed actors"
  - "Custom depth stencil value 255 for outline highlight"
  - "BlueprintImplementableEvent for prompt UI allows full Blueprint customization"

patterns-established:
  - "IInteractableTarget::Execute_* for calling interface methods on actors"
  - "FTimerHandle with looping timer for periodic checks"
  - "Focus state management with OnFocused/OnUnfocused pair"
  - "Interaction prompt forwarding through PlayerController to UI"

# Metrics
duration: 5min
completed: 2026-01-22
---

# Phase 4 Plan 2: Pickup System Summary

**Lyra-pattern IInteractableTarget interface with line trace detection, ConsumablePickup actor implementing inventory integration, timer-based interaction checking on HorrorPlayerController**

## Performance

- **Duration:** 5 min
- **Started:** 2026-01-22T01:58:01Z
- **Completed:** 2026-01-22T02:03:33Z
- **Tasks:** 3/3
- **Files created:** 3
- **Files modified:** 5

## Accomplishments

- Created IInteractableTarget interface with OnFocused, OnUnfocused, OnInteract, GetInteractionPrompt methods using Lyra-pattern BlueprintNativeEvent
- Built AConsumablePickup actor implementing IInteractableTarget with custom depth highlight, ItemData reference, stack limit checking, and auto-destroy on pickup
- Added timer-based line trace interaction detection to HorrorPlayerController with 0.1s interval for performance
- Integrated interaction prompt display through HorrorUI using BlueprintImplementableEvent pattern

## Task Commits

Each task was committed atomically:

1. **Task 1: Create IInteractableTarget Interface** - `a30b2cf` (feat)
2. **Task 2: Create ConsumablePickup Actor** - `6285e6a` (feat)
3. **Task 3: Add Interaction Detection to HorrorPlayerController** - `fa32117` (feat)

## Files Created/Modified

**Created:**
- `Source/ProjectSerene/Inventory/InteractableInterface.h` - UInterface with BlueprintNativeEvent methods for interaction
- `Source/ProjectSerene/Variant_Horror/Pickups/ConsumablePickup.h` - World pickup actor header
- `Source/ProjectSerene/Variant_Horror/Pickups/ConsumablePickup.cpp` - Implements IInteractableTarget, adds to inventory, destroys self

**Modified:**
- `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.h` - Added interaction system properties and methods
- `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp` - Timer-based line trace, focus management, input binding
- `Source/ProjectSerene/Variant_Horror/UI/HorrorUI.h` - Added ShowInteractionPrompt/HideInteractionPrompt methods
- `Source/ProjectSerene/Variant_Horror/UI/HorrorUI.cpp` - Forward interaction prompt to Blueprint events
- `Source/ProjectSerene/ProjectSerene.Build.cs` - Added Variant_Horror/Pickups to include paths

## Decisions Made

1. **Timer-based interaction checking** - 0.1s interval balances responsiveness with performance; more efficient than Tick
2. **GetPlayerViewPoint for line trace** - Uses camera location and rotation directly, accurate for first-person
3. **WeakObjectPtr for CurrentInteractable** - Safe reference that auto-clears if actor is destroyed
4. **Custom depth stencil 255** - Standard value for post-process outline detection
5. **BlueprintImplementableEvent for prompt** - Allows full Blueprint control over prompt text widget and animations

## Deviations from Plan

None - plan executed exactly as written.

## Issues Encountered

None - all tasks compiled and executed cleanly.

## User Setup Required

**Blueprint configuration required after Editor restart:**

1. **Create IA_Interact Input Action:**
   - Project Settings > Input > Input Actions
   - Create Digital Bool action named IA_Interact

2. **Add to Input Mapping Context:**
   - Add IA_Interact to IMC_Horror
   - Bind to E key

3. **Configure BP_HorrorPlayerController:**
   - Open BP_HorrorPlayerController
   - Set InteractAction property to IA_Interact

4. **Configure BP_HorrorUI interaction prompt:**
   - Implement BP_ShowInteractionPrompt event
   - Implement BP_HideInteractionPrompt event
   - Add text widget for prompt display

5. **Create BP_ConsumablePickup Blueprints:**
   - Create subclass of ConsumablePickup
   - Assign ItemData (DA_Bandage, DA_Battery, etc.)
   - Set Quantity (default 1)
   - Optionally assign PickupSound

6. **Set up outline post-process material:**
   - Create post-process material reading custom depth
   - Apply outline effect when stencil value = 255

## Next Phase Readiness

**Ready for Phase 5 (Investigation System):**
- IInteractableTarget interface can be used for evidence pickups
- Pattern established for any world interactable (doors, containers)
- Interaction prompt UI ready for evidence-specific messages

**Ready for inventory UI:**
- InventoryComponent has OnInventoryChanged delegate
- Items can be picked up and appear in inventory
- Quick slots ready for radial menu binding

**No blockers.** Pickup system complete and functional.

---
*Phase: 04-consumables-inventory*
*Plan: 02*
*Completed: 2026-01-22*
