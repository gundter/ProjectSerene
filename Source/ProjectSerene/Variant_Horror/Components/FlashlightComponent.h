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
class UCurveFloat;
class UTimelineComponent;
struct FOnAttributeChangeData;

/**
 * EFlashlightState
 *
 * Represents the current state of the flashlight.
 * Full state machine with visual polish states.
 */
UENUM(BlueprintType)
enum class EFlashlightState : uint8
{
	/** Flashlight is off */
	Off,

	/** Flashlight is fading in (~0.5s), no battery drain yet */
	WarmingUp,

	/** Flashlight is on at full brightness, draining battery */
	On,

	/** Battery < 10%, subtle flicker + battery drain continues */
	Flickering,

	/** Battery depleted, dramatic 1-2s death sequence */
	DyingOut
};

/**
 * UFlashlightComponent
 *
 * Manages flashlight state, battery drain, and GAS integration.
 * Attached to HorrorCharacter, this component:
 * - Controls flashlight toggle on/off with warm-up fade-in
 * - Applies battery drain Gameplay Effect when flashlight is on/flickering
 * - Monitors Battery attribute for flicker threshold and auto-off
 * - Implements dramatic death sequence when battery depletes
 * - Adds sprint sway via rotation offset
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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ----------------------------------------
	// Configuration - Battery Drain
	// ----------------------------------------

	/** Gameplay Effect that drains battery while flashlight is on (Infinite with Period) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Battery")
	TSubclassOf<UGameplayEffect> BatteryDrainEffect;

	// ----------------------------------------
	// Configuration - Audio
	// ----------------------------------------

	/** Sound to play when flashlight turns on */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ToggleOnSound;

	/** Sound to play when flashlight turns off */
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* ToggleOffSound;

	// ----------------------------------------
	// Configuration - Light
	// ----------------------------------------

	/** Base flashlight intensity in lumens */
	UPROPERTY(EditDefaultsOnly, Category = "Light")
	float BaseIntensity = 5000.0f;

	// ----------------------------------------
	// Configuration - Curves
	// ----------------------------------------

	/** Curve defining intensity ramp during warm-up (0->1 over ~0.5s) */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Curves")
	UCurveFloat* WarmupCurve;

	/** Curve defining intensity decay during death sequence (1->0 over ~1.5s) */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Curves")
	UCurveFloat* DeathCurve;

	// ----------------------------------------
	// Configuration - Flicker
	// ----------------------------------------

	/** Battery percentage threshold below which flickering begins (10% = 0.10) */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Flicker", meta = (ClampMin = "0.0", ClampMax = "0.5"))
	float FlickerThreshold = 0.10f;

	/** Flicker frequency multiplier for Perlin noise */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Flicker", meta = (ClampMin = "1.0", ClampMax = "20.0"))
	float FlickerFrequency = 8.0f;

	/** Minimum intensity during flicker (relative to base, 0.3 = 30%) */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Flicker", meta = (ClampMin = "0.1", ClampMax = "0.9"))
	float FlickerMinIntensity = 0.3f;

	// ----------------------------------------
	// Configuration - Sprint Sway
	// ----------------------------------------

	/** Maximum pitch sway in degrees during sprint */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Sprint", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float MaxSwayPitch = 3.0f;

	/** Maximum yaw sway in degrees during sprint */
	UPROPERTY(EditDefaultsOnly, Category = "Flashlight|Sprint", meta = (ClampMin = "0.0", ClampMax = "10.0"))
	float MaxSwayYaw = 2.0f;

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
	// Timeline Components
	// ----------------------------------------

	/** Timeline for warm-up fade-in */
	UPROPERTY()
	UTimelineComponent* WarmupTimeline;

	/** Timeline for death sequence fade-out */
	UPROPERTY()
	UTimelineComponent* DeathTimeline;

	// ----------------------------------------
	// Flicker State
	// ----------------------------------------

	/** Timer handle for flicker updates */
	FTimerHandle FlickerTimer;

	/** Time accumulator for flicker Perlin noise */
	float FlickerTime = 0.0f;

	// ----------------------------------------
	// Sprint Sway State
	// ----------------------------------------

	/** Time accumulator for sway calculation */
	float SwayTime = 0.0f;

	/** Cached base rotation of spotlight */
	FRotator BaseRotation;

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

	/** Sets the SpotLight intensity */
	void SetLightIntensity(float Intensity);

	/** Get the owning character's Ability System Component */
	UAbilitySystemComponent* GetOwnerASC() const;

	/** Add the State.FlashlightOn gameplay tag */
	void AddFlashlightTag();

	/** Remove the State.FlashlightOn gameplay tag */
	void RemoveFlashlightTag();

	/** Play toggle sound based on direction */
	void PlayToggleSound(bool bTurningOn);

	// ----------------------------------------
	// Timeline Methods
	// ----------------------------------------

	/** Creates and configures timeline components */
	void SetupTimelines();

	/** Called each tick of warm-up timeline */
	UFUNCTION()
	void OnWarmupTick(float Value);

	/** Called when warm-up timeline finishes */
	UFUNCTION()
	void OnWarmupFinished();

	/** Called each tick of death timeline */
	UFUNCTION()
	void OnDeathTick(float Value);

	/** Called when death timeline finishes */
	UFUNCTION()
	void OnDeathFinished();

	// ----------------------------------------
	// Flicker Methods
	// ----------------------------------------

	/** Starts the flicker timer loop */
	void StartFlickerLoop();

	/** Stops the flicker timer loop */
	void StopFlickerLoop();

	// ----------------------------------------
	// Battery Helpers
	// ----------------------------------------

	/** Gets current battery value from GAS */
	float GetCurrentBattery() const;

	/** Gets max battery value from GAS */
	float GetMaxBattery() const;

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
	 * - If Off and Battery > 0: starts warm-up
	 * - If On or Flickering: turns off
	 * - Ignored during WarmingUp or DyingOut
	 */
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void Toggle();

	/** Returns true if the flashlight is currently producing light (any state except Off) */
	UFUNCTION(BlueprintPure, Category = "Flashlight")
	bool IsOn() const { return CurrentState != EFlashlightState::Off; }

	/** Returns true if flashlight is in a stable on state (On or Flickering) */
	UFUNCTION(BlueprintPure, Category = "Flashlight")
	bool IsFlashlightActive() const { return CurrentState == EFlashlightState::On || CurrentState == EFlashlightState::Flickering; }

	/** Returns the current flashlight state */
	UFUNCTION(BlueprintPure, Category = "Flashlight")
	EFlashlightState GetState() const { return CurrentState; }
};
