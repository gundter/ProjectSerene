// Copyright Project Serene. All Rights Reserved.

#include "SereneAttributeSet.h"
#include "SereneGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

USereneAttributeSet::USereneAttributeSet()
{
	// Initialize all attributes to 0
	// Actual starting values should be set via Gameplay Effect (GE_InitializeAttributes)
	InitHealth(0.0f);
	InitMaxHealth(0.0f);
	InitStamina(0.0f);
	InitMaxStamina(0.0f);
	InitSanity(0.0f);
	InitMaxSanity(0.0f);
	InitBattery(0.0f);
	InitMaxBattery(0.0f);
}

void USereneAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Register attributes for replication (even though single-player, this follows GAS best practices)
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, Sanity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, MaxSanity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, Battery, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USereneAttributeSet, MaxBattery, COND_None, REPNOTIFY_Always);
}

void USereneAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp current value changes to [0, Max]
	// Note: Per CONTEXT.md, attributes CAN overfill above 100 temporarily,
	// but they still clamp to MaxValue (which can itself be temporarily boosted)
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
	}
	else if (Attribute == GetSanityAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSanity());
	}
	else if (Attribute == GetBatteryAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxBattery());
	}
}

void USereneAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	// Handle Health attribute changes
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp BaseValue after instant effects
		const float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
		SetHealth(NewHealth);

		// Apply/remove LowHealth tag based on threshold (30%)
		UpdateThresholdTag(ASC, NewHealth, GetMaxHealth(), 0.30f, SereneGameplayTags::State_LowHealth);

		// Handle death (Health == 0)
		if (NewHealth <= 0.0f)
		{
			ASC->AddLooseGameplayTag(SereneGameplayTags::State_Dead);
		}
		else
		{
			ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_Dead);
		}
	}
	// Handle Stamina attribute changes
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		const float NewStamina = FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina());
		SetStamina(NewStamina);

		// Apply/remove LowStamina tag based on threshold (20%)
		UpdateThresholdTag(ASC, NewStamina, GetMaxStamina(), 0.20f, SereneGameplayTags::State_LowStamina);
	}
	// Handle Sanity attribute changes
	else if (Data.EvaluatedData.Attribute == GetSanityAttribute())
	{
		const float NewSanity = FMath::Clamp(GetSanity(), 0.0f, GetMaxSanity());
		SetSanity(NewSanity);

		// Apply/remove LowSanity tag based on threshold (30%)
		UpdateThresholdTag(ASC, NewSanity, GetMaxSanity(), 0.30f, SereneGameplayTags::State_LowSanity);
	}
	// Handle Battery attribute changes
	else if (Data.EvaluatedData.Attribute == GetBatteryAttribute())
	{
		const float NewBattery = FMath::Clamp(GetBattery(), 0.0f, GetMaxBattery());
		SetBattery(NewBattery);

		// Apply/remove LowBattery tag based on threshold (20%)
		UpdateThresholdTag(ASC, NewBattery, GetMaxBattery(), 0.20f, SereneGameplayTags::State_LowBattery);
	}
}

void USereneAttributeSet::UpdateThresholdTag(UAbilitySystemComponent* ASC, float CurrentValue, float MaxValue, float ThresholdPercent, const FGameplayTag& Tag)
{
	if (!ASC || MaxValue <= 0.0f)
	{
		return;
	}

	const float Percent = CurrentValue / MaxValue;
	if (Percent <= ThresholdPercent)
	{
		if (!ASC->HasMatchingGameplayTag(Tag))
		{
			ASC->AddLooseGameplayTag(Tag);
		}
	}
	else
	{
		ASC->RemoveLooseGameplayTag(Tag);
	}
}

// ----------------------------------------
// Replication callbacks
// ----------------------------------------

void USereneAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, Health, OldHealth);
}

void USereneAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, MaxHealth, OldMaxHealth);
}

void USereneAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, Stamina, OldStamina);
}

void USereneAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, MaxStamina, OldMaxStamina);
}

void USereneAttributeSet::OnRep_Sanity(const FGameplayAttributeData& OldSanity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, Sanity, OldSanity);
}

void USereneAttributeSet::OnRep_MaxSanity(const FGameplayAttributeData& OldMaxSanity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, MaxSanity, OldMaxSanity);
}

void USereneAttributeSet::OnRep_Battery(const FGameplayAttributeData& OldBattery)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, Battery, OldBattery);
}

void USereneAttributeSet::OnRep_MaxBattery(const FGameplayAttributeData& OldMaxBattery)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USereneAttributeSet, MaxBattery, OldMaxBattery);
}
