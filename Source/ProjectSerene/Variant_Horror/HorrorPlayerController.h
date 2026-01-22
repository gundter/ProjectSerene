// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HorrorPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UHorrorUI;
class UInventoryWidget;
class URadialMenuWidget;
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

	// ----------------------------------------
	// Interaction System
	// ----------------------------------------

	/** Input action for interaction (E key) */
	UPROPERTY(EditDefaultsOnly, Category = "Horror|Input")
	TObjectPtr<UInputAction> InteractAction;

	/** Maximum distance for interaction detection */
	UPROPERTY(EditDefaultsOnly, Category = "Horror|Interaction")
	float InteractionRange = 300.0f;

	/** Timer interval for interaction checking (performance) */
	UPROPERTY(EditDefaultsOnly, Category = "Horror|Interaction")
	float InteractionCheckInterval = 0.1f;

	/** Currently focused interactable actor */
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentInteractable;

	/** Handle for interaction check timer */
	FTimerHandle InteractionCheckTimerHandle;

	/** Start periodic interaction checking */
	void StartInteractionChecking();

	/** Stop periodic interaction checking */
	void StopInteractionChecking();

	/** Check for interactable object via line trace */
	void CheckForInteractable();

	/** Handle interact input */
	void OnInteractPressed();

	// ----------------------------------------
	// Inventory UI
	// ----------------------------------------

	/** Input action for inventory toggle (Tab key) */
	UPROPERTY(EditDefaultsOnly, Category = "Horror|Input")
	TObjectPtr<UInputAction> ToggleInventoryAction;

	/** Inventory widget class to spawn */
	UPROPERTY(EditAnywhere, Category = "Horror|UI")
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	/** Pointer to inventory widget instance */
	UPROPERTY()
	TObjectPtr<UInventoryWidget> InventoryWidget;

	/** Whether inventory is currently open */
	bool bInventoryOpen = false;

	/** Toggle inventory visibility */
	void ToggleInventory();

	/** Open inventory (show widget, pause game, UI input mode) */
	void OpenInventory();

	/** Close inventory (hide widget, unpause game, game input mode) */
	void CloseInventory();

	// ----------------------------------------
	// Radial Menu UI
	// ----------------------------------------

	/** Input action for radial menu toggle (Q key) */
	UPROPERTY(EditDefaultsOnly, Category = "Horror|Input")
	TObjectPtr<UInputAction> ToggleRadialMenuAction;

	/** Radial menu widget class to spawn */
	UPROPERTY(EditAnywhere, Category = "Horror|UI")
	TSubclassOf<URadialMenuWidget> RadialMenuWidgetClass;

	/** Pointer to radial menu widget instance */
	UPROPERTY()
	TObjectPtr<URadialMenuWidget> RadialMenuWidget;

	/** Toggle radial menu visibility */
	void ToggleRadialMenu();

	/** Open radial menu (show widget, show cursor, but NO pause) */
	void OpenRadialMenu();

	/** Close radial menu (hide widget, hide cursor) */
	void CloseRadialMenu();

private:
	/** Tracks whether sanity warning is currently active (for edge detection) */
	bool bSanityWarningActive = false;

	/** Cached PlayerState reference to avoid repeated GetPlayerState casts */
	UPROPERTY()
	TWeakObjectPtr<ASerenePlayerState> CachedPlayerState;
};
