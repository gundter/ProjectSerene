// Copyright Project Serene. All Rights Reserved.

#include "Inventory/ItemDataAsset.h"

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	// Asset Manager uses this to categorize and manage items
	// Format: Item:DA_Bandage, Item:DA_Battery, etc.
	return FPrimaryAssetId(TEXT("Item"), GetFName());
}
