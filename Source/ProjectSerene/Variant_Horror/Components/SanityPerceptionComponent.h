// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "SanityPerceptionComponent.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;
class UCameraComponent;
class UAudioComponent;
class AHorrorCharacter;
struct FOnAttributeChangeData;

/**
 * USanityPerceptionComponent
 *
 * Manages sanity drain/regeneration based on light proximity.
 * Attached to HorrorCharacter, this component:
 * - Detects proximity to protective light sources
 * - Applies sanity drain Gameplay Effect when in darkness
 * - Applies sanity regen Gameplay Effect when in light (capped at 80%)
 * - Implements grace period when leaving light before drain starts
 *
 * Light detection is proximity-based, not ambient sampling.
 * Flickering lights provide no protection.
 * The player's flashlight does NOT provide sanity protection.
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class PROJECTSERENE_API USanityPerceptionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USanityPerceptionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// ----------------------------------------
	// Light Detection Configuration
	// ----------------------------------------

	/** Radius for light detection (cm) - approximately 5-6 meters */
	UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
	float LightDetectionRadius = 550.0f;

	/** Grace period before drain starts after leaving light (seconds) */
	UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
	float GracePeriodDuration = 1.5f;

	/** Interval for light proximity checks (seconds) */
	UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
	float LightCheckInterval = 0.1f;

	/** Tag that marks a light as flickering (no protection) */
	UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
	FName FlickeringLightTag = FName("FlickeringLight");

	/** Tag that marks any actor as a protective light source (windows, moonlight) */
	UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
	FName ProtectiveLightTag = FName("ProtectiveLight");

	// ----------------------------------------
	// GAS Effect Classes
	// ----------------------------------------

	/** Gameplay Effect that drains sanity in darkness (Infinite with Period) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Sanity")
	TSubclassOf<UGameplayEffect> SanityDrainEffect;

	/** Gameplay Effect that regenerates sanity in light (Infinite with Period, capped at 80%) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Sanity")
	TSubclassOf<UGameplayEffect> SanityRegenEffect;

	// ----------------------------------------
	// Visual Distortion Configuration
	// ----------------------------------------

	/** Minimum vignette intensity (at 100% sanity) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Visual")
	float MinVignetteIntensity = 0.2f;

	/** Maximum vignette intensity (at 5% sanity) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Visual")
	float MaxVignetteIntensity = 0.8f;

	/** Maximum film grain intensity (at 5% sanity, 0 at 100%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Visual")
	float MaxGrainIntensity = 0.5f;

	/** Maximum chromatic aberration intensity (at 5% sanity, 0 at 100%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Visual")
	float MaxChromaticAberration = 3.0f;

	/** Minimum color saturation (at 5% sanity, 1.0 at 100%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Visual")
	float MinColorSaturation = 0.6f;

	// ----------------------------------------
	// Audio Distortion Configuration
	// ----------------------------------------

	/** MetaSound to play for sanity distortion (heartbeat, breathing, whispers) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Audio")
	USoundBase* SanityDistortionSound;

	/** Sanity threshold below which heartbeat/breathing begins (50%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Audio")
	float HeartbeatThreshold = 0.5f;

	/** Sanity threshold below which whispers begin (30%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Audio")
	float WhisperThreshold = 0.3f;

	/** Sanity threshold below which world audio muffles (20%) */
	UPROPERTY(EditDefaultsOnly, Category = "Perception|Audio")
	float MuffleThreshold = 0.2f;

private:
	// ----------------------------------------
	// Cached Light Actors
	// ----------------------------------------

	/** Array of valid protective light actors in the level */
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> LightActors;

	// ----------------------------------------
	// State
	// ----------------------------------------

	/** True if player is currently within range of a protective light */
	bool bInLight = false;

	/** True if player is in grace period (just left light, drain hasn't started yet) */
	bool bInGracePeriod = false;

	// ----------------------------------------
	// Effect Handles
	// ----------------------------------------

	/** Handle for the active sanity drain effect */
	FActiveGameplayEffectHandle DrainHandle;

	/** Handle for the active sanity regen effect */
	FActiveGameplayEffectHandle RegenHandle;

	// ----------------------------------------
	// Timers
	// ----------------------------------------

	/** Timer for periodic light proximity checks */
	FTimerHandle LightCheckTimer;

	/** Timer for grace period before drain starts */
	FTimerHandle GracePeriodTimer;

	/** Cached reference to owning character's camera */
	UPROPERTY()
	TWeakObjectPtr<UCameraComponent> CachedCamera;

	/** Audio component for sanity distortion sounds */
	UPROPERTY()
	TObjectPtr<UAudioComponent> SanityAudioComponent;

	// ----------------------------------------
	// Core Methods
	// ----------------------------------------

	/** Cache all protective light actors in the level */
	void CacheLightActors();

	/** Periodic check for light proximity - called every LightCheckInterval */
	void CheckLightProximity();

	/** Called when player enters a light zone */
	void OnEnterLight();

	/** Called when player exits light zone (starts grace period) */
	void OnExitLight();

	/** Called when grace period ends - starts sanity drain */
	void OnGracePeriodEnded();

	// ----------------------------------------
	// Visual Distortion Methods
	// ----------------------------------------

	/** Set up sanity change listener and initialize post-process */
	void SetupSanityListener();

	/** Called when Sanity attribute changes */
	void OnSanityChanged(const FOnAttributeChangeData& Data);

	/** Update visual distortion based on sanity percentage (0-1) */
	void UpdateVisualDistortion(float SanityPercent);

	/** Initialize post-process override flags on camera */
	void InitializePostProcessSettings();

	/** Check if sanity has reached regen cap and stop regen if so */
	void CheckRegenCap(float CurrentSanity, float MaxSanity);

	// ----------------------------------------
	// Audio Distortion Methods
	// ----------------------------------------

	/** Initialize audio component for sanity effects */
	void InitializeAudioComponent();

	/** Update audio distortion based on sanity percentage (0-1) */
	void UpdateAudioDistortion(float SanityPercent);

	// ----------------------------------------
	// GAS Effect Management
	// ----------------------------------------

	/** Applies the sanity drain effect */
	void StartSanityDrain();

	/** Removes the sanity drain effect */
	void StopSanityDrain();

	/** Applies the sanity regen effect */
	void StartSanityRegen();

	/** Removes the sanity regen effect */
	void StopSanityRegen();

	/** Get the owning character's Ability System Component */
	UAbilitySystemComponent* GetOwnerASC() const;

public:
	// ----------------------------------------
	// Public State Query
	// ----------------------------------------

	/** Returns true if player is currently in a protective light zone */
	UFUNCTION(BlueprintPure, Category = "Sanity|Light")
	bool IsInLight() const { return bInLight; }

	/** Returns true if player is in grace period (just left light, drain delayed) */
	UFUNCTION(BlueprintPure, Category = "Sanity|Light")
	bool IsInGracePeriod() const { return bInGracePeriod; }

	/** Force refresh of cached light actors (call on level load or when lights change) */
	UFUNCTION(BlueprintCallable, Category = "Sanity|Light")
	void RefreshLightCache();
};
