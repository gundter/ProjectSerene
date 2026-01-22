// Copyright Project Serene. All Rights Reserved.

#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemDataAsset.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "ProjectSerene.h"

UInventoryComponent::UInventoryComponent()
{
	// No ticking needed - inventory is event-driven
	PrimaryComponentTick.bCanEverTick = false;

	// Initialize quick slots array with nullptrs
	QuickSlots.SetNum(NUM_QUICK_SLOTS);
	for (int32 i = 0; i < NUM_QUICK_SLOTS; ++i)
	{
		QuickSlots[i] = nullptr;
	}
}

// ----------------------------------------
// Core Operations
// ----------------------------------------

bool UInventoryComponent::AddItem(UItemDataAsset* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	// Check if we can add without exceeding stack limit
	if (!CanAddItem(Item, Quantity))
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Cannot add %d x %s - would exceed stack size %d"),
			Quantity, *Item->DisplayName.ToString(), Item->MaxStackSize);
		return false;
	}

	// Add to inventory
	int32& CurrentQuantity = Items.FindOrAdd(Item);
	CurrentQuantity += Quantity;

	// Broadcast change
	OnInventoryChanged.Broadcast(Item, CurrentQuantity);

	UE_LOG(LogProjectSerene, Log, TEXT("InventoryComponent: Added %d x %s (now have %d)"),
		Quantity, *Item->DisplayName.ToString(), CurrentQuantity);

	return true;
}

bool UInventoryComponent::RemoveItem(UItemDataAsset* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	int32* CurrentQuantityPtr = Items.Find(Item);
	if (!CurrentQuantityPtr || *CurrentQuantityPtr < Quantity)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Cannot remove %d x %s - only have %d"),
			Quantity, *Item->DisplayName.ToString(), CurrentQuantityPtr ? *CurrentQuantityPtr : 0);
		return false;
	}

	*CurrentQuantityPtr -= Quantity;
	int32 NewQuantity = *CurrentQuantityPtr;

	// Remove from map if quantity reached zero
	if (NewQuantity <= 0)
	{
		Items.Remove(Item);
		NewQuantity = 0;

		// Clear from quick slots if removed entirely
		for (int32 i = 0; i < NUM_QUICK_SLOTS; ++i)
		{
			if (QuickSlots[i] == Item)
			{
				QuickSlots[i] = nullptr;
			}
		}
	}

	// Broadcast change
	OnInventoryChanged.Broadcast(Item, NewQuantity);

	UE_LOG(LogProjectSerene, Log, TEXT("InventoryComponent: Removed %d x %s (now have %d)"),
		Quantity, *Item->DisplayName.ToString(), NewQuantity);

	return true;
}

bool UInventoryComponent::UseItem(UItemDataAsset* Item)
{
	if (!Item)
	{
		return false;
	}

	// Check we have the item
	if (GetItemQuantity(Item) <= 0)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Cannot use %s - not in inventory"),
			*Item->DisplayName.ToString());
		return false;
	}

	// Check item has a use effect
	if (!Item->UseEffect)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Cannot use %s - no UseEffect defined"),
			*Item->DisplayName.ToString());
		return false;
	}

	// Get ASC to apply effect
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Cannot use %s - no ASC available"),
			*Item->DisplayName.ToString());
		return false;
	}

	// Apply the item's Gameplay Effect
	// Pattern from SerenePlayerState::InitializeAttributes
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetOwner());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Item->UseEffect, 1, ContextHandle);
	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogProjectSerene, Error, TEXT("InventoryComponent: Failed to create spec for %s UseEffect"),
			*Item->DisplayName.ToString());
		return false;
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

	// Remove one from inventory
	RemoveItem(Item, 1);

	UE_LOG(LogProjectSerene, Log, TEXT("InventoryComponent: Used %s - effect applied"),
		*Item->DisplayName.ToString());

	return true;
}

bool UInventoryComponent::CanAddItem(UItemDataAsset* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

	int32 CurrentQuantity = GetItemQuantity(Item);
	return (CurrentQuantity + Quantity) <= Item->MaxStackSize;
}

// ----------------------------------------
// Query Operations
// ----------------------------------------

int32 UInventoryComponent::GetItemQuantity(UItemDataAsset* Item) const
{
	if (!Item)
	{
		return 0;
	}

	const int32* QuantityPtr = Items.Find(Item);
	return QuantityPtr ? *QuantityPtr : 0;
}

TArray<UItemDataAsset*> UInventoryComponent::GetAllItems() const
{
	TArray<UItemDataAsset*> Result;
	Result.Reserve(Items.Num());

	for (const auto& Pair : Items)
	{
		if (Pair.Key)
		{
			Result.Add(Pair.Key);
		}
	}

	return Result;
}

TArray<UItemDataAsset*> UInventoryComponent::GetItemsByCategory(FGameplayTag Category) const
{
	TArray<UItemDataAsset*> Result;

	for (const auto& Pair : Items)
	{
		UItemDataAsset* Item = Pair.Key;
		if (Item && Item->ItemCategory.MatchesTag(Category))
		{
			Result.Add(Item);
		}
	}

	return Result;
}

// ----------------------------------------
// Quick Slot Operations
// ----------------------------------------

void UInventoryComponent::AssignQuickSlot(int32 SlotIndex, UItemDataAsset* Item)
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("InventoryComponent: Invalid quick slot index %d"), SlotIndex);
		return;
	}

	QuickSlots[SlotIndex] = Item;

	UE_LOG(LogProjectSerene, Log, TEXT("InventoryComponent: Quick slot %d assigned to %s"),
		SlotIndex, Item ? *Item->DisplayName.ToString() : TEXT("(empty)"));
}

UItemDataAsset* UInventoryComponent::GetQuickSlotItem(int32 SlotIndex) const
{
	if (!IsValidSlotIndex(SlotIndex))
	{
		return nullptr;
	}

	return QuickSlots[SlotIndex];
}

bool UInventoryComponent::UseQuickSlot(int32 SlotIndex)
{
	UItemDataAsset* Item = GetQuickSlotItem(SlotIndex);
	if (!Item)
	{
		UE_LOG(LogProjectSerene, Log, TEXT("InventoryComponent: Quick slot %d is empty"), SlotIndex);
		return false;
	}

	return UseItem(Item);
}

// ----------------------------------------
// Helper Methods
// ----------------------------------------

UAbilitySystemComponent* UInventoryComponent::GetOwnerASC()
{
	// Return cached ASC if still valid
	if (CachedASC.IsValid())
	{
		return CachedASC.Get();
	}

	// Get ASC from owner via IAbilitySystemInterface
	// Owner should be SerenePlayerState which implements this interface
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Owner);
	if (ASCInterface)
	{
		CachedASC = ASCInterface->GetAbilitySystemComponent();
		return CachedASC.Get();
	}

	return nullptr;
}

bool UInventoryComponent::IsValidSlotIndex(int32 SlotIndex) const
{
	return SlotIndex >= 0 && SlotIndex < NUM_QUICK_SLOTS;
}
