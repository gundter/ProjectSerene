// Copyright Project Serene. All Rights Reserved.

#include "SereneAbilitySystemComponent.h"

USereneAbilitySystemComponent::USereneAbilitySystemComponent()
	: bIsSinglePlayerMode(true)
{
	// Disable replication for single-player game
	// This provides performance benefits and avoids unnecessary network code
	SetIsReplicatedByDefault(false);

	// Use minimal network mode for single-player
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}

void USereneAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
}
