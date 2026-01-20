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

protected:

	/** Sprint input action */
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** If true, we're sprinting (input held down) */
	bool bSprinting = false;

	/** If true, we're recovering stamina (must reach 20% before sprinting allowed) */
	bool bRecovering = false;

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

public:
	// ----------------------------------------
	// Component Accessors
	// ----------------------------------------

	/** Returns the sanity perception component */
	FORCEINLINE USanityPerceptionComponent* GetSanityPerceptionComponent() const { return SanityPerceptionComponent; }
};
