// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "InventoryWidget.generated.h"

class UInventoryComponent;
class UItemDataAsset;
class UItemSlotWidget;
class UTileView;
class UButton;
class UWidgetSwitcher;

/**
 * UInventoryWidget
 *
 * Main inventory panel with categorized tabs: Medical, Tools, Evidence.
 * Per CONTEXT.md: Tab key toggles, game pauses when open.
 *
 * Features:
 * - Three category tabs via WidgetSwitcher
 * - TileView for item grid display
 * - Click item to use
 * - Binds to InventoryComponent::OnInventoryChanged
 *
 * Blueprint setup required:
 * - Create WidgetSwitcher (CategorySwitcher) with 3 pages
 * - Create 3 TileViews (MedicalTileView, ToolsTileView, EvidenceTileView)
 * - Create 3 tab Buttons (MedicalTabButton, ToolsTabButton, EvidenceTabButton)
 * - Configure TileView Entry Widget Class to ItemSlotWidget subclass
 */
UCLASS()
class PROJECTSERENE_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Initialize with inventory component reference */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SetInventoryComponent(UInventoryComponent* InInventoryComponent);

	/** Refresh all item displays */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void RefreshInventory();

	/** Switch to specific category tab */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	void SwitchToCategory(int32 CategoryIndex);

	/** Get current category index */
	UFUNCTION(BlueprintCallable, Category = "Inventory|UI")
	int32 GetCurrentCategoryIndex() const { return CurrentCategoryIndex; }

protected:
	// ----------------------------------------
	// Widget Bindings
	// ----------------------------------------

	/** Widget switcher for category panels */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> CategorySwitcher;

	/** Tab buttons */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MedicalTabButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ToolsTabButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EvidenceTabButton;

	/** TileViews for each category */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> MedicalTileView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> ToolsTileView;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> EvidenceTileView;

	// ----------------------------------------
	// Configuration
	// ----------------------------------------

	/** Item slot widget class for TileView entries */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory|UI")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass;

	// ----------------------------------------
	// State
	// ----------------------------------------

	UPROPERTY()
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

	int32 CurrentCategoryIndex = 0;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnMedicalTabClicked();

	UFUNCTION()
	void OnToolsTabClicked();

	UFUNCTION()
	void OnEvidenceTabClicked();

	UFUNCTION()
	void OnInventoryChanged(UItemDataAsset* Item, int32 NewQuantity);

	/** Handle item slot clicked - uses item via InventoryComponent */
	UFUNCTION()
	void OnItemSlotClicked(UItemDataAsset* Item);

	/** Populate a TileView with items matching category tag */
	void PopulateTileView(UTileView* TileView, FGameplayTag CategoryTag);
};
