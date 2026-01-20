// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "SereneAttributeSet.generated.h"

// Macro for generating getter/setter/initter functions for attributes
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * USereneAttributeSet
 *
 * Defines the core player attributes for Project Serene:
 * - Health/MaxHealth: Player vitality, 0 = death
 * - Stamina/MaxStamina: Sprint resource, regenerates over time
 * - Sanity/MaxSanity: Mental state, affects perception and hallucinations
 * - Battery/MaxBattery: Flashlight power resource
 *
 * Attributes can temporarily exceed their max values (overfill from items).
 * Clamping occurs in PreAttributeChange (CurrentValue) and PostGameplayEffectExecute (BaseValue).
 */
UCLASS()
class PROJECTSERENE_API USereneAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	USereneAttributeSet();

	// Attribute replication (if needed in future)
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ----------------------------------------
	// Health Attributes
	// ----------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxHealth)

	// ----------------------------------------
	// Stamina Attributes
	// ----------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, Stamina)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Stamina", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxStamina)

	// ----------------------------------------
	// Sanity Attributes
	// ----------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sanity", ReplicatedUsing = OnRep_Sanity)
	FGameplayAttributeData Sanity;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, Sanity)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Sanity", ReplicatedUsing = OnRep_MaxSanity)
	FGameplayAttributeData MaxSanity;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxSanity)

	// ----------------------------------------
	// Battery Attributes
	// ----------------------------------------

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Battery", ReplicatedUsing = OnRep_Battery)
	FGameplayAttributeData Battery;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, Battery)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Battery", ReplicatedUsing = OnRep_MaxBattery)
	FGameplayAttributeData MaxBattery;
	ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxBattery)

protected:
	/**
	 * Called before an attribute's CurrentValue changes.
	 * Use this to clamp values before they're applied.
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/**
	 * Called after a GameplayEffect executes and modifies BaseValue.
	 * Use this to clamp BaseValue and apply state tags based on thresholds.
	 */
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// ----------------------------------------
	// Replication callbacks (for future multiplayer support)
	// ----------------------------------------

	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UFUNCTION()
	virtual void OnRep_Sanity(const FGameplayAttributeData& OldSanity);

	UFUNCTION()
	virtual void OnRep_MaxSanity(const FGameplayAttributeData& OldMaxSanity);

	UFUNCTION()
	virtual void OnRep_Battery(const FGameplayAttributeData& OldBattery);

	UFUNCTION()
	virtual void OnRep_MaxBattery(const FGameplayAttributeData& OldMaxBattery);

private:
	/**
	 * Helper to apply or remove a gameplay tag based on attribute threshold.
	 * @param ASC - The ability system component to modify tags on
	 * @param CurrentValue - The current attribute value
	 * @param MaxValue - The max attribute value
	 * @param ThresholdPercent - The threshold (0.0 to 1.0) below which to add the tag
	 * @param Tag - The tag to add/remove
	 */
	void UpdateThresholdTag(UAbilitySystemComponent* ASC, float CurrentValue, float MaxValue, float ThresholdPercent, const FGameplayTag& Tag);
};
