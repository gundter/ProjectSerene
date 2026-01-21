// Copyright Project Serene. All Rights Reserved.

#include "Player/SerenePlayerState.h"
#include "GAS/SereneAbilitySystemComponent.h"
#include "GAS/SereneAttributeSet.h"
#include "GameplayEffect.h"
#include "ProjectSerene.h"

ASerenePlayerState::ASerenePlayerState()
{
	// Create the AbilitySystemComponent as a subobject
	AbilitySystemComponent = CreateDefaultSubobject<USereneAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	// Create the AttributeSet as a subobject
	// Must be created on the same actor that owns the ASC
	AttributeSet = CreateDefaultSubobject<USereneAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* ASerenePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASerenePlayerState::InitializeAttributes()
{
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("SerenePlayerState: Cannot initialize attributes - ASC is null"));
		return;
	}

	if (!DefaultAttributeEffect)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("SerenePlayerState: Cannot initialize attributes - DefaultAttributeEffect is not set. Assign GE_InitializeAttributes in Blueprint."));
		return;
	}

	// Create a context for the effect
	FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	// Create the spec handle for the initialization effect
	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, ContextHandle);

	if (SpecHandle.IsValid())
	{
		// Apply the instant effect to set initial attribute values
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
	else
	{
		UE_LOG(LogProjectSerene, Error, TEXT("SerenePlayerState: Failed to create spec handle for DefaultAttributeEffect"));
	}
}
