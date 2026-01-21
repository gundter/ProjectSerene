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
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Components/AudioComponent.h"
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

	// Initialize audio component for sanity effects
	InitializeAudioComponent();
}

void USanityPerceptionComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// Stop sanity audio
	if (SanityAudioComponent && SanityAudioComponent->IsPlaying())
	{
		SanityAudioComponent->Stop();
	}

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

	AActor* OwnerActor = GetOwner();

	// Iterate through all actors and find those with point or spot light components
	// This catches both traditional ALight actors AND generic actors with light components
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;

		// Skip the owner (player character) - their flashlight should not provide protection
		if (Actor == OwnerActor)
		{
			continue;
		}

		// Filter out flickering lights - they provide no sanity protection
		if (Actor->ActorHasTag(FlickeringLightTag))
		{
			continue;
		}

		// Check if actor has a point light or spot light component
		bool bHasProtectiveLight = false;

		// Check for PointLightComponent
		if (Actor->FindComponentByClass<UPointLightComponent>())
		{
			bHasProtectiveLight = true;
		}
		// Check for SpotLightComponent
		else if (Actor->FindComponentByClass<USpotLightComponent>())
		{
			bHasProtectiveLight = true;
		}
		// Check for ProtectiveLight tag (windows, moonlight zones, etc.)
		else if (Actor->ActorHasTag(ProtectiveLightTag))
		{
			bHasProtectiveLight = true;
		}

		if (bHasProtectiveLight)
		{
			LightActors.Add(Actor);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Cached %d protective light sources (excluding owner)"), LightActors.Num());

	// Log each cached light for debugging
	for (int32 i = 0; i < LightActors.Num(); ++i)
	{
		if (AActor* LightActor = LightActors[i].Get())
		{
			UE_LOG(LogTemp, Log, TEXT("  [%d] %s at %s"), i, *LightActor->GetName(), *LightActor->GetActorLocation().ToString());
		}
	}
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

	// Check each cached light actor for actual illumination
	for (int32 i = LightActors.Num() - 1; i >= 0; --i)
	{
		AActor* LightActor = LightActors[i].Get();

		// Remove invalid (destroyed) lights from cache
		if (!LightActor)
		{
			LightActors.RemoveAt(i);
			continue;
		}

		// Check if player is actually illuminated by this light (cone + occlusion)
		if (IsIlluminatedByLight(LightActor, PlayerLocation))
		{
			bFoundLightInRange = true;
			break;  // Only need one light illuminating us
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
			// Was in light, now leaving - start grace period
			OnExitLight();
		}
		else if (!bInLight && !bInGracePeriod && !DrainHandle.IsValid())
		{
			// Never was in light and drain not active - start draining immediately
			// This handles the case where player spawns in darkness
			UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Player in darkness (no nearby lights), starting sanity drain"));
			StartSanityDrain();
		}
	}
}

bool USanityPerceptionComponent::IsIlluminatedByLight(AActor* LightActor, const FVector& PlayerLocation) const
{
	if (!LightActor)
	{
		return false;
	}

	// For actors with ProtectiveLight tag (non-light sources), use simple distance check
	if (LightActor->ActorHasTag(ProtectiveLightTag))
	{
		const float Distance = FVector::Dist(PlayerLocation, LightActor->GetActorLocation());
		return Distance <= LightDetectionRadius;
	}

	// Try to find a spot light component first
	USpotLightComponent* SpotLight = LightActor->FindComponentByClass<USpotLightComponent>();
	if (SpotLight)
	{
		// Get the light's position and direction
		const FVector LightLocation = SpotLight->GetComponentLocation();
		const FVector LightForward = SpotLight->GetForwardVector();

		// Check distance against attenuation radius
		const float Distance = FVector::Dist(PlayerLocation, LightLocation);
		const float AttenuationRadius = SpotLight->AttenuationRadius;

		if (Distance > AttenuationRadius)
		{
			return false;  // Too far from light
		}

		// Check if player is within the spot light cone
		const FVector ToPlayer = (PlayerLocation - LightLocation).GetSafeNormal();
		const float DotProduct = FVector::DotProduct(LightForward, ToPlayer);
		const float AngleToPlayer = FMath::Acos(DotProduct);  // Radians

		// Use outer cone angle (full light cone)
		const float OuterConeAngleRad = FMath::DegreesToRadians(SpotLight->OuterConeAngle);

		if (AngleToPlayer > OuterConeAngleRad)
		{
			return false;  // Player is outside the light cone
		}

		// Cone check passed, now check occlusion
		if (bCheckLightOcclusion)
		{
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());  // Ignore player
			QueryParams.AddIgnoredActor(LightActor);  // Ignore light actor

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				LightLocation,
				PlayerLocation,
				OcclusionTraceChannel,
				QueryParams
			);

			if (bHit)
			{
				return false;  // Something is blocking the light
			}
		}

		return true;  // Player is illuminated by this spot light
	}

	// Try point light component
	UPointLightComponent* PointLight = LightActor->FindComponentByClass<UPointLightComponent>();
	if (PointLight)
	{
		const FVector LightLocation = PointLight->GetComponentLocation();

		// Check distance against attenuation radius
		const float Distance = FVector::Dist(PlayerLocation, LightLocation);
		const float AttenuationRadius = PointLight->AttenuationRadius;

		if (Distance > AttenuationRadius)
		{
			return false;  // Too far from light
		}

		// Point lights have no cone, just check occlusion
		if (bCheckLightOcclusion)
		{
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(GetOwner());  // Ignore player
			QueryParams.AddIgnoredActor(LightActor);  // Ignore light actor

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				HitResult,
				LightLocation,
				PlayerLocation,
				OcclusionTraceChannel,
				QueryParams
			);

			if (bHit)
			{
				return false;  // Something is blocking the light
			}
		}

		return true;  // Player is illuminated by this point light
	}

	// No recognized light component, fall back to simple distance check
	const float Distance = FVector::Dist(PlayerLocation, LightActor->GetActorLocation());
	return Distance <= LightDetectionRadius;
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

	// Update audio effects
	UpdateAudioDistortion(SanityPercent);

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

// ----------------------------------------
// Audio Distortion Methods
// ----------------------------------------

void USanityPerceptionComponent::InitializeAudioComponent()
{
	if (!SanityDistortionSound)
	{
		UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: No SanityDistortionSound set, audio distortion disabled"));
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// Create audio component attached to owner
	SanityAudioComponent = UGameplayStatics::SpawnSoundAttached(
		SanityDistortionSound,
		Owner->GetRootComponent(),
		NAME_None,
		FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset,
		true,  // Stop when attached to destroyed
		1.0f,  // Volume multiplier
		1.0f,  // Pitch multiplier
		0.0f,  // Start time
		nullptr, // Attenuation
		nullptr, // Concurrency
		false    // Auto destroy (false - we manage lifecycle)
	);

	if (SanityAudioComponent)
	{
		// Start playing but at zero intensity
		SanityAudioComponent->SetFloatParameter(FName("HeartbeatIntensity"), 0.0f);
		SanityAudioComponent->SetFloatParameter(FName("WhisperIntensity"), 0.0f);
		SanityAudioComponent->SetFloatParameter(FName("MuffleAmount"), 0.0f);

		UE_LOG(LogTemp, Log, TEXT("SanityPerceptionComponent: Audio distortion initialized"));
	}
}

void USanityPerceptionComponent::UpdateAudioDistortion(float SanityPercent)
{
	if (!SanityAudioComponent || !SanityAudioComponent->IsPlaying())
	{
		return;
	}

	// Calculate heartbeat intensity (ramps from 0 at threshold to 1 at 0%)
	float HeartbeatIntensity = 0.0f;
	if (SanityPercent < HeartbeatThreshold)
	{
		// Map [0, HeartbeatThreshold] to [1, 0]
		HeartbeatIntensity = 1.0f - (SanityPercent / HeartbeatThreshold);
	}

	// Calculate whisper intensity (ramps from 0 at threshold to 1 at 0%)
	float WhisperIntensity = 0.0f;
	if (SanityPercent < WhisperThreshold)
	{
		// Map [0, WhisperThreshold] to [1, 0]
		WhisperIntensity = 1.0f - (SanityPercent / WhisperThreshold);
	}

	// Calculate muffle amount (ramps from 0 at threshold to 1 at 0%)
	float MuffleAmount = 0.0f;
	if (SanityPercent < MuffleThreshold)
	{
		// Map [0, MuffleThreshold] to [1, 0]
		MuffleAmount = 1.0f - (SanityPercent / MuffleThreshold);
	}

	// Update MetaSound parameters
	SanityAudioComponent->SetFloatParameter(FName("HeartbeatIntensity"), HeartbeatIntensity);
	SanityAudioComponent->SetFloatParameter(FName("WhisperIntensity"), WhisperIntensity);
	SanityAudioComponent->SetFloatParameter(FName("MuffleAmount"), MuffleAmount);

	UE_LOG(LogTemp, Verbose, TEXT("SanityPerceptionComponent: Audio distortion updated - Heartbeat: %.2f, Whisper: %.2f, Muffle: %.2f"),
		HeartbeatIntensity, WhisperIntensity, MuffleAmount);
}
