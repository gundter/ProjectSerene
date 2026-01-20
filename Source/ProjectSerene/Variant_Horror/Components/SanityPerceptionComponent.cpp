// Copyright Project Serene. All Rights Reserved.

#include "SanityPerceptionComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PointLight.h"
#include "Engine/SpotLight.h"
#include "Engine/Light.h"
#include "Camera/CameraComponent.h"
#include "GameplayEffectTypes.h"
#include "GAS/SereneAttributeSet.h"
#include "Variant_Horror/HorrorCharacter.h"

USanityPerceptionComponent::USanityPerceptionComponent()
{
	// Component doesn't need to tick - we use timers instead for better performance
	PrimaryComponentTick.bCanEverTick = false;
}

void USanityPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache all protective light actors in the level
	CacheLightActors();

	// Start periodic light proximity check timer
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			LightCheckTimer,
			this,
			&USanityPerceptionComponent::CheckLightProximity,
			LightCheckInterval,
			true  // Looping
		);

		UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Started light check timer (%.2fs interval), cached %d lights"),
			LightCheckInterval, LightActors.Num());
	}

	// Set up sanity listener for visual effects
	SetupSanityListener();

	// Initialize post-process settings on the camera
	InitializePostProcessSettings();
}

void USanityPerceptionComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// Clear all timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(LightCheckTimer);
		World->GetTimerManager().ClearTimer(GracePeriodTimer);
	}

	// Clean up any active effects
	StopSanityDrain();
	StopSanityRegen();

	Super::EndPlay(EndPlayReason);
}

void USanityPerceptionComponent::CacheLightActors()
{
	LightActors.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// Get all light actors (Point and Spot lights)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, ALight::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		// Filter out flickering lights - they provide no sanity protection
		if (Actor->ActorHasTag(FlickeringLightTag))
		{
			continue;
		}

		// Only include Point and Spot lights (not directional/ambient)
		if (Cast<APointLight>(Actor) || Cast<ASpotLight>(Actor))
		{
			LightActors.Add(Actor);
		}
	}

	// Also find any actors tagged as protective light sources (windows, moonlight zones)
	TArray<AActor*> TaggedLightActors;
	UGameplayStatics::GetAllActorsWithTag(World, ProtectiveLightTag, TaggedLightActors);

	for (AActor* Actor : TaggedLightActors)
	{
		// Don't add duplicates (some lights might already be in the list)
		bool bAlreadyAdded = false;
		for (const TWeakObjectPtr<AActor>& Existing : LightActors)
		{
			if (Existing.Get() == Actor)
			{
				bAlreadyAdded = true;
				break;
			}
		}

		if (!bAlreadyAdded)
		{
			LightActors.Add(Actor);
		}
	}

	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Cached %d protective light sources"), LightActors.Num());
}

void USanityPerceptionComponent::CheckLightProximity()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FVector PlayerLocation = Owner->GetActorLocation();
	bool bFoundLightInRange = false;

	// Check distance to each cached light actor
	for (int32 i = LightActors.Num() - 1; i >= 0; --i)
	{
		AActor* LightActor = LightActors[i].Get();

		// Remove invalid (destroyed) lights from cache
		if (!LightActor)
		{
			LightActors.RemoveAt(i);
			continue;
		}

		const float Distance = FVector::Dist(PlayerLocation, LightActor->GetActorLocation());
		if (Distance <= LightDetectionRadius)
		{
			bFoundLightInRange = true;
			break;  // Only need one light in range
		}
	}

	// Handle state transitions
	if (bFoundLightInRange)
	{
		if (!bInLight)
		{
			OnEnterLight();
		}
	}
	else
	{
		if (bInLight && !bInGracePeriod)
		{
			OnExitLight();
		}
	}
}

void USanityPerceptionComponent::OnEnterLight()
{
	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Entered light zone"));

	bInLight = true;

	// Cancel grace period if running (player returned to light before grace ended)
	if (bInGracePeriod)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(GracePeriodTimer);
		}
		bInGracePeriod = false;
	}

	// Stop sanity drain and start regeneration
	StopSanityDrain();
	StartSanityRegen();
}

void USanityPerceptionComponent::OnExitLight()
{
	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Exited light zone, starting %.1fs grace period"), GracePeriodDuration);

	// Start grace period - don't change bInLight yet
	bInGracePeriod = true;

	// Stop sanity regen immediately when leaving light
	StopSanityRegen();

	// Start grace period timer before drain begins
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			GracePeriodTimer,
			this,
			&USanityPerceptionComponent::OnGracePeriodEnded,
			GracePeriodDuration,
			false  // Not looping
		);
	}
}

void USanityPerceptionComponent::OnGracePeriodEnded()
{
	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Grace period ended, starting sanity drain"));

	bInGracePeriod = false;
	bInLight = false;

	// Start sanity drain
	StartSanityDrain();
}

void USanityPerceptionComponent::StartSanityDrain()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !SanityDrainEffect)
	{
		return;
	}

	// Don't double-apply if already active
	if (DrainHandle.IsValid())
	{
		return;
	}

	// Apply the drain effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetOwner());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SanityDrainEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		DrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity drain effect applied"));
	}
}

void USanityPerceptionComponent::StopSanityDrain()
{
	if (!DrainHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetOwnerASC())
	{
		ASC->RemoveActiveGameplayEffect(DrainHandle);
		DrainHandle.Invalidate();
		UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity drain effect removed"));
	}
}

void USanityPerceptionComponent::StartSanityRegen()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC || !SanityRegenEffect)
	{
		return;
	}

	// Don't double-apply if already active
	if (RegenHandle.IsValid())
	{
		return;
	}

	// Apply the regen effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(GetOwner());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(SanityRegenEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		RegenHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity regen effect applied"));
	}
}

void USanityPerceptionComponent::StopSanityRegen()
{
	if (!RegenHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetOwnerASC())
	{
		ASC->RemoveActiveGameplayEffect(RegenHandle);
		RegenHandle.Invalidate();
		UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity regen effect removed"));
	}
}

UAbilitySystemComponent* USanityPerceptionComponent::GetOwnerASC() const
{
	AActor* Owner = GetOwner();
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

void USanityPerceptionComponent::RefreshLightCache()
{
	CacheLightActors();
	UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Light cache refreshed, %d lights cached"), LightActors.Num());
}

// ----------------------------------------
// Visual Distortion Methods
// ----------------------------------------

void USanityPerceptionComponent::SetupSanityListener()
{
	UAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC)
	{
		return;
	}

	ASC->GetGameplayAttributeValueChangeDelegate(
		USereneAttributeSet::GetSanityAttribute())
		.AddUObject(this, &USanityPerceptionComponent::OnSanityChanged);

	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity change listener registered"));
}

void USanityPerceptionComponent::OnSanityChanged(const FOnAttributeChangeData& Data)
{
	// Get max sanity for percentage calculation
	const USereneAttributeSet* AttributeSet = nullptr;
	if (UAbilitySystemComponent* ASC = GetOwnerASC())
	{
		AttributeSet = ASC->GetSet<USereneAttributeSet>();
	}

	float MaxSanity = AttributeSet ? AttributeSet->GetMaxSanity() : 100.0f;
	float SanityPercent = MaxSanity > 0.0f ? Data.NewValue / MaxSanity : 0.0f;

	// Update visual effects
	UpdateVisualDistortion(SanityPercent);

	// Check if we should stop regen (reached 80% cap)
	CheckRegenCap(Data.NewValue, MaxSanity);
}

void USanityPerceptionComponent::UpdateVisualDistortion(float SanityPercent)
{
	UCameraComponent* Camera = CachedCamera.Get();
	if (!Camera)
	{
		return;
	}

	// Calculate distortion scale (inverse of sanity - lower sanity = higher effects)
	// Clamp sanity to minimum 5% for calculation
	float ClampedSanity = FMath::Clamp(SanityPercent, 0.05f, 1.0f);
	float DistortionScale = 1.0f - ClampedSanity;

	// Apply effects with linear interpolation
	Camera->PostProcessSettings.VignetteIntensity =
		FMath::Lerp(MinVignetteIntensity, MaxVignetteIntensity, DistortionScale);

	Camera->PostProcessSettings.FilmGrainIntensity =
		FMath::Lerp(0.0f, MaxGrainIntensity, DistortionScale);

	Camera->PostProcessSettings.SceneFringeIntensity =
		FMath::Lerp(0.0f, MaxChromaticAberration, DistortionScale);

	// Color desaturation (cold/blue tint at low sanity)
	float Saturation = FMath::Lerp(1.0f, MinColorSaturation, DistortionScale);
	Camera->PostProcessSettings.ColorSaturation = FVector4(Saturation, Saturation, Saturation, 1.0f);

	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Visual distortion updated - Sanity: %.1f%%, Distortion: %.2f"),
		SanityPercent * 100.0f, DistortionScale);
}

void USanityPerceptionComponent::InitializePostProcessSettings()
{
	AHorrorCharacter* Character = Cast<AHorrorCharacter>(GetOwner());
	if (!Character)
	{
		return;
	}

	UCameraComponent* Camera = Character->GetFirstPersonCameraComponent();
	if (!Camera)
	{
		return;
	}

	// Cache camera reference
	CachedCamera = Camera;

	// Enable camera post-process
	Camera->PostProcessBlendWeight = 1.0f;

	// Set all override flags for properties we'll modify
	Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
	Camera->PostProcessSettings.bOverride_FilmGrainIntensity = true;
	Camera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
	Camera->PostProcessSettings.bOverride_ColorSaturation = true;

	// Initialize to minimal distortion (high sanity)
	Camera->PostProcessSettings.VignetteIntensity = MinVignetteIntensity;
	Camera->PostProcessSettings.FilmGrainIntensity = 0.0f;
	Camera->PostProcessSettings.SceneFringeIntensity = 0.0f;
	Camera->PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);

	UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Post-process settings initialized"));
}

void USanityPerceptionComponent::CheckRegenCap(float CurrentSanity, float MaxSanity)
{
	// If regen is active and we've reached 80% cap, stop regen
	if (RegenHandle.IsValid())
	{
		float RegenCapValue = MaxSanity * 0.8f; // 80% cap
		if (CurrentSanity >= RegenCapValue)
		{
			StopSanityRegen();
			UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Sanity reached regen cap (%.1f%%), regen stopped"),
				(RegenCapValue / MaxSanity) * 100.0f);
		}
	}
}
