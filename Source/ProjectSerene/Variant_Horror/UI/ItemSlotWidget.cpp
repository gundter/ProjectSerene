// Copyright Project Serene. All Rights Reserved.

#include "ItemSlotWidget.h"
#include "Inventory/ItemDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.AddDynamic(this, &UItemSlotWidget::HandleButtonClicked);
	}
}

void UItemSlotWidget::SetItemData(UItemDataAsset* Item, int32 Quantity)
{
	CurrentItem = Item;
	CurrentQuantity = Quantity;

	if (Item)
	{
		// Set icon
		if (IconImage && Item->Icon)
		{
			IconImage->SetBrushFromTexture(Item->Icon);
			IconImage->SetVisibility(ESlateVisibility::Visible);
		}

		// Set quantity (hide if 1)
		if (QuantityText)
		{
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

		// Set tooltip (per CONTEXT.md: name, quantity, effect description)
		FText TooltipText = FText::Format(
			NSLOCTEXT("Inventory", "ItemTooltip", "{0}\n{1}"),
			Item->DisplayName,
			Item->Description
		);
		SetToolTipText(TooltipText);
	}
	else
	{
		ClearSlot();
	}
}

void UItemSlotWidget::ClearSlot()
{
	CurrentItem = nullptr;
	CurrentQuantity = 0;

	if (IconImage)
	{
		IconImage->SetVisibility(ESlateVisibility::Hidden);
	}
	if (QuantityText)
	{
		QuantityText->SetVisibility(ESlateVisibility::Hidden);
	}
	SetToolTipText(FText::GetEmpty());
}

void UItemSlotWidget::HandleButtonClicked()
{
	if (CurrentItem)
	{
		OnItemSlotClicked.Broadcast(CurrentItem);
	}
}

FReply UItemSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// Detect right-click for quick slot assignment
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (CurrentItem)
		{
			OnItemSlotRightClicked.Broadcast(CurrentItem);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
