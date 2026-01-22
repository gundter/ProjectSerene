---
phase: 04-consumables-inventory
plan: 01
subsystem: inventory
tags: [gas, gameplay-effects, data-assets, inventory, consumables, gameplay-tags]

# Dependency graph
requires:
  - phase: 01-core-attributes
    provides: GAS foundation, SerenePlayerState with ASC, Gameplay Effects pattern
provides:
  - UItemDataAsset for editor-friendly item definitions
  - UInventoryComponent for player inventory management
  - Item.Consumable.* gameplay tags for categorization
  - GAS integration for applying item effects
affects: [04-02, 05-investigation, pickup-system]

# Tech tracking
tech-stack:
  added: []
  patterns:
    - "UPrimaryDataAsset for item definitions"
    - "TMap<TObjectPtr, int32> for inventory storage"
    - "Category filtering via GameplayTag hierarchy"
    - "Quick slots for fast consumable access"

key-files:
  created:
    - Source/ProjectSerene/Inventory/ItemDataAsset.h
    - Source/ProjectSerene/Inventory/ItemDataAsset.cpp
    - Source/ProjectSerene/Inventory/InventoryComponent.h
    - Source/ProjectSerene/Inventory/InventoryComponent.cpp
  modified:
    - Source/ProjectSerene/GAS/SereneGameplayTags.h
    - Source/ProjectSerene/GAS/SereneGameplayTags.cpp
    - Source/ProjectSerene/Player/SerenePlayerState.h
    - Source/ProjectSerene/Player/SerenePlayerState.cpp
    - Source/ProjectSerene/ProjectSerene.Build.cs

key-decisions:
  - "InventoryComponent on PlayerState follows ASC ownership pattern for persistence"
  - "UPrimaryDataAsset over UDataAsset for Asset Manager integration"
  - "TMap with UPROPERTY for safe garbage collection"
  - "4 quick slots per CONTEXT.md specification"
  - "MatchesTag for category filtering supports hierarchy (Item.Consumable matches Medical/Tool)"

patterns-established:
  - "UPrimaryDataAsset pattern: GetPrimaryAssetId() returns Item:AssetName format"
  - "Item effect application: ASC->MakeOutgoingSpec + ApplyGameplayEffectSpecToSelf"
  - "OnInventoryChanged delegate for UI binding"
  - "Quick slot array with NUM_QUICK_SLOTS constant"

# Metrics
duration: 7min
completed: 2026-01-22
---

# Phase 4 Plan 1: Inventory Infrastructure Summary

**UItemDataAsset for item definitions, UInventoryComponent with GAS integration for applying consumable effects via Gameplay Effects**

## Performance

- **Duration:** 7 min
- **Started:** 2026-01-22T01:47:03Z
- **Completed:** 2026-01-22T01:54:25Z
- **Tasks:** 3/3
- **Files modified:** 9

## Accomplishments

- Created UItemDataAsset as UPrimaryDataAsset for editor-friendly item definitions with display properties, category tags, stack size, use effects, and pickup mesh
- Built UInventoryComponent with TMap storage, stack limits, GAS integration for UseItem, 4 quick slots, and category filtering
- Integrated InventoryComponent into SerenePlayerState following ASC ownership pattern for persistence
- Registered Item.Consumable, Item.Consumable.Medical, Item.Consumable.Tool, Item.Evidence gameplay tags

## Task Commits

Each task was committed atomically:

1. **Task 1: Create ItemDataAsset and Item Gameplay Tags** - `72dfd52` (feat)
2. **Task 2: Create InventoryComponent with GAS Integration** - `2e56aca` (feat)
3. **Task 3: Integrate InventoryComponent into SerenePlayerState** - `81597a1` (feat)

## Files Created/Modified

**Created:**
- `Source/ProjectSerene/Inventory/ItemDataAsset.h` - UPrimaryDataAsset subclass for item data (display, category, stack, effect, mesh)
- `Source/ProjectSerene/Inventory/ItemDataAsset.cpp` - GetPrimaryAssetId implementation for Asset Manager
- `Source/ProjectSerene/Inventory/InventoryComponent.h` - Inventory management with TMap storage, quick slots, GAS integration
- `Source/ProjectSerene/Inventory/InventoryComponent.cpp` - AddItem/RemoveItem/UseItem/GetItemsByCategory implementations

**Modified:**
- `Source/ProjectSerene/GAS/SereneGameplayTags.h` - Added Item_Consumable, Item_Consumable_Medical, Item_Consumable_Tool, Item_Evidence tags
- `Source/ProjectSerene/GAS/SereneGameplayTags.cpp` - Tag definitions with comments
- `Source/ProjectSerene/Player/SerenePlayerState.h` - Added InventoryComponent member and GetInventoryComponent accessor
- `Source/ProjectSerene/Player/SerenePlayerState.cpp` - CreateDefaultSubobject for InventoryComponent
- `Source/ProjectSerene/ProjectSerene.Build.cs` - Added Inventory to PublicIncludePaths

## Decisions Made

1. **InventoryComponent on PlayerState** - Follows established ASC ownership pattern; inventory persists across respawns like attributes
2. **UPrimaryDataAsset over UDataAsset** - Enables Asset Manager integration for async loading and organized asset discovery
3. **TMap with UPROPERTY** - Per RESEARCH.md Pitfall 2: prevents garbage collection issues with UObject keys
4. **WeakObjectPtr for ASC cache** - Avoids dangling references if PlayerState is destroyed
5. **MatchesTag for category filtering** - Hierarchical matching (Item.Consumable matches Item.Consumable.Medical) without explicit subtype checks

## Deviations from Plan

### Auto-fixed Issues

**1. [Rule 3 - Blocking] Fixed TMap::GetKeys template deduction error**
- **Found during:** Task 2 (InventoryComponent compilation)
- **Issue:** Items.GetKeys(TArray<UItemDataAsset*>) failed - TMap key is TObjectPtr<UItemDataAsset>, not UItemDataAsset*
- **Fix:** Replaced GetKeys with manual iteration over TMap pairs
- **Files modified:** Source/ProjectSerene/Inventory/InventoryComponent.cpp
- **Verification:** Compilation succeeded
- **Committed in:** 2e56aca (Task 2 commit)

---

**Total deviations:** 1 auto-fixed (1 blocking)
**Impact on plan:** Template type mismatch fix necessary for compilation. No scope creep.

## Issues Encountered

- UE 5.7 path discovery: Build.bat location was UE_5.7, not UE_5.5 as initially attempted. Resolved by checking Epic Games installation directory.

## User Setup Required

**Blueprint assets required after next Editor restart:**

1. **Create Item Data Assets:**
   - Right-click Content Browser > Miscellaneous > Data Asset
   - Select ItemDataAsset as parent class
   - Create DA_Bandage, DA_Battery, DA_Medication etc.
   - Set DisplayName, Description, Icon, ItemCategory (use Item.Consumable.Medical/Tool tags)
   - Set MaxStackSize (5 for common, 2 for rare)
   - Assign UseEffect (GE_UseBandage, GE_UseBattery, etc. - Instant effects modifying attributes)

2. **Create Item Gameplay Effects:**
   - GE_UseBandage: +25 Health (Instant)
   - GE_UseBattery: +50 Battery (Instant)
   - GE_UseMedication: +40 Sanity (Instant)

3. **Test Inventory:**
   - Get InventoryComponent from PlayerState
   - Call AddItem with a DA_* asset
   - Call UseItem to verify effect application

## Next Phase Readiness

**Ready for 04-02:**
- ItemDataAsset class ready for pickup actors to reference
- InventoryComponent ready for pickup interaction
- Quick slots ready for HUD integration
- OnInventoryChanged delegate ready for UI binding

**No blockers.** Infrastructure complete for pickup system implementation.

---
*Phase: 04-consumables-inventory*
*Plan: 01*
*Completed: 2026-01-22*
