// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableTarget : public UInterface
{
    GENERATED_BODY()
};

/**
 * IInteractableTarget
 *
 * Interface for world objects the player can interact with via E key.
 * Implements the Lyra interaction pattern: line trace detection + interface methods.
 *
 * Usage:
 * - Implement on pickup actors, doors, containers
 * - OnFocused called when player looks at object (show highlight)
 * - OnUnfocused called when player looks away (hide highlight)
 * - OnInteract called when player presses E key
 */
class PROJECTSERENE_API IInteractableTarget
{
    GENERATED_BODY()

public:
    /**
     * Called when player looks at this object (enters focus).
     * Use to show outline highlight, enable custom depth.
     * @param InteractionInstigator The player pawn performing the interaction
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnFocused(AActor* InteractionInstigator);

    /**
     * Called when player looks away from this object (exits focus).
     * Use to hide outline highlight, disable custom depth.
     * @param InteractionInstigator The player pawn performing the interaction
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    void OnUnfocused(AActor* InteractionInstigator);

    /**
     * Called when player presses interact key (E) while focused.
     * Use to perform interaction (pickup item, open door, etc.)
     * @param InteractionInstigator The player pawn performing the interaction
     * @return True if interaction was successful
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    bool OnInteract(AActor* InteractionInstigator);

    /**
     * Get the text prompt to display when focused.
     * Example: "Press E to pick up Battery"
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
    FText GetInteractionPrompt() const;
};
