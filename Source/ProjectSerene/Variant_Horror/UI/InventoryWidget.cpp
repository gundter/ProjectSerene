// Copyright Project Serene. All Rights Reserved.

#include "InventoryWidget.h"
#include "ItemSlotWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemDataAsset.h"
#include "GAS/SereneGameplayTags.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/TileView.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind tab buttons
	if (MedicalTabButton)
	{
		MedicalTabButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnMedicalTabClicked);
	}
	if (ToolsTabButton)
	{
		ToolsTabButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnToolsTabClicked);
	}
	if (EvidenceTabButton)
	{
		EvidenceTabButton->OnClicked.AddDynamic(this, &UInventoryWidget::OnEvidenceTabClicked);
	}
}

void UInventoryWidget::NativeDestruct()
{
	// Unbind from inventory changes
	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::OnInventoryChanged);
	}

	Super::NativeDestruct();
}

void UInventoryWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	// Remove old binding if any
	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnInventoryChanged.RemoveDynamic(this, &UInventoryWidget::OnInventoryChanged);
	}

	InventoryComponent = InInventoryComponent;

	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnInventoryChanged.AddDynamic(this, &UInventoryWidget::OnInventoryChanged);
	}

	RefreshInventory();
}

void UInventoryWidget::RefreshInventory()
{
	if (!InventoryComponent.IsValid()) return;

	// Populate each category TileView using namespace tags
	PopulateTileView(MedicalTileView, SereneGameplayTags::Item_Consumable_Medical);
	PopulateTileView(ToolsTileView, SereneGameplayTags::Item_Consumable_Tool);
	PopulateTileView(EvidenceTileView, SereneGameplayTags::Item_Evidence);
}

void UInventoryWidget::PopulateTileView(UTileView* TileView, FGameplayTag CategoryTag)
{
	if (!TileView || !InventoryComponent.IsValid()) return;

	TileView->ClearListItems();

	TArray<UItemDataAsset*> CategoryItems = InventoryComponent->GetItemsByCategory(CategoryTag);
	for (UItemDataAsset* Item : CategoryItems)
	{
		// TileView uses UObject* list items; ItemDataAsset can be the list item directly
		// Blueprint TileView EntryWidget (ItemSlotWidget) will receive this and call SetItemData
		TileView->AddItem(Item);
	}
}

void UInventoryWidget::SwitchToCategory(int32 CategoryIndex)
{
	CurrentCategoryIndex = FMath::Clamp(CategoryIndex, 0, 2);
	if (CategorySwitcher)
	{
		CategorySwitcher->SetActiveWidgetIndex(CurrentCategoryIndex);
	}
}

void UInventoryWidget::OnMedicalTabClicked()
{
	SwitchToCategory(0);
}

void UInventoryWidget::OnToolsTabClicked()
{
	SwitchToCategory(1);
}

void UInventoryWidget::OnEvidenceTabClicked()
{
	SwitchToCategory(2);
}

void UInventoryWidget::OnInventoryChanged(UItemDataAsset* Item, int32 NewQuantity)
{
	// Refresh when inventory contents change
	RefreshInventory();
}

void UInventoryWidget::OnItemSlotClicked(UItemDataAsset* Item)
{
	if (InventoryComponent.IsValid() && Item)
	{
		InventoryComponent->UseItem(Item);
		// Inventory changed delegate will trigger refresh
	}
}
