// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "SanityPerceptionComponent.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

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
