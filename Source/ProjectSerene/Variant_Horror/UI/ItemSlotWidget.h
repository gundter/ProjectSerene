// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

class UItemDataAsset;
class UImage;
class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, UItemDataAsset*, Item);

/**
 * UItemSlotWidget
 *
 * Widget displaying a single inventory item with icon, quantity, and click interaction.
 * Used within InventoryWidget's TileView for item display.
 *
 * Features:
 * - Item icon display
 * - Quantity badge (hidden if quantity = 1)
 * - Click to use item
 * - Tooltip showing name and description
 *
 * Blueprint setup required:
 * - Create UMG widget with Image (IconImage), TextBlock (QuantityText), Button (SlotButton)
 * - Bind widgets using BindWidget meta specifier names
 */
UCLASS()
class PROJECTSERENE_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Configure this slot with item data */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetItemData(UItemDataAsset* Item, int32 Quantity);

	/** Clear this slot */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void ClearSlot();

	/** Get the currently displayed item */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	UItemDataAsset* GetItem() const { return CurrentItem; }

	/** Get the current quantity displayed */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	int32 GetQuantity() const { return CurrentQuantity; }

	/** Delegate fired when slot is clicked */
	UPROPERTY(BlueprintAssignable, Category = "Inventory|UI")
	FOnItemSlotClicked OnItemSlotClicked;

protected:
	// ----------------------------------------
	// Widget Bindings (Blueprint must create these)
	// ----------------------------------------

	/** Icon image widget - bind to Image in Blueprint */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	/** Quantity text widget - bind to TextBlock in Blueprint */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> QuantityText;

	/** Button for click interaction - bind to Button in Blueprint */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton;

	// ----------------------------------------
	// State
	// ----------------------------------------

	UPROPERTY()
	TObjectPtr<UItemDataAsset> CurrentItem;

	int32 CurrentQuantity = 0;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void HandleButtonClicked();
};
