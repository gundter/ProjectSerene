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
		// Sanity minimum is 5% of MaxSanity - player can never reach 0 sanity
		const float MinSanity = GetMaxSanity() * SanityMinimumPercent;
		NewValue = FMath::Clamp(NewValue, MinSanity, GetMaxSanity());
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
			if (!ASC->HasMatchingGameplayTag(SereneGameplayTags::State_Dead))
			{
				ASC->AddLooseGameplayTag(SereneGameplayTags::State_Dead);
			}
		}
		else
		{
			if (ASC->HasMatchingGameplayTag(SereneGameplayTags::State_Dead))
			{
				ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_Dead);
			}
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
		// Sanity minimum is 5% of MaxSanity - player can never reach 0 sanity
		const float MinSanity = GetMaxSanity() * SanityMinimumPercent;
		const float NewSanity = FMath::Clamp(GetSanity(), MinSanity, GetMaxSanity());
		SetSanity(NewSanity);

		// Calculate percentage for threshold tags
		const float SanityPercent = GetMaxSanity() > 0.0f ? NewSanity / GetMaxSanity() : 0.0f;

		// Apply/remove LowSanity tag based on threshold (30%) - legacy tag for compatibility
		UpdateThresholdTag(ASC, NewSanity, GetMaxSanity(), 0.30f, SereneGameplayTags::State_LowSanity);

		// Update all sanity threshold tags (50%, 30%, 20%)
		UpdateSanityThresholdTags(ASC, SanityPercent);
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
		// Pre-check required: AddLooseGameplayTag uses reference counting, so calling it
		// repeatedly will increment the count even if tag already exists
		if (!ASC->HasMatchingGameplayTag(Tag))
		{
			ASC->AddLooseGameplayTag(Tag);
		}
	}
	else
	{
		// Only remove if tag exists to avoid log warnings
		if (ASC->HasMatchingGameplayTag(Tag))
		{
			ASC->RemoveLooseGameplayTag(Tag);
		}
	}
}

float USereneAttributeSet::GetSanityRegenCapValue() const
{
	return GetMaxSanity() * SanityRegenCap;
}

void USereneAttributeSet::UpdateSanityThresholdTags(UAbilitySystemComponent* ASC, float SanityPercent)
{
	if (!ASC)
	{
		return;
	}

	// Data-driven sanity thresholds for maintainability
	struct FSanityThreshold
	{
		float Percent;
		FGameplayTag Tag;
	};

	static const FSanityThreshold SanityThresholds[] = {
		{ 0.5f, SereneGameplayTags::State_Sanity50 },  // Perception effects begin
		{ 0.3f, SereneGameplayTags::State_Sanity30 },  // Hallucinations intensify
		{ 0.2f, SereneGameplayTags::State_Sanity20 },  // Critical level, audio muffling
	};

	for (const FSanityThreshold& Threshold : SanityThresholds)
	{
		if (SanityPercent < Threshold.Percent)
		{
			// Pre-check required: AddLooseGameplayTag uses reference counting, so calling it
			// repeatedly will increment the count even if tag already exists
			if (!ASC->HasMatchingGameplayTag(Threshold.Tag))
			{
				ASC->AddLooseGameplayTag(Threshold.Tag);
			}
		}
		else
		{
			// Only remove if tag exists to avoid log warnings
			if (ASC->HasMatchingGameplayTag(Threshold.Tag))
			{
				ASC->RemoveLooseGameplayTag(Threshold.Tag);
			}
		}
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
