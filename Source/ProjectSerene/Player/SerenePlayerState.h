// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "SerenePlayerState.generated.h"

class USereneAbilitySystemComponent;
class USereneAttributeSet;
class UGameplayEffect;
class UInventoryComponent;

/**
 * ASerenePlayerState
 *
 * Custom PlayerState for Project Serene that owns the Ability System Component
 * and AttributeSet. This follows the GAS best practice of placing ASC on PlayerState
 * for player-controlled characters, allowing attributes to persist across respawns
 * and level transitions.
 *
 * The Character (HorrorCharacter) serves as the AvatarActor and accesses GAS
 * components through this PlayerState.
 */
UCLASS()
class PROJECTSERENE_API ASerenePlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASerenePlayerState();

	// ----------------------------------------
	// IAbilitySystemInterface
	// ----------------------------------------

	/** Returns the AbilitySystemComponent owned by this PlayerState */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// ----------------------------------------
	// Attribute Access
	// ----------------------------------------

	/** Returns the AttributeSet owned by this PlayerState */
	UFUNCTION(BlueprintCallable, Category = "GAS")
	USereneAttributeSet* GetAttributeSet() const { return AttributeSet; }

	// ----------------------------------------
	// Inventory Access
	// ----------------------------------------

	/** Returns the InventoryComponent owned by this PlayerState */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	// ----------------------------------------
	// Initialization
	// ----------------------------------------

	/**
	 * Initialize attributes with the default Gameplay Effect.
	 * This should be called after InitAbilityActorInfo to set starting values
	 * (Health=100, Stamina=100, etc.)
	 */
	UFUNCTION(BlueprintCallable, Category = "GAS")
	void InitializeAttributes();

protected:
	// ----------------------------------------
	// GAS Components
	// ----------------------------------------

	/** The Ability System Component - manages abilities, effects, and tags */
	UPROPERTY()
	TObjectPtr<USereneAbilitySystemComponent> AbilitySystemComponent;

	/** The Attribute Set - contains Health, Stamina, Sanity, Battery attributes */
	UPROPERTY()
	TObjectPtr<USereneAttributeSet> AttributeSet;

	// ----------------------------------------
	// Inventory Component
	// ----------------------------------------

	/** Inventory Component - manages player's collected items */
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// ----------------------------------------
	// Configuration
	// ----------------------------------------

	/**
	 * Default Gameplay Effect to initialize attribute values.
	 * Should set Health=100, MaxHealth=100, Stamina=100, MaxStamina=100, etc.
	 * Assign in Blueprint subclass (BP_SerenePlayerState).
	 */
	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> DefaultAttributeEffect;
};
