// Copyright Project Serene. All Rights Reserved.

#include "FlashlightComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/SereneAttributeSet.h"
#include "GAS/SereneGameplayTags.h"
#include "ProjectSerene.h"

UFlashlightComponent::UFlashlightComponent()
{
	// Component doesn't need to tick - we use GAS delegates for battery monitoring
	PrimaryComponentTick.bCanEverTick = false;
}

void UFlashlightComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache owner reference to avoid repeated GetOwner() calls
	CachedOwner = GetOwner();

	// Cache the ASC for faster access
	CachedASC = GetOwnerASC();

	// Bind to Battery attribute changes for auto-off when depleted
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			USereneAttributeSet::GetBatteryAttribute())
			.AddUObject(this, &UFlashlightComponent::OnBatteryChanged);
	}

	// Ensure flashlight starts off
	if (SpotLight.IsValid())
	{
		SetLightEnabled(false);
	}
}

void UFlashlightComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// Clean up battery drain effect
	StopBatteryDrain();

	// Remove flashlight tag if active
	if (CurrentState == EFlashlightState::On)
	{
		SetFlashlightTag(false);
	}

	Super::EndPlay(EndPlayReason);
}

void UFlashlightComponent::Initialize(USpotLightComponent* InSpotLight)
{
	if (!InSpotLight)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("FlashlightComponent::Initialize called with null SpotLight"));
		return;
	}

	SpotLight = InSpotLight;

	// Ensure light starts off (may be called before BeginPlay)
	SetLightEnabled(false);
}

void UFlashlightComponent::Toggle()
{
	if (CurrentState == EFlashlightState::Off)
	{
		// Check if we have battery to turn on
		if (UAbilitySystemComponent* ASC = CachedASC.Get())
		{
			const USereneAttributeSet* AttributeSet = ASC->GetSet<USereneAttributeSet>();
			if (AttributeSet && AttributeSet->GetBattery() <= 0.0f)
			{
				UE_LOG(LogProjectSerene, Verbose, TEXT("FlashlightComponent: Cannot turn on - battery depleted"));
				return;
			}
		}

		// Turn on
		TransitionToState(EFlashlightState::On);

		// Play toggle on sound
		if (ToggleOnSound && CachedOwner.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ToggleOnSound,
				CachedOwner->GetActorLocation()
			);
		}
	}
	else
	{
		// Turn off
		TransitionToState(EFlashlightState::Off);

		// Play toggle off sound
		if (ToggleOffSound && CachedOwner.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ToggleOffSound,
				CachedOwner->GetActorLocation()
			);
		}
	}
}

void UFlashlightComponent::TransitionToState(EFlashlightState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	// Exit current state
	switch (CurrentState)
	{
	case EFlashlightState::On:
		// Stop battery drain when turning off
		StopBatteryDrain();
		SetFlashlightTag(false);
		break;

	case EFlashlightState::Off:
	default:
		// Nothing to clean up when leaving Off state
		break;
	}

	// Set new state
	CurrentState = NewState;

	// Enter new state
	switch (CurrentState)
	{
	case EFlashlightState::On:
		// Enable light and start battery drain
		SetLightEnabled(true);
		StartBatteryDrain();
		SetFlashlightTag(true);
		break;

	case EFlashlightState::Off:
	default:
		// Disable light
		SetLightEnabled(false);
		break;
	}
}

void UFlashlightComponent::StartBatteryDrain()
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC || !BatteryDrainEffect)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("FlashlightComponent: Cannot start battery drain - missing ASC or BatteryDrainEffect"));
		return;
	}

	// Don't double-apply if already active
	if (DrainHandle.IsValid())
	{
		return;
	}

	// Apply the drain effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(CachedOwner.Get());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BatteryDrainEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		DrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UFlashlightComponent::StopBatteryDrain()
{
	if (!DrainHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ASC->RemoveActiveGameplayEffect(DrainHandle);
		DrainHandle.Invalidate();
	}
}

void UFlashlightComponent::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
	// Auto-off when battery depletes
	if (Data.NewValue <= 0.0f && CurrentState == EFlashlightState::On)
	{
		UE_LOG(LogProjectSerene, Log, TEXT("FlashlightComponent: Battery depleted - turning off flashlight"));
		TransitionToState(EFlashlightState::Off);

		// Play toggle off sound for battery depletion
		if (ToggleOffSound && CachedOwner.IsValid())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				ToggleOffSound,
				CachedOwner->GetActorLocation()
			);
		}
	}

	// Note: Flicker threshold (e.g., below 20%) will be added in Plan 02
}

void UFlashlightComponent::SetLightEnabled(bool bEnabled)
{
	if (!SpotLight.IsValid())
	{
		return;
	}

	SpotLight->SetVisibility(bEnabled);
}

UAbilitySystemComponent* UFlashlightComponent::GetOwnerASC() const
{
	AActor* Owner = CachedOwner.Get();
	if (!Owner)
	{
		Owner = GetOwner();
	}

	if (!Owner)
	{
		return nullptr;
	}

	// Try to get ASC via IAbilitySystemInterface
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		return ASI->GetAbilitySystemComponent();
	}

	return nullptr;
}

void UFlashlightComponent::SetFlashlightTag(bool bAdd)
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC)
	{
		return;
	}

	if (bAdd)
	{
		if (!ASC->HasMatchingGameplayTag(SereneGameplayTags::State_FlashlightOn))
		{
			ASC->AddLooseGameplayTag(SereneGameplayTags::State_FlashlightOn);
		}
	}
	else
	{
		if (ASC->HasMatchingGameplayTag(SereneGameplayTags::State_FlashlightOn))
		{
			ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_FlashlightOn);
		}
	}
}
