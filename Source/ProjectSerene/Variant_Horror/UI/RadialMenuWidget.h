// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialMenuWidget.generated.h"

class UInventoryComponent;
class UItemDataAsset;
class UImage;
class UTextBlock;
class UButton;

/**
 * URadialMenuWidget
 *
 * Quick-select radial menu with 4 assignable slots.
 * Per CONTEXT.md: Tab to toggle open, click to use (not hold-and-release).
 * Does NOT pause game - allows tactical use during gameplay.
 *
 * Layout: 4 quadrant buttons arranged radially around center.
 * - Slot 0: Top
 * - Slot 1: Right
 * - Slot 2: Bottom
 * - Slot 3: Left
 */
UCLASS()
class PROJECTSERENE_API URadialMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Set the inventory component reference for this widget */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	/** Refresh slot displays from inventory quick slots */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void RefreshSlots();

	/** Show the radial menu */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void ShowMenu();

	/** Hide the radial menu */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void HideMenu();

	/** Check if menu is currently visible */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	bool IsMenuVisible() const { return bIsVisible; }

protected:
	// ----------------------------------------
	// Widget Bindings (4 slots)
	// ----------------------------------------

	/** Slot buttons - arranged in quadrants */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot0Button;  // Top

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot1Button;  // Right

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot2Button;  // Bottom

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot3Button;  // Left

	/** Slot icons */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Slot0Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Slot1Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Slot2Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Slot3Icon;

	/** Slot quantity text (optional) */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Slot0Quantity;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Slot1Quantity;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Slot2Quantity;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> Slot3Quantity;

	// ----------------------------------------
	// State
	// ----------------------------------------

	UPROPERTY()
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

	bool bIsVisible = false;

	virtual void NativeConstruct() override;

	/** Update a single slot's display */
	void UpdateSlotDisplay(int32 SlotIndex, UButton* Button, UImage* Icon, UTextBlock* QuantityText);

	/** Slot click handlers */
	UFUNCTION()
	void OnSlot0Clicked();

	UFUNCTION()
	void OnSlot1Clicked();

	UFUNCTION()
	void OnSlot2Clicked();

	UFUNCTION()
	void OnSlot3Clicked();

	/** Use the item in specified quick slot */
	void UseQuickSlot(int32 SlotIndex);
};
