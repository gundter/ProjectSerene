// Copyright Project Serene. All Rights Reserved.

#include "RadialMenuWidget.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemDataAsset.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void URadialMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind slot button clicks
	if (Slot0Button) Slot0Button->OnClicked.AddDynamic(this, &URadialMenuWidget::OnSlot0Clicked);
	if (Slot1Button) Slot1Button->OnClicked.AddDynamic(this, &URadialMenuWidget::OnSlot1Clicked);
	if (Slot2Button) Slot2Button->OnClicked.AddDynamic(this, &URadialMenuWidget::OnSlot2Clicked);
	if (Slot3Button) Slot3Button->OnClicked.AddDynamic(this, &URadialMenuWidget::OnSlot3Clicked);

	// Start hidden
	SetVisibility(ESlateVisibility::Collapsed);
}

void URadialMenuWidget::SetInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;
	RefreshSlots();
}

void URadialMenuWidget::RefreshSlots()
{
	UpdateSlotDisplay(0, Slot0Button, Slot0Icon, Slot0Quantity);
	UpdateSlotDisplay(1, Slot1Button, Slot1Icon, Slot1Quantity);
	UpdateSlotDisplay(2, Slot2Button, Slot2Icon, Slot2Quantity);
	UpdateSlotDisplay(3, Slot3Button, Slot3Icon, Slot3Quantity);
}

void URadialMenuWidget::UpdateSlotDisplay(int32 SlotIndex, UButton* Button, UImage* Icon, UTextBlock* QuantityText)
{
	if (!InventoryComponent.IsValid()) return;

	UItemDataAsset* Item = InventoryComponent->GetQuickSlotItem(SlotIndex);

	if (Item)
	{
		// Show icon
		if (Icon && Item->Icon)
		{
			Icon->SetBrushFromTexture(Item->Icon);
			Icon->SetVisibility(ESlateVisibility::Visible);
		}

		// Show quantity
		if (QuantityText)
		{
			int32 Quantity = InventoryComponent->GetItemQuantity(Item);
			if (Quantity > 1)
			{
				QuantityText->SetText(FText::AsNumber(Quantity));
				QuantityText->SetVisibility(ESlateVisibility::Visible);
			}
			else
			{
				QuantityText->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		// Enable button
		if (Button)
		{
			Button->SetIsEnabled(true);
		}
	}
	else
	{
		// Empty slot
		if (Icon) Icon->SetVisibility(ESlateVisibility::Hidden);
		if (QuantityText) QuantityText->SetVisibility(ESlateVisibility::Hidden);
		if (Button) Button->SetIsEnabled(false);
	}
}

void URadialMenuWidget::ShowMenu()
{
	bIsVisible = true;
	RefreshSlots();
	SetVisibility(ESlateVisibility::Visible);
}

void URadialMenuWidget::HideMenu()
{
	bIsVisible = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void URadialMenuWidget::OnSlot0Clicked() { UseQuickSlot(0); }
void URadialMenuWidget::OnSlot1Clicked() { UseQuickSlot(1); }
void URadialMenuWidget::OnSlot2Clicked() { UseQuickSlot(2); }
void URadialMenuWidget::OnSlot3Clicked() { UseQuickSlot(3); }

void URadialMenuWidget::UseQuickSlot(int32 SlotIndex)
{
	if (InventoryComponent.IsValid())
	{
		bool bUsed = InventoryComponent->UseQuickSlot(SlotIndex);
		if (bUsed)
		{
			// Refresh to update quantity display
			RefreshSlots();
			// Auto-hide after use per CONTEXT.md (click to use, not hold)
			HideMenu();
		}
	}
}
