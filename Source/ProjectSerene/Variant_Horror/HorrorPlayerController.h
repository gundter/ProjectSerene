// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HorrorPlayerController.generated.h"

class UInputMappingContext;
class UHorrorUI;
class ASerenePlayerState;
struct FOnAttributeChangeData;

/**
 *  Player Controller for a first person horror game
 *
 *  Responsibilities:
 *  - Input mapping management
 *  - UI management (HorrorUI widget)
 *  - GAS attribute change listeners (binds to ASC, forwards to HUD)
 *
 *  Attribute changes from GAS are received here and forwarded to HorrorUI
 *  for display (Health, Stamina, Sanity, Battery bars).
 */
UCLASS(abstract, config="Game")
class PROJECTSERENE_API AHorrorPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Horror|UI")
	TSubclassOf<UHorrorUI> HorrorUIClass;

	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UHorrorUI> HorrorUI;

public:

	/** Constructor */
	AHorrorPlayerController();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay Initialization */
	virtual void BeginPlay() override;

	/** Possessed pawn initialization */
	virtual void OnPossess(APawn* aPawn) override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	// ----------------------------------------
	// GAS Attribute Listeners
	// ----------------------------------------

	/** Set up GAS attribute change listeners on the PlayerState's ASC */
	void SetupAttributeListeners();

	/** Attribute change handlers - receive data from GAS, forward to HUD */
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	void OnSanityChanged(const FOnAttributeChangeData& Data);
	void OnBatteryChanged(const FOnAttributeChangeData& Data);

private:
	/** Tracks whether sanity warning is currently active (for edge detection) */
	bool bSanityWarningActive = false;

	/** Cached PlayerState reference to avoid repeated GetPlayerState casts */
	UPROPERTY()
	TWeakObjectPtr<ASerenePlayerState> CachedPlayerState;
};
