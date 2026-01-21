// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ProjectSereneCharacter.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "HorrorCharacter.generated.h"

class USpotLightComponent;
class UInputAction;
class UGameplayEffect;
class UAbilitySystemComponent;
class USanityPerceptionComponent;
class UFlashlightComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSprintMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSprintStateChangedDelegate, bool, bSprinting);

/**
 *  First person horror character with GAS-based stamina system
 *
 *  Accesses the Ability System Component through SerenePlayerState.
 *  Stamina drain and regeneration are handled via Gameplay Effects instead of timers.
 *  Existing UI delegates (OnSprintMeterUpdated, OnSprintStateChanged) are preserved for compatibility.
 */
UCLASS(abstract)
class PROJECTSERENE_API AHorrorCharacter : public AProjectSereneCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	/** Player light source (flashlight) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

	/** Component managing sanity drain/regen based on light proximity */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USanityPerceptionComponent* SanityPerceptionComponent;

	/** Component managing flashlight state, battery drain, and light control */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UFlashlightComponent* FlashlightComponent;

protected:

	/** Sprint input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** Toggle flashlight input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* ToggleFlashlightAction;

	/** If true, the sprint key is currently held down */
	bool bSprintKeyHeld = false;

	/** If true, we're actively sprinting (key held AND moving) */
	bool bIsSprinting = false;

	/** If true, we're recovering stamina (must reach 20% before sprinting allowed) */
	bool bRecovering = false;

	/** Minimum velocity required to consider the character "moving" for sprint drain (cm/s) */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0))
	float MinSprintVelocity = 10.0f;

	/** Timer handle for periodic sprint movement check */
	FTimerHandle SprintMovementCheckTimer;

	/** Interval for checking movement while sprint key is held (seconds) */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0.01, ClampMax = 0.5))
	float SprintMovementCheckInterval = 0.1f;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	/** Walk speed while sprinting */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, Units = "cm/s"))
	float SprintSpeed = 600.0f;

	/** Walk speed while recovering stamina */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, Units = "cm/s"))
	float RecoveringWalkSpeed = 150.0f;

	// ----------------------------------------
	// GAS Stamina System
	// ----------------------------------------

	/** Gameplay Effect that drains stamina while sprinting (Infinite with Period) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Stamina")
	TSubclassOf<UGameplayEffect> StaminaDrainEffect;

	/** Gameplay Effect that regenerates stamina when not sprinting (Infinite with Period) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Stamina")
	TSubclassOf<UGameplayEffect> StaminaRegenEffect;

	/** Active handle for the stamina drain effect (to remove when sprint stops) */
	FActiveGameplayEffectHandle ActiveDrainHandle;

	/** Active handle for the stamina regen effect (to remove when sprint starts) */
	FActiveGameplayEffectHandle ActiveRegenHandle;

	/** Timer handle for delayed stamina regen start after sprint ends */
	FTimerHandle RegenDelayTimer;

	/** Delay before stamina regeneration starts after sprinting stops (seconds) */
	UPROPERTY(EditDefaultsOnly, Category = "GAS|Stamina", meta = (ClampMin = 0, ClampMax = 5))
	float StaminaRegenDelay = 2.0f;

public:

	/** Delegate called when the sprint meter should be updated (0.0 to 1.0) */
	FUpdateSprintMeterDelegate OnSprintMeterUpdated;

	/** Delegate called when we start and stop sprinting */
	FSprintStateChangedDelegate OnSprintStateChanged;

	// ----------------------------------------
	// IAbilitySystemInterface
	// ----------------------------------------

	/**
	 * Returns the AbilitySystemComponent from PlayerState.
	 * Character does not own ASC directly - it forwards to SerenePlayerState.
	 */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:

	/** Constructor */
	AHorrorCharacter();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Called when this Character is possessed by a Controller */
	virtual void PossessedBy(AController* NewController) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:

	/** Starts sprinting behavior - applies stamina drain effect */
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void DoStartSprint();

	/** Stops sprinting behavior - removes drain, starts regen after delay */
	UFUNCTION(BlueprintCallable, Category = "Sprint")
	void DoEndSprint();

	// ----------------------------------------
	// GAS Helpers
	// ----------------------------------------

	/** Called when Stamina attribute changes - updates UI and handles recovery */
	void OnStaminaChanged(const FOnAttributeChangeData& Data);

	/** Starts the stamina regeneration effect */
	void StartStaminaRegen();

	/** Stops the stamina regeneration effect */
	void StopStaminaRegen();

	/** Applies the stamina drain effect */
	void ApplyStaminaDrain();

	/** Removes the stamina drain effect */
	void RemoveStaminaDrain();

	/** Checks if character is moving and updates drain state accordingly */
	void CheckSprintMovement();

	// ----------------------------------------
	// Flashlight Input
	// ----------------------------------------

	/** Toggles the flashlight on/off - bound to input action */
	void ToggleFlashlight();

public:
	// ----------------------------------------
	// Component Accessors
	// ----------------------------------------

	/** Returns the sanity perception component */
	FORCEINLINE USanityPerceptionComponent* GetSanityPerceptionComponent() const { return SanityPerceptionComponent; }

	/** Returns the flashlight component */
	FORCEINLINE UFlashlightComponent* GetFlashlightComponent() const { return FlashlightComponent; }
};
