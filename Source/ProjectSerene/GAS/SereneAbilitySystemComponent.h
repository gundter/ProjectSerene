// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "SereneAbilitySystemComponent.generated.h"

/**
 * USereneAbilitySystemComponent
 *
 * Custom Ability System Component for Project Serene.
 * Configured for single-player (no replication) with single-player optimizations.
 *
 * This component should be owned by PlayerState (not Character) to persist
 * attributes across level transitions. The Character serves as the AvatarActor.
 */
UCLASS()
class PROJECTSERENE_API USereneAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	USereneAbilitySystemComponent();

	// ----------------------------------------
	// Initialization
	// ----------------------------------------

	/**
	 * Called when the ASC is initialized with its owner and avatar.
	 * Override to perform any custom initialization.
	 */
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

protected:
	// ----------------------------------------
	// Single-player optimizations
	// ----------------------------------------

	/**
	 * For single-player games, we can skip certain networked checks.
	 * This keeps the code ready for multiplayer if needed later.
	 */
	bool bIsSinglePlayerMode;
};
