// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemDataAsset.generated.h"

class UGameplayEffect;
class UTexture2D;
class UStaticMesh;

/**
 * UItemDataAsset
 *
 * Data asset for defining item properties in Project Serene.
 * Items are defined as Primary Data Assets for easy editor management
 * and Asset Manager integration.
 *
 * Usage:
 * - Create child data assets in Editor (e.g., DA_Bandage, DA_Battery)
 * - Set display properties, category tag, and use effect
 * - InventoryComponent stores and manages these items
 * - UseEffect is applied via GAS when item is consumed
 *
 * Item categories (via ItemCategory tag):
 * - Item.Consumable.Medical: Health restoration (bandage, medkit, medication)
 * - Item.Consumable.Tool: Resource restoration (battery, energy drink)
 * - Item.Evidence: Investigation items (Phase 5)
 */
UCLASS(BlueprintType)
class PROJECTSERENE_API UItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ----------------------------------------
	// Display Properties
	// ----------------------------------------

	/** Display name shown in UI and inventory */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	FText DisplayName;

	/** Item description shown in details panel */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display", meta = (MultiLine = true))
	FText Description;

	/** Icon texture for UI display */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Display")
	TObjectPtr<UTexture2D> Icon;

	// ----------------------------------------
	// Categorization
	// ----------------------------------------

	/**
	 * Category tag for this item type.
	 * Use Item.Consumable.Medical, Item.Consumable.Tool, Item.Evidence, etc.
	 * Enables filtering items by category in InventoryComponent.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTag ItemCategory;

	// ----------------------------------------
	// Stacking
	// ----------------------------------------

	/**
	 * Maximum stack size for this item.
	 * Common items: 5 (bandages, batteries)
	 * Rare items: 2 (medkits, medication)
	 * Evidence: 1 (unique items)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1", ClampMax = "99"))
	int32 MaxStackSize = 5;

	// ----------------------------------------
	// Consumable Properties
	// ----------------------------------------

	/**
	 * Gameplay Effect applied when item is used.
	 * Should be an Instant effect that modifies attributes
	 * (e.g., GE_UseBandage: +25 Health, GE_UseBattery: +50 Battery)
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Consumable")
	TSubclassOf<UGameplayEffect> UseEffect;

	// ----------------------------------------
	// World Pickup Properties
	// ----------------------------------------

	/** Static mesh for world pickup representation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Pickup")
	TObjectPtr<UStaticMesh> PickupMesh;

	// ----------------------------------------
	// Asset Manager Integration
	// ----------------------------------------

	/**
	 * Returns the Primary Asset ID for Asset Manager registration.
	 * Format: Item:AssetName
	 */
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
