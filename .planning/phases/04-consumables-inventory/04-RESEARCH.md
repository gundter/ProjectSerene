# Phase 4: Consumables & Inventory - Research

**Researched:** 2026-01-21
**Domain:** Inventory Systems, GAS Item Consumables, UMG UI, Interaction Systems
**Confidence:** HIGH

## Summary

Phase 4 implements an inventory system for collecting and using consumable items (batteries, med kits, energy drinks, medication) that modify player attributes through the existing GAS infrastructure. The research reveals a well-established architecture pattern: `UPrimaryDataAsset` for item definitions, `UActorComponent` for inventory management, instant `UGameplayEffect` for attribute modification, and `IInteractableTarget` interface for pickup interaction.

The project already has strong GAS foundations (SereneAttributeSet with Health, Stamina, Sanity, Battery; SereneAbilitySystemComponent; attribute change delegates). Consumables naturally extend this by applying instant Gameplay Effects when items are used. The interaction system should follow the Lyra pattern: line trace detection with an interface-based design for pickup actors.

**Primary recommendation:** Use `UPrimaryDataAsset` subclass for item definitions, `UInventoryComponent` on PlayerState for inventory management, instant Gameplay Effects for attribute restoration, and `IInteractableTarget` interface for world pickups. Leverage existing `BlueprintImplementableEvent` patterns from HorrorUI for inventory UI.

## Standard Stack

The established libraries/tools for this domain:

### Core
| Library/Class | Version | Purpose | Why Standard |
|---------------|---------|---------|--------------|
| UPrimaryDataAsset | UE5.5+ | Item data definitions | Asset Manager integration, async loading, editor-friendly |
| UActorComponent | UE5.5+ | Inventory management | Follows project pattern (FlashlightComponent, SanityPerceptionComponent) |
| UGameplayEffect (Instant) | UE5.5+ | Attribute restoration | Already used for attribute initialization; instant effects bypass container |
| IInteractableTarget | Custom | Pickup interface | Lyra-proven pattern for line trace interaction |
| UTileView | UE5.5+ | Inventory UI grid | Performant for item lists, virtualizes off-screen items |

### Supporting
| Library/Class | Version | Purpose | When to Use |
|---------------|---------|---------|-------------|
| FGameplayTag | UE5.5+ | Item categorization | Hierarchical tags like Item.Consumable.Battery |
| Custom Depth Stencil | UE5.5+ | Pickup highlighting | Visual feedback for interactable items |
| UAnimMontage | UE5.5+ | Use animations | 1-2 second consume sequences |
| Enhanced Input | UE5.5+ | Inventory toggle, interact | Already configured in project |

### Alternatives Considered
| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| UPrimaryDataAsset | FTableRowHandle | DataTables less flexible for complex item data, harder to extend |
| Custom IInteractableTarget | Overlap collision | Line trace is more precise for "looking at" detection |
| UTileView | ScrollBox + manual items | TileView handles virtualization automatically, better performance |
| CommonUI | Standard UMG | CommonUI adds complexity; standard UMG sufficient for single-player |

**Installation:**
No additional plugins required. All functionality is built into UE5 core and GAS modules already enabled.

## Architecture Patterns

### Recommended Project Structure
```
Source/ProjectSerene/
├── GAS/
│   └── SereneGameplayTags.h      # Add Item.* tags
├── Inventory/
│   ├── ItemDataAsset.h/cpp       # UPrimaryDataAsset subclass for item definitions
│   ├── InventoryComponent.h/cpp  # UActorComponent for inventory management
│   └── InteractableInterface.h   # IInteractableTarget interface
├── Variant_Horror/
│   ├── Pickups/
│   │   └── ConsumablePickup.h/cpp  # World pickup actor implementing interface
│   └── UI/
│       ├── InventoryWidget.h/cpp   # Main inventory panel
│       ├── ItemSlotWidget.h/cpp    # Individual item display
│       └── RadialMenuWidget.h/cpp  # Quick-select radial menu
```

### Pattern 1: Item Data Asset Definition
**What:** Define item properties in UPrimaryDataAsset subclass, separate from world representation
**When to use:** All item types (consumables, tools, evidence)

```cpp
// Source: UE5 Official Documentation - Data Assets
UCLASS(BlueprintType)
class UItemDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    // Display
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    FText Description;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    UTexture2D* Icon;

    // Categorization
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    FGameplayTag ItemCategory;  // Item.Consumable.Medical, Item.Consumable.Tool, etc.

    // Stacking
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
    int32 MaxStackSize = 5;

    // Effect to apply when used
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Consumable")
    TSubclassOf<UGameplayEffect> UseEffect;

    // World representation
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Pickup")
    UStaticMesh* PickupMesh;

    // Asset Manager integration
    virtual FPrimaryAssetId GetPrimaryAssetId() const override
    {
        return FPrimaryAssetId(TEXT("Item"), GetFName());
    }
};
```

### Pattern 2: Inventory Component on PlayerState
**What:** Manage inventory data alongside GAS on PlayerState for persistence
**When to use:** Player inventory (follows ASC ownership pattern)

```cpp
// Source: Project pattern - SerenePlayerState owns ASC
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    // Add item, returns false if inventory full
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool AddItem(UItemDataAsset* Item, int32 Quantity = 1);

    // Use item, applies GameplayEffect via ASC
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool UseItem(UItemDataAsset* Item);

    // Quick slots for radial menu (4 slots per CONTEXT.md)
    UPROPERTY(EditDefaultsOnly, Category = "Inventory")
    TArray<UItemDataAsset*> QuickSlots;  // Size 4

    // Delegates for UI binding
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, UItemDataAsset*, Item, int32, NewQuantity);
    FOnInventoryChanged OnInventoryChanged;

private:
    // Item storage: maps item asset to quantity
    UPROPERTY()
    TMap<UItemDataAsset*, int32> Items;
};
```

### Pattern 3: Interactable Interface for Pickups
**What:** Interface-based design allowing line trace interaction detection
**When to use:** All world objects player can interact with

```cpp
// Source: Lyra Interaction System Documentation
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableTarget : public UInterface
{
    GENERATED_BODY()
};

class IInteractableTarget
{
    GENERATED_BODY()
public:
    // Called when player looks at this object
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnFocused(AActor* InteractionInstigator);

    // Called when player looks away
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnUnfocused(AActor* InteractionInstigator);

    // Called when player presses interact key
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnInteract(AActor* InteractionInstigator);

    // Get interaction prompt text
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionPrompt() const;
};
```

### Pattern 4: Instant Gameplay Effect for Attribute Restoration
**What:** Use Duration=Instant with Additive modifier for permanent attribute changes
**When to use:** All consumable item effects

```cpp
// Source: GAS Documentation - Instant Effects
// Blueprint configuration for GE_RestoreBattery:
// - Duration Policy: Instant
// - Modifiers:
//   - Attribute: SereneAttributeSet.Battery
//   - Modifier Op: Add
//   - Magnitude: Scalable Float (100 for full restore)
//
// Batteries per CONTEXT.md: Full restore (0 -> 100%)
// Med kits: Bandages +30, Med kits +75
// Medication: Mild +30 fixed, Strong = full restore
```

### Pattern 5: Line Trace Interaction Detection
**What:** Periodic line trace from camera to detect interactable objects
**When to use:** E-key interaction system (per CONTEXT.md)

```cpp
// Source: Lyra Interaction System - simplified for single-player
void AHorrorPlayerController::CheckForInteractable()
{
    // Fire line trace from camera
    FVector Start = GetPlayerCameraLocation();
    FVector End = Start + GetPlayerCameraForward() * InteractionRange;

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(GetPawn());

    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
    {
        if (IInteractableTarget* Interactable = Cast<IInteractableTarget>(HitResult.GetActor()))
        {
            if (CurrentInteractable != HitResult.GetActor())
            {
                // Focus changed
                if (CurrentInteractable)
                {
                    IInteractableTarget::Execute_OnUnfocused(CurrentInteractable, GetPawn());
                }
                CurrentInteractable = HitResult.GetActor();
                IInteractableTarget::Execute_OnFocused(CurrentInteractable, GetPawn());

                // Show prompt UI
                ShowInteractionPrompt(IInteractableTarget::Execute_GetInteractionPrompt(CurrentInteractable));
            }
        }
    }
}
```

### Anti-Patterns to Avoid
- **Storing UObject* in TArray without UPROPERTY:** Memory will be garbage collected; always mark with UPROPERTY()
- **Checking inventory on Tick:** Use delegates and events, check only when interaction occurs
- **Hard-coding item data in pickup actors:** Use DataAsset references for flexibility
- **Direct attribute modification without GAS:** Always use Gameplay Effects for attribute changes to maintain tag/delegate consistency
- **Synchronous asset loading for item icons:** Use FStreamableManager for async loading if many items

## Don't Hand-Roll

Problems that look simple but have existing solutions:

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Item data storage | Custom FStruct arrays | UPrimaryDataAsset | Asset Manager, async loading, editor tooling |
| Attribute restoration | Direct SetValue calls | Instant UGameplayEffect | Maintains GAS delegate triggers, tag updates |
| Grid UI layout | Manual positioning | UTileView | Built-in virtualization, scrolling, selection |
| Object highlighting | Custom materials per object | Custom Depth Stencil | Global post-process, single material instance |
| Input mode switching | Manual cursor/input toggles | SetInputMode functions | Handles focus, cursor, input routing correctly |
| Game pausing | Custom time dilation | SetGamePaused | Proper engine-level pause handling |

**Key insight:** The GAS infrastructure already handles attribute changes with delegates (used by HorrorUI). Using Gameplay Effects ensures existing UI bindings automatically update when consumables restore attributes.

## Common Pitfalls

### Pitfall 1: Instant Effect Not Triggering PostGameplayEffectExecute
**What goes wrong:** Instant effects bypass Active Gameplay Effects Container, may miss delegate callbacks
**Why it happens:** Instant effects execute immediately and are not stored
**How to avoid:** PostGameplayEffectExecute DOES trigger for instant effects - ensure you're checking BaseValue changes, not CurrentValue
**Warning signs:** Attribute changes but no UI update

### Pitfall 2: TMap with UObject Keys Causing Crashes
**What goes wrong:** UObject pointers as TMap keys can become invalid after GC
**Why it happens:** UObjects can be garbage collected; TMap doesn't prevent this
**How to avoid:** Use UPROPERTY() on the TMap, or use FPrimaryAssetId as key instead
**Warning signs:** Intermittent crashes when accessing inventory

### Pitfall 3: UI Input Mode Breaking Gameplay Input
**What goes wrong:** After closing inventory, player can't move or look around
**Why it happens:** SetInputMode not properly reset to Game Only
**How to avoid:** Always pair UI mode entry with explicit Game mode restoration; use stack-based approach if multiple menus
**Warning signs:** Character frozen after closing UI

### Pitfall 4: Custom Depth Not Rendering on Nanite Meshes
**What goes wrong:** Outline highlight doesn't appear on pickup objects
**Why it happens:** Custom Depth Stencil doesn't work with Nanite as of UE5.5
**How to avoid:** Use non-Nanite meshes for pickup actors, or use material-based outline
**Warning signs:** Some pickups highlight, others don't

### Pitfall 5: Interaction Check Running on Server in Single-Player
**What goes wrong:** Unnecessary performance overhead, potential timing issues
**Why it happens:** Code written for multiplayer without single-player optimization
**How to avoid:** Check IsLocalController() before running interaction traces; project is single-player
**Warning signs:** Interaction feels laggy or delayed

### Pitfall 6: Animation Montage Blocking Movement
**What goes wrong:** Player can't walk while using item
**Why it happens:** Montage plays on full body slot, or root motion enabled
**How to avoid:** Play montage on upper body slot only; ensure root motion disabled for consume animations
**Warning signs:** Character stops moving during item use

## Code Examples

Verified patterns from official sources and project conventions:

### Applying Instant Gameplay Effect for Item Use
```cpp
// Source: Project pattern - SerenePlayerState::InitializeAttributes
void UInventoryComponent::UseItem(UItemDataAsset* Item)
{
    if (!Item || !Item->UseEffect)
    {
        return;
    }

    // Get ASC from owning PlayerState (follows project pattern)
    ASerenePlayerState* PS = Cast<ASerenePlayerState>(GetOwner());
    if (!PS)
    {
        return;
    }

    UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
    if (!ASC)
    {
        return;
    }

    // Apply instant effect (same pattern as attribute initialization)
    FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
    ContextHandle.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Item->UseEffect, 1, ContextHandle);
    if (SpecHandle.IsValid())
    {
        ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

        // Decrement item count
        RemoveItem(Item, 1);
    }
}
```

### Custom Depth Stencil Highlight Toggle
```cpp
// Source: Tom Looman - Custom Depth in Unreal Engine
void AConsumablePickup::OnFocused_Implementation(AActor* InteractionInstigator)
{
    // Enable custom depth for outline effect
    if (MeshComponent)
    {
        MeshComponent->SetRenderCustomDepth(true);
        MeshComponent->SetCustomDepthStencilValue(STENCIL_INTERACTABLE);  // e.g., 255
    }
}

void AConsumablePickup::OnUnfocused_Implementation(AActor* InteractionInstigator)
{
    if (MeshComponent)
    {
        MeshComponent->SetRenderCustomDepth(false);
    }
}
```

### Inventory UI with TileView Binding
```cpp
// Source: UE5 Documentation - TileView
// In Blueprint, but pattern shown in C++:
void UInventoryWidget::RefreshInventory()
{
    TileView->ClearListItems();

    for (const auto& Pair : InventoryComponent->GetItems())
    {
        UItemDataAsset* Item = Pair.Key;
        int32 Quantity = Pair.Value;

        // Create data object for TileView entry
        UInventorySlotData* SlotData = NewObject<UInventorySlotData>();
        SlotData->Item = Item;
        SlotData->Quantity = Quantity;

        TileView->AddItem(SlotData);
    }
}
```

### Game Pause with Input Mode Switch
```cpp
// Source: UE5 Documentation - Creating a Pause Menu
void AHorrorPlayerController::OpenInventory()
{
    if (InventoryWidget)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Visible);

        // Pause game per CONTEXT.md requirement
        UGameplayStatics::SetGamePaused(GetWorld(), true);

        // Switch to UI input mode
        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;
    }
}

void AHorrorPlayerController::CloseInventory()
{
    if (InventoryWidget)
    {
        InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

        // Resume game
        UGameplayStatics::SetGamePaused(GetWorld(), false);

        // Return to game input mode
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = false;
    }
}
```

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| FTableRowHandle for items | UPrimaryDataAsset | UE4.17+ | Better asset management, async loading |
| Direct attribute modification | Gameplay Effects | GAS standard | Proper delegate firing, tag management |
| Manual UI item lists | TileView/ListView | UE4.22+ | Automatic virtualization, better performance |
| Overlap for interaction | Line trace with interface | Lyra (UE5) | More precise "looking at" detection |
| CommonUI for all UIs | Standard UMG for single-player | Current | CommonUI adds complexity unnecessary for single-player |

**Deprecated/outdated:**
- **TArray<FInventorySlot> on Character:** Move to PlayerState component for persistence
- **String-based item IDs:** Use FGameplayTag or FPrimaryAssetId for type safety
- **Manual SetAttribute calls:** Always use Gameplay Effects

## Open Questions

Things that couldn't be fully resolved:

1. **First-person consume animation approach**
   - What we know: AnimMontage on upper body slot, played during item use
   - What's unclear: Whether project has first-person arms mesh set up, or uses camera-attached static mesh
   - Recommendation: Check existing character setup; if no arms mesh, consider simple camera shake or skip animation for Phase 4

2. **Radial menu exact implementation**
   - What we know: 4 slots, Tab to toggle open, click to use
   - What's unclear: Best UMG approach for radial layout (sector-based widgets vs circular arrangement)
   - Recommendation: Use Simple radial with 4 quadrant buttons; don't need full pie menu for 4 items

3. **Energy drink stamina restoration amount**
   - What we know: Per CONTEXT.md, amount is "TBD by planner"
   - What's unclear: Balance target
   - Recommendation: Start with 50% (50 stamina), same as mild medication pattern

## Sources

### Primary (HIGH confidence)
- UE5 Official Documentation: Data Assets, Gameplay Effects, TileView
- Lyra Sample Game Interaction System documentation
- Project codebase: SereneAttributeSet, SerenePlayerState, FlashlightComponent patterns
- GASDocumentation (tranek/GASDocumentation) - GAS architecture patterns

### Secondary (MEDIUM confidence)
- Tom Looman tutorials: Custom Depth, Interaction systems
- Epic Developer Community forums: TileView, inventory best practices
- Unrealist.org: CommonUI tabs and switchers

### Tertiary (LOW confidence)
- Community marketplace assets: Referenced for pattern validation only
- Forum discussions: Implementation alternatives

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH - UPrimaryDataAsset, GAS, TileView are documented Epic patterns
- Architecture: HIGH - Follows established project patterns (component-based, GAS integration)
- Pitfalls: MEDIUM - Gathered from multiple sources, some edge cases may exist
- UI patterns: MEDIUM - TileView documented, radial menu less standardized

**Research date:** 2026-01-21
**Valid until:** 2026-02-21 (30 days - stable patterns)
