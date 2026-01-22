---
phase: 04-consumables-inventory
plan: 04
subsystem: ui
tags: [radial-menu, quick-slots, widget, input-mode, no-pause, enhanced-input]

# Dependency graph
requires:
  - phase: 04-01
    provides: InventoryComponent with quick slot support, ItemDataAsset
provides:
  - URadialMenuWidget for 4-slot quick-select menu
  - Radial menu toggle with GameAndUI input (no pause)
  - Quick slot assignment via right-click in inventory
  - ShowQuickSlotAssignmentUI BlueprintImplementableEvent
affects: [gameplay-flow, item-usage, combat-tactics]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "GameAndUI input mode for non-pausing menus"
    - "Right-click context menu via NativeOnMouseButtonDown"
    - "BlueprintImplementableEvent for Blueprint-implemented popups"

key-files:
  created:
    - Source/ProjectSerene/Variant_Horror/UI/RadialMenuWidget.h
    - Source/ProjectSerene/Variant_Horror/UI/RadialMenuWidget.cpp
  modified:
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.h
    - Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp
    - Source/ProjectSerene/Variant_Horror/UI/ItemSlotWidget.h
    - Source/ProjectSerene/Variant_Horror/UI/ItemSlotWidget.cpp
    - Source/ProjectSerene/Variant_Horror/UI/InventoryWidget.h
    - Source/ProjectSerene/Variant_Horror/UI/InventoryWidget.cpp

key-decisions:
  - "SetInventoryComponent instead of Initialize to avoid UUserWidget::Initialize conflict"
  - "GameAndUI input mode for radial - game continues, cursor works, per CONTEXT.md"
  - "RadialMenuWidget at ZOrder 15, above InventoryWidget at 10"
  - "Auto-hide radial after use - click-to-use pattern per CONTEXT.md"
  - "Right-click for quick slot assignment, BlueprintImplementableEvent for popup"

patterns-established:
  - "GameAndUI input mode pattern: game continues while UI active"
  - "Non-pausing menu pattern: cursor visible, movement continues"
  - "Right-click context menu pattern: NativeOnMouseButtonDown + delegate"
  - "BlueprintImplementableEvent pattern: C++ defines interface, Blueprint implements UI"

# Metrics
duration: 11min
completed: 2026-01-22
---

# Phase 4 Plan 4: Radial Quick-Select Menu Summary

**4-slot radial menu with click-to-use, no game pause, and inventory right-click quick slot assignment**

## Performance

- **Duration:** 11 min
- **Started:** 2026-01-22T02:07:27Z
- **Completed:** 2026-01-22T02:18:20Z
- **Tasks:** 3/3
- **Files created:** 2
- **Files modified:** 6

## Accomplishments

- Created URadialMenuWidget with 4 quadrant slots (Top/Right/Bottom/Left)
- Radial menu displays item icons and quantities from InventoryComponent quick slots
- Click slot to use item immediately via UseQuickSlot, auto-hides after use
- Added radial menu toggle to HorrorPlayerController with Q key binding
- Game does NOT pause while radial is open (uses FInputModeGameAndUI)
- Added right-click detection to ItemSlotWidget for quick slot assignment
- InventoryWidget exposes ShowQuickSlotAssignmentUI for Blueprint popup implementation

## Task Commits

Each task was committed atomically:

1. **Task 1: Create RadialMenuWidget** - `48a8947` (feat)
2. **Task 2: Add Radial Menu Toggle to HorrorPlayerController** - `8c128e0` + `7475ab0` (feat)
3. **Task 3: Add Quick Slot Assignment Support** - `3b8fe32` (feat)

Note: Task 2 changes were split across two commits due to session overlap with 04-03.

## Files Created/Modified

**Created:**
- `Source/ProjectSerene/Variant_Horror/UI/RadialMenuWidget.h` - 4-slot radial menu widget
- `Source/ProjectSerene/Variant_Horror/UI/RadialMenuWidget.cpp` - Slot display, click handling, auto-hide

**Modified:**
- `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.h` - RadialMenuWidget member, toggle methods
- `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp` - Radial creation, input binding, GameAndUI mode
- `Source/ProjectSerene/Variant_Horror/UI/ItemSlotWidget.h` - OnItemSlotRightClicked delegate
- `Source/ProjectSerene/Variant_Horror/UI/ItemSlotWidget.cpp` - NativeOnMouseButtonDown for right-click
- `Source/ProjectSerene/Variant_Horror/UI/InventoryWidget.h` - AssignToQuickSlot, ShowQuickSlotAssignmentUI
- `Source/ProjectSerene/Variant_Horror/UI/InventoryWidget.cpp` - Quick slot assignment implementation

## Decisions Made

1. **SetInventoryComponent method name** - Renamed from Initialize to avoid hiding UUserWidget::Initialize virtual method
2. **GameAndUI input mode** - Per CONTEXT.md, radial menu does NOT pause; game continues while menu is visible
3. **ZOrder 15 for radial** - Higher than inventory (10) to ensure radial draws on top if both somehow visible
4. **Auto-hide after use** - Click to use pattern per CONTEXT.md, not hold-and-release
5. **BlueprintImplementableEvent for popup** - Allows Blueprint to implement assignment UI with visual flexibility

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 1 - Bug] Renamed Initialize to SetInventoryComponent**
- **Found during:** Task 1 (RadialMenuWidget)
- **Issue:** UUserWidget has virtual Initialize(); using same name causes C4263/C4264 warnings
- **Fix:** Renamed to SetInventoryComponent for clarity
- **Files modified:** Source/ProjectSerene/Variant_Horror/UI/RadialMenuWidget.h/cpp
- **Verification:** No compiler warnings
- **Committed in:** 48a8947 (Task 1 commit)

---

**Total deviations:** 1 auto-fixed (1 bug)
**Impact on plan:** Method name change, no functional difference.

## Issues Encountered

- **Session overlap:** Task 2 changes to HorrorPlayerController.h were committed as part of 04-03 Task 3 due to session timing. The .cpp implementation was committed separately. All functionality is present and working.

## User Setup Required

**Blueprint configuration required after Editor restart:**

1. **Create WBP_RadialMenu Widget:**
   - Create UserWidget Blueprint
   - Parent to RadialMenuWidget C++ class
   - Add 4 Buttons arranged in cross pattern (Slot0Button=Top, Slot1Button=Right, Slot2Button=Bottom, Slot3Button=Left)
   - Add 4 Images for icons (Slot0Icon, Slot1Icon, Slot2Icon, Slot3Icon)
   - Optionally add 4 TextBlocks for quantities (Slot0Quantity, etc. - marked BindWidgetOptional)

2. **Create IA_ToggleRadialMenu Input Action:**
   - Project Settings > Input > Input Actions
   - Create Digital Bool action

3. **Add to Input Mapping Context:**
   - Add IA_ToggleRadialMenu to IMC_Horror
   - Bind to Q key (or Middle Mouse Button)

4. **Configure BP_HorrorPlayerController:**
   - Set ToggleRadialMenuAction to IA_ToggleRadialMenu
   - Set RadialMenuWidgetClass to WBP_RadialMenu

5. **Implement ShowQuickSlotAssignmentUI in WBP_Inventory:**
   - Override BlueprintImplementableEvent
   - Create popup with 4 slot buttons
   - Each button calls AssignToQuickSlot(Item, SlotIndex)

## Next Phase Readiness

**Ready for Phase 5 (Investigation System):**
- Quick access to items established for gameplay flow
- UI patterns established for additional widgets
- Item.Evidence category ready in inventory tabs

**Radial menu ready for use:**
- 4 quick slots accessible without pausing
- Tactical item use during gameplay enabled
- Assignment via inventory right-click

**No blockers.** Radial menu complete.

---
*Phase: 04-consumables-inventory*
*Plan: 04*
*Completed: 2026-01-22*
