# Phase 4: Blueprint Configuration Checklist

**Status:** Pending human verification
**Resume command:** `/gsd:execute-phase 4` (will continue from checkpoint)

---

## 1. Input Actions

Create in `Content/Variant_Horror/Input/Actions/`:

| Action | Type | Key | Notes |
|--------|------|-----|-------|
| IA_Interact | Digital (Bool) | E | |
| IA_ToggleInventory | Digital (Bool) | Tab | **Execute When Paused = true** |
| IA_ToggleRadialMenu | Digital (Bool) | Q | |

Add all three to `IMC_Horror` Input Mapping Context.

---

## 2. Gameplay Effects

Create in `Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/`:

See `ConsumableEffects_Setup.md` for full details. Summary:

| Effect | Duration | Modifier | Attribute | Magnitude |
|--------|----------|----------|-----------|-----------|
| GE_RestoreBattery | Instant | Override | Battery | Attribute-Based (MaxBattery) |
| GE_RestoreHealth_Small | Instant | Add | Health | Scalable Float: 30 |
| GE_RestoreHealth_Large | Instant | Add | Health | Scalable Float: 75 |
| GE_RestoreStamina | Instant | Add | Stamina | Scalable Float: 50 |
| GE_RestoreSanity_Mild | Instant | Add | Sanity | Scalable Float: 30 |
| GE_RestoreSanity_Full | Instant | Override | Sanity | Attribute-Based (MaxSanity) |

**How to create partial restore effects (Add + Scalable Float):**
1. Right-click > Blueprint Class > GameplayEffect
2. Duration Policy = Instant
3. Modifiers > Add element
4. Attribute = SereneAttributeSet.{AttributeName}
5. Modifier Op = Add
6. Modifier Magnitude > Magnitude Calculation Type = Scalable Float
7. Scalable Float Magnitude = {Value}

**How to create full restore effects (Override + Attribute-Based):**
1. Right-click > Blueprint Class > GameplayEffect
2. Duration Policy = Instant
3. Modifiers > Add element
4. Attribute = SereneAttributeSet.{AttributeName}
5. Modifier Op = Override
6. Modifier Magnitude > Magnitude Calculation Type = Attribute Based
7. Backing Attribute > Attribute to Capture = SereneAttributeSet.Max{AttributeName}
8. Attribute Source = Target

---

## 3. Item Data Assets

Create in `Content/Variant_Horror/Blueprints/Items/`:

See `ItemAssets_Setup.md` for full details. Summary:

| Asset | Display Name | Category Tag | Stack | Use Effect |
|-------|--------------|--------------|-------|------------|
| DA_Battery | Battery | Item.Consumable.Tool | 5 | GE_RestoreBattery |
| DA_Bandage | Bandage | Item.Consumable.Medical | 10 | GE_RestoreHealth_Small |
| DA_MedKit | Med Kit | Item.Consumable.Medical | 3 | GE_RestoreHealth_Large |
| DA_EnergyDrink | Energy Drink | Item.Consumable.Tool | 5 | GE_RestoreStamina |
| DA_MedicationMild | Mild Medication | Item.Consumable.Medical | 5 | GE_RestoreSanity_Mild |
| DA_MedicationStrong | Strong Medication | Item.Consumable.Medical | 2 | GE_RestoreSanity_Full |

**How to create each:**
1. Right-click > Miscellaneous > Data Asset
2. Select ItemDataAsset class
3. Fill in: DisplayName, Description, ItemCategory tag, MaxStackSize, UseEffect reference
4. Optionally assign Icon texture and PickupMesh

---

## 4. UI Widgets

Create in `Content/Variant_Horror/UI/`:

### WBP_ItemSlot (parent: ItemSlotWidget)
Required BindWidget components:
- `ItemIcon` (Image)
- `QuantityText` (TextBlock)
- `SlotButton` (Button)

### WBP_Inventory (parent: InventoryWidget)
Required BindWidget components:
- `CategorySwitcher` (WidgetSwitcher)
- `MedicalItemList` (TileView)
- `ToolsItemList` (TileView)
- `EvidenceItemList` (TileView)
- `MedicalTabButton` (Button)
- `ToolsTabButton` (Button)
- `EvidenceTabButton` (Button)

Set TileView Entry Widget Class = WBP_ItemSlot

### WBP_RadialMenu (parent: RadialMenuWidget)
Required BindWidget components:
- `Slot0Button`, `Slot1Button`, `Slot2Button`, `Slot3Button` (Buttons)
- `Slot0Icon`, `Slot1Icon`, `Slot2Icon`, `Slot3Icon` (Images)
- `Slot0Quantity`, `Slot1Quantity`, `Slot2Quantity`, `Slot3Quantity` (TextBlocks, optional)

Layout: Cross pattern (up/down/left/right) or 4 quadrants

---

## 5. Test Pickup Actor

Create `BP_Pickup_Battery`:
1. Right-click > Blueprint Class > ConsumablePickup
2. Set ItemData = DA_Battery
3. Optionally set mesh in PickupMesh component
4. Place in test level

---

## 6. Configure BP_HorrorPlayerController

Open `BP_HorrorPlayerController` and set:
- Interact Action = IA_Interact
- Toggle Inventory Action = IA_ToggleInventory
- Toggle Radial Menu Action = IA_ToggleRadialMenu
- Inventory Widget Class = WBP_Inventory
- Radial Menu Widget Class = WBP_RadialMenu

---

## 7. Runtime Verification Test

1. PIE > walk to battery pickup
2. Look at it > should highlight + show "Press E to pick up Battery"
3. Press E > item disappears
4. Press Tab > inventory opens, game pauses
5. Find battery in Tools tab
6. Click battery > battery attribute restores to 100% (check HUD)
7. Press Tab > inventory closes, game unpauses
8. Press Q > radial menu opens (game does NOT pause)
9. Press Q > radial menu closes

**Optional advanced tests:**
- Pick up multiple items, verify stack counts
- Right-click item in inventory > assign to quick slot
- Use item from radial menu

---

## Resume Instructions

When ready to continue on any machine:

```
/gsd:execute-phase 4
```

GSD will detect 04-05 is incomplete and resume from the checkpoint. Type "verified" after testing passes, or describe any issues.

---

*Created: 2026-01-21*
