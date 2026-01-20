// Copyright Epic Games, Inc. All Rights Reserved.


#include "HorrorUI.h"
#include "HorrorCharacter.h"

void UHorrorUI::SetupCharacter(AHorrorCharacter* HorrorCharacter)
{
	// Legacy binding for existing character delegates (stamina via Character)
	// Note: New GAS-based updates come from PlayerController->UpdateStaminaBar()
	HorrorCharacter->OnSprintMeterUpdated.AddDynamic(this, &UHorrorUI::OnSprintMeterUpdated);
	HorrorCharacter->OnSprintStateChanged.AddDynamic(this, &UHorrorUI::OnSprintStateChanged);
}

// ----------------------------------------
// Attribute Update Methods
// ----------------------------------------

void UHorrorUI::UpdateHealthBar(float Percent)
{
	BP_HealthUpdated(Percent);
	ShowAttributeBar(FName("Health"));
}

void UHorrorUI::UpdateStaminaBar(float Percent)
{
	// Use existing BP event name for compatibility
	BP_SprintMeterUpdated(Percent);
	ShowAttributeBar(FName("Stamina"));
}

void UHorrorUI::UpdateSanityBar(float Percent)
{
	BP_SanityUpdated(Percent);
	ShowAttributeBar(FName("Sanity"));
}

void UHorrorUI::UpdateBatteryBar(float Percent)
{
	BP_BatteryUpdated(Percent);
	ShowAttributeBar(FName("Battery"));
}

void UHorrorUI::ShowAttributeBar(FName BarName)
{
	// Pass to Blueprint to handle visibility and fade animation
	BP_ShowAttributeBar(BarName);
}

// ----------------------------------------
// Legacy Sprint Callbacks
// ----------------------------------------

void UHorrorUI::OnSprintMeterUpdated(float Percent)
{
	// Legacy callback from Character delegate
	// Forward to the BP handler
	BP_SprintMeterUpdated(Percent);
}

void UHorrorUI::OnSprintStateChanged(bool bSprinting)
{
	// Legacy callback from Character delegate
	BP_SprintStateChanged(bSprinting);
}
