// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "FlashlightComponent.generated.h"

class USpotLightComponent;
class UGameplayEffect;
class UAbilitySystemComponent;
class USoundBase;
struct FOnAttributeChangeData;

/**
 * EFlashlightState
 *
 * Represents the current state of the flashlight.
 * Simple state machine for Plan 01 - will be extended with WarmingUp, Flickering, DyingOut in Plan 02.
 */
UENUM(BlueprintType)
enum class EFlashlightState : uint8
{
	/** Flashlight is off */
	Off,

	/** Flashlight is on and illuminating */
	On
};

/**
 * UFlashlightComponent
 *
 * Manages flashlight state, battery drain, and GAS integration.
 * Attached to HorrorCharacter, this component:
 * - Controls flashlight toggle on/off
 * - Applies battery drain Gameplay Effect when flashlight is on
 * - Monitors Battery attribute to auto-off when depleted
 * - Manages State.FlashlightOn gameplay tag for system awareness
 *
 * The actual SpotLightComponent lives on HorrorCharacter (attached to camera).
 * This component controls that light via Initialize().
 */
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class PROJECTSERENE_API UFlashlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlashlightComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// ----------------------------------------
	// Configuration
	// ----------------------------------------

	/** Gameplay Effect that drains battery while flashlight is on (Infinite with Period) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Battery")
	TSubclassOf<UGameplayEffect> BatteryDrainEffect;

	/** Sound to play when flashlight turns on */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ToggleOnSound;

	/** Sound to play when flashlight turns off */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ToggleOffSound;

	/** Base flashlight intensity in lumens (used when restoring light after flicker in Plan 02) */
	UPROPERTY(EditDefaultsOnly, Category = "Light")
	float BaseIntensity = 5000.0f;

private:
	// ----------------------------------------
	// State
	// ----------------------------------------

	/** Current flashlight state */
	EFlashlightState CurrentState = EFlashlightState::Off;

	/** Reference to the SpotLight we control (set via Initialize) */
	UPROPERTY()
	TWeakObjectPtr<USpotLightComponent> SpotLight;

	/** Cached reference to owning character's ASC */
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;

	/** Cached owner actor for GetOwner() optimization */
	UPROPERTY()
	TWeakObjectPtr<AActor> CachedOwner;

	// ----------------------------------------
	// Effect Handles
	// ----------------------------------------

	/** Handle for the active battery drain effect */
	FActiveGameplayEffectHandle DrainHandle;

	// ----------------------------------------
	// Core Methods
	// ----------------------------------------

	/**
	 * Transitions the flashlight to a new state.
	 * Handles exit from current state and entry to new state.
	 * @param NewState - The state to transition to
	 */
	void TransitionToState(EFlashlightState NewState);

	/** Starts battery drain by applying the BatteryDrainEffect */
	void StartBatteryDrain();

	/** Stops battery drain by removing the active effect */
	void StopBatteryDrain();

	/** Called when Battery attribute changes - auto-off when depleted */
	void OnBatteryChanged(const FOnAttributeChangeData& Data);

	/** Sets the SpotLight visibility */
	void SetLightEnabled(bool bEnabled);

	/** Get the owning character's Ability System Component */
	UAbilitySystemComponent* GetOwnerASC() const;

	/** Add or remove the State.FlashlightOn gameplay tag */
	void SetFlashlightTag(bool bAdd);

public:
	// ----------------------------------------
	// Public Interface
	// ----------------------------------------

	/**
	 * Initializes the component with the SpotLight to control.
	 * Must be called by HorrorCharacter after construction.
	 * @param InSpotLight - The SpotLightComponent to control
	 */
	void Initialize(USpotLightComponent* InSpotLight);

	/**
	 * Toggles the flashlight on or off.
	 * - If Off and Battery > 0: turns on
	 * - If On: turns off
	 */
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void Toggle();

	/** Returns true if the flashlight is currently on */
	UFUNCTION(BlueprintPure, Category = "Flashlight")
	bool IsOn() const { return CurrentState == EFlashlightState::On; }

	/** Returns the current flashlight state */
	UFUNCTION(BlueprintPure, Category = "Flashlight")
	EFlashlightState GetState() const { return CurrentState; }
};
