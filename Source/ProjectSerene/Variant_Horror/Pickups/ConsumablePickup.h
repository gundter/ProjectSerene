// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inventory/InteractableInterface.h"
#include "ConsumablePickup.generated.h"

class UItemDataAsset;
class UStaticMeshComponent;
class USoundBase;

/**
 * AConsumablePickup
 *
 * World pickup actor for consumable items. Implements IInteractableTarget
 * for line trace interaction. When interacted, adds item to player inventory
 * and destroys self.
 *
 * Features:
 * - Custom depth highlight on focus (per CONTEXT.md)
 * - Interaction prompt showing item name
 * - Stack check before pickup
 */
UCLASS()
class PROJECTSERENE_API AConsumablePickup : public AActor, public IInteractableTarget
{
    GENERATED_BODY()

public:
    AConsumablePickup();

    // ----------------------------------------
    // IInteractableTarget
    // ----------------------------------------

    virtual void OnFocused_Implementation(AActor* InteractionInstigator) override;
    virtual void OnUnfocused_Implementation(AActor* InteractionInstigator) override;
    virtual bool OnInteract_Implementation(AActor* InteractionInstigator) override;
    virtual FText GetInteractionPrompt_Implementation() const override;

protected:
    // ----------------------------------------
    // Components
    // ----------------------------------------

    /** Mesh component for visual representation */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UStaticMeshComponent> MeshComponent;

    // ----------------------------------------
    // Configuration
    // ----------------------------------------

    /** The item data asset this pickup represents */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
    TObjectPtr<UItemDataAsset> ItemData;

    /** Quantity to add when picked up */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (ClampMin = "1"))
    int32 Quantity = 1;

    /** Custom depth stencil value for outline highlight */
    UPROPERTY(EditDefaultsOnly, Category = "Pickup|Highlight")
    int32 HighlightStencilValue = 255;

    // ----------------------------------------
    // Audio
    // ----------------------------------------

    /** Sound to play when item is picked up */
    UPROPERTY(EditDefaultsOnly, Category = "Pickup|Audio")
    TObjectPtr<USoundBase> PickupSound;

    virtual void BeginPlay() override;

private:
    /** Set mesh from ItemData if available */
    void UpdateMeshFromItemData();
};
