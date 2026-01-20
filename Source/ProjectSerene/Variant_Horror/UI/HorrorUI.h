// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HorrorUI.generated.h"

class AHorrorCharacter;

/**
 *  UI widget for a first person horror game
 *
 *  Manages display of core player attributes:
 *  - Health: Player vitality
 *  - Stamina: Sprint resource
 *  - Sanity: Mental state affecting perception
 *  - Battery: Flashlight power
 *
 *  C++ provides the interface via Update*Bar methods.
 *  Blueprint implements actual bar widgets and fade animations.
 */
UCLASS(abstract)
class PROJECTSERENE_API UHorrorUI : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Sets up delegate listeners for the passed character (legacy compatibility) */
	void SetupCharacter(AHorrorCharacter* HorrorCharacter);

	// ----------------------------------------
	// Attribute Update Methods (called by PlayerController)
	// ----------------------------------------

	/** Update health bar (0-1 percentage) */
	UFUNCTION(BlueprintCallable, Category = "Horror|Attributes")
	void UpdateHealthBar(float Percent);

	/** Update stamina bar (0-1 percentage) */
	UFUNCTION(BlueprintCallable, Category = "Horror|Attributes")
	void UpdateStaminaBar(float Percent);

	/** Update sanity bar (0-1 percentage) */
	UFUNCTION(BlueprintCallable, Category = "Horror|Attributes")
	void UpdateSanityBar(float Percent);

	/** Update battery bar (0-1 percentage) */
	UFUNCTION(BlueprintCallable, Category = "Horror|Attributes")
	void UpdateBatteryBar(float Percent);

	/** Show attribute bar with auto-fade (bar name identifies which bar to show) */
	UFUNCTION(BlueprintCallable, Category = "Horror|Attributes")
	void ShowAttributeBar(FName BarName);

	// ----------------------------------------
	// Legacy Sprint Callbacks (for existing delegate compatibility)
	// ----------------------------------------

	/** Called when the character's sprint meter is updated */
	UFUNCTION()
	void OnSprintMeterUpdated(float Percent);

	/** Called when the character's sprint state changes */
	UFUNCTION()
	void OnSprintStateChanged(bool bSprinting);

protected:

	// ----------------------------------------
	// Blueprint Implementable Events
	// ----------------------------------------

	/** Called when health bar should update (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Health Updated"))
	void BP_HealthUpdated(float Percent);

	/** Called when stamina/sprint meter should update (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Sprint Meter Updated"))
	void BP_SprintMeterUpdated(float Percent);

	/** Called when sanity bar should update (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Sanity Updated"))
	void BP_SanityUpdated(float Percent);

	/** Called when battery bar should update (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Battery Updated"))
	void BP_BatteryUpdated(float Percent);

	/** Called when an attribute bar should be shown with auto-fade (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Show Attribute Bar"))
	void BP_ShowAttributeBar(const FName& BarName);

	/** Called when sprint state changes (implement in Blueprint) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Horror|Attributes", meta = (DisplayName = "Sprint State Changed"))
	void BP_SprintStateChanged(bool bSprinting);

	// ----------------------------------------
	// Configuration
	// ----------------------------------------

	/** How long bars stay visible before fading (managed in Blueprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Horror|Attributes")
	float BarDisplayDuration = 3.0f;
};
