// Copyright Project Serene. All Rights Reserved.

#include "Variant_Horror/Pickups/ConsumablePickup.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/ItemDataAsset.h"
#include "Inventory/InventoryComponent.h"
#include "Player/SerenePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

AConsumablePickup::AConsumablePickup()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // Enable custom depth for outline rendering (per RESEARCH.md)
    MeshComponent->SetRenderCustomDepth(false);
    MeshComponent->SetCustomDepthStencilValue(HighlightStencilValue);
}

void AConsumablePickup::BeginPlay()
{
    Super::BeginPlay();
    UpdateMeshFromItemData();
}

void AConsumablePickup::UpdateMeshFromItemData()
{
    if (ItemData && ItemData->PickupMesh)
    {
        MeshComponent->SetStaticMesh(ItemData->PickupMesh);
    }
}

void AConsumablePickup::OnFocused_Implementation(AActor* InteractionInstigator)
{
    // Enable outline highlight via custom depth
    if (MeshComponent)
    {
        MeshComponent->SetRenderCustomDepth(true);
    }
}

void AConsumablePickup::OnUnfocused_Implementation(AActor* InteractionInstigator)
{
    if (MeshComponent)
    {
        MeshComponent->SetRenderCustomDepth(false);
    }
}

bool AConsumablePickup::OnInteract_Implementation(AActor* InteractionInstigator)
{
    if (!ItemData)
    {
        return false;
    }

    // Get inventory from instigator's PlayerState
    APawn* Pawn = Cast<APawn>(InteractionInstigator);
    if (!Pawn)
    {
        return false;
    }

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC)
    {
        return false;
    }

    ASerenePlayerState* PS = PC->GetPlayerState<ASerenePlayerState>();
    if (!PS)
    {
        return false;
    }

    UInventoryComponent* Inventory = PS->GetInventoryComponent();
    if (!Inventory)
    {
        return false;
    }

    // Check if can add (stack limit check)
    if (!Inventory->CanAddItem(ItemData, Quantity))
    {
        // "Inventory full" - item stays in world per CONTEXT.md
        // Could broadcast event for UI message here
        return false;
    }

    // Add to inventory
    if (Inventory->AddItem(ItemData, Quantity))
    {
        // Play pickup sound if configured
        if (PickupSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
        }

        // Destroy pickup
        Destroy();
        return true;
    }

    return false;
}

FText AConsumablePickup::GetInteractionPrompt_Implementation() const
{
    if (ItemData)
    {
        return FText::Format(
            NSLOCTEXT("Interaction", "PickupPrompt", "Press E to pick up {0}"),
            ItemData->DisplayName
        );
    }
    return NSLOCTEXT("Interaction", "PickupPromptGeneric", "Press E to pick up");
}
