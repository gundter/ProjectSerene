// Copyright Project Serene. All Rights Reserved.

#include "SereneAbilitySystemComponent.h"

USereneAbilitySystemComponent::USereneAbilitySystemComponent()
	: bIsSinglePlayerMode(true)
{
	// Disable replication for single-player game
	// This provides performance benefits and avoids unnecessary network code
	SetIsReplicated(false);

	// Use minimal network mode for single-player
	ReplicationMode = EGameplayEffectReplicationMode::Minimal;
}

void USereneAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// Log initialization for debugging
	if (InOwnerActor && InAvatarActor)
	{
		UE_LOG(LogTemp, Log, TEXT("SereneASC: Initialized with Owner=%s, Avatar=%s"),
			*InOwnerActor->GetName(), *InAvatarActor->GetName());
	}
}
