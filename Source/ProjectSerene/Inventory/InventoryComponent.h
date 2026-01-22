// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventoryComponent.generated.h"

class UItemDataAsset;
class UAbilitySystemComponent;

/**
 * Delegate broadcast when inventory contents change.
 * @param Item - The item that changed
 * @param NewQuantity - The new quantity (0 means item removed)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryChanged, UItemDataAsset*, Item, int32, NewQuantity);

/**
 * UInventoryComponent
 *
 * Manages the player's inventory of items in Project Serene.
 * Attached to SerenePlayerState to persist across respawns (like ASC).
 *
 * Features:
 * - TMap-based storage mapping ItemDataAsset to quantity
 * - Stack size limits per item type (MaxStackSize on ItemDataAsset)
 * - GAS integration: UseItem applies the item's UseEffect via ASC
 * - 4 quick slots for fast consumable access
 * - Category filtering via GameplayTag hierarchy
 * - OnInventoryChanged delegate for UI binding
 *
 * Design decisions:
 * - Owned by PlayerState (follows ASC ownership pattern)
 * - UPROPERTY on TMap prevents garbage collection issues
 * - WeakObjectPtr for ASC cache to avoid dangling references
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class PROJECTSERENE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	// ----------------------------------------
	// Core Operations
	// ----------------------------------------

	/**
	 * Adds an item to the inventory.
	 * @param Item - The item data asset to add
	 * @param Quantity - Number to add (default 1)
	 * @return True if all items were added, false if inventory full or invalid item
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemDataAsset* Item, int32 Quantity = 1);

	/**
	 * Removes an item from the inventory.
	 * @param Item - The item data asset to remove
	 * @param Quantity - Number to remove (default 1)
	 * @return True if items were removed, false if not enough in inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemDataAsset* Item, int32 Quantity = 1);

	/**
	 * Uses an item: applies its UseEffect via GAS and decrements quantity.
	 * @param Item - The item to use
	 * @return True if item was used, false if not in inventory or no effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool UseItem(UItemDataAsset* Item);

	/**
	 * Checks if the specified quantity can be added without exceeding stack limit.
	 * @param Item - The item to check
	 * @param Quantity - Number to potentially add
	 * @return True if can add, false if would exceed MaxStackSize
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool CanAddItem(UItemDataAsset* Item, int32 Quantity = 1) const;

	// ----------------------------------------
	// Query Operations
	// ----------------------------------------

	/**
	 * Gets the current quantity of an item in inventory.
	 * @param Item - The item to query
	 * @return Current quantity (0 if not in inventory)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantity(UItemDataAsset* Item) const;

	/**
	 * Gets all items currently in inventory.
	 * @return Array of all item data assets (no duplicates)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<UItemDataAsset*> GetAllItems() const;

	/**
	 * Gets all items matching a category tag (supports hierarchy).
	 * Example: GetItemsByCategory(Item.Consumable) returns all consumables.
	 * @param Category - The category tag to filter by
	 * @return Array of matching item data assets
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	TArray<UItemDataAsset*> GetItemsByCategory(FGameplayTag Category) const;

	// ----------------------------------------
	// Quick Slot Operations
	// ----------------------------------------

	/**
	 * Assigns an item to a quick slot for fast access.
	 * @param SlotIndex - Slot index (0-3)
	 * @param Item - The item to assign (nullptr to clear)
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlots")
	void AssignQuickSlot(int32 SlotIndex, UItemDataAsset* Item);

	/**
	 * Gets the item assigned to a quick slot.
	 * @param SlotIndex - Slot index (0-3)
	 * @return The assigned item, or nullptr if empty/invalid slot
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlots")
	UItemDataAsset* GetQuickSlotItem(int32 SlotIndex) const;

	/**
	 * Uses the item in a quick slot.
	 * @param SlotIndex - Slot index (0-3)
	 * @return True if item was used, false if slot empty or item not in inventory
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|QuickSlots")
	bool UseQuickSlot(int32 SlotIndex);

	// ----------------------------------------
	// Delegate
	// ----------------------------------------

	/**
	 * Broadcast when inventory contents change.
	 * UI binds to this for updating inventory display.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

protected:
	// ----------------------------------------
	// Storage
	// ----------------------------------------

	/**
	 * Item storage: maps item data asset to current quantity.
	 * UPROPERTY ensures proper garbage collection (see RESEARCH.md Pitfall 2).
	 */
	UPROPERTY()
	TMap<TObjectPtr<UItemDataAsset>, int32> Items;

	/**
	 * Quick slots array (fixed size 4 per CONTEXT.md).
	 * Stores references to items for fast access via number keys.
	 */
	UPROPERTY()
	TArray<TObjectPtr<UItemDataAsset>> QuickSlots;

	/** Number of quick slots available */
	static const int32 NUM_QUICK_SLOTS = 4;

	// ----------------------------------------
	// Cached References
	// ----------------------------------------

	/** Cached ASC for applying item effects */
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	// ----------------------------------------
	// Helper Methods
	// ----------------------------------------

	/**
	 * Gets the Ability System Component from the owning PlayerState.
	 * Caches the result for performance.
	 */
	UAbilitySystemComponent* GetOwnerASC();

	/**
	 * Validates a quick slot index.
	 * @param SlotIndex - The index to validate
	 * @return True if valid (0 to NUM_QUICK_SLOTS-1)
	 */
	bool IsValidSlotIndex(int32 SlotIndex) const;
};
