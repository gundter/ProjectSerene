// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "ProjectSereneCameraManager.h"
#include "HorrorCharacter.h"
#include "HorrorUI.h"
#include "ProjectSerene.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "TimerManager.h"

// GAS includes for attribute listeners
#include "AbilitySystemComponent.h"
#include "Player/SerenePlayerState.h"
#include "GAS/SereneAttributeSet.h"

// Interaction system includes
#include "Inventory/InteractableInterface.h"
#include "Inventory/InventoryComponent.h"

// Inventory UI includes
#include "UI/InventoryWidget.h"
#include "Kismet/GameplayStatics.h"

AHorrorPlayerController::AHorrorPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AProjectSereneCameraManager::StaticClass();
}

void AHorrorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogProjectSerene, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	// Start periodic interaction checking for local player
	if (IsLocalPlayerController())
	{
		StartInteractionChecking();
	}
}

void AHorrorPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(aPawn))
		{
			// create the UI
			if (!HorrorUI)
			{
				HorrorUI = CreateWidget<UHorrorUI>(this, HorrorUIClass);
				HorrorUI->AddToViewport(0);
			}

			// Legacy: setup character delegates for sprint state
			HorrorUI->SetupCharacter(HorrorCharacter);

			// New: setup GAS attribute listeners for all attributes
			SetupAttributeListeners();

			// Create inventory widget (hidden by default)
			if (InventoryWidgetClass && !InventoryWidget)
			{
				InventoryWidget = CreateWidget<UInventoryWidget>(this, InventoryWidgetClass);
				if (InventoryWidget)
				{
					InventoryWidget->AddToViewport(10); // Higher ZOrder than HorrorUI
					InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

					// Initialize with inventory component from PlayerState
					if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
					{
						InventoryWidget->SetInventoryComponent(PS->GetInventoryComponent());
					}
				}
			}
		}
	}
}

void AHorrorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

		// Bind input actions
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
		{
			if (InteractAction)
			{
				EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHorrorPlayerController::OnInteractPressed);
			}
			if (ToggleInventoryAction)
			{
				EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AHorrorPlayerController::ToggleInventory);
			}
		}
	}
}

bool AHorrorPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

// ----------------------------------------
// GAS Attribute Listeners
// ----------------------------------------

void AHorrorPlayerController::SetupAttributeListeners()
{
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	if (!PS)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("HorrorPlayerController: Cannot setup attribute listeners - PlayerState is null"));
		return;
	}

	// Cache PlayerState to avoid repeated GetPlayerState casts in attribute handlers
	CachedPlayerState = PS;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("HorrorPlayerController: Cannot setup attribute listeners - ASC is null"));
		return;
	}

	// Bind attribute change delegates for all core attributes
	ASC->GetGameplayAttributeValueChangeDelegate(
		USereneAttributeSet::GetHealthAttribute())
		.AddUObject(this, &AHorrorPlayerController::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		USereneAttributeSet::GetStaminaAttribute())
		.AddUObject(this, &AHorrorPlayerController::OnStaminaChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		USereneAttributeSet::GetSanityAttribute())
		.AddUObject(this, &AHorrorPlayerController::OnSanityChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		USereneAttributeSet::GetBatteryAttribute())
		.AddUObject(this, &AHorrorPlayerController::OnBatteryChanged);

	UE_LOG(LogProjectSerene, Log, TEXT("HorrorPlayerController: Attribute listeners bound to ASC"));
}

void AHorrorPlayerController::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI) return;

	// Use cached PlayerState for max attribute lookup
	ASerenePlayerState* PS = CachedPlayerState.Get();
	float MaxHealth = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxHealth() : 100.0f;
	float Percent = MaxHealth > 0.0f ? Data.NewValue / MaxHealth : 0.0f;

	HorrorUI->UpdateHealthBar(Percent);
}

void AHorrorPlayerController::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI) return;

	// Use cached PlayerState for max attribute lookup
	ASerenePlayerState* PS = CachedPlayerState.Get();
	float MaxStamina = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxStamina() : 100.0f;
	float Percent = MaxStamina > 0.0f ? Data.NewValue / MaxStamina : 0.0f;

	HorrorUI->UpdateStaminaBar(Percent);
}

void AHorrorPlayerController::OnSanityChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI) return;

	// Use cached PlayerState for max attribute lookup
	ASerenePlayerState* PS = CachedPlayerState.Get();
	float MaxSanity = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxSanity() : 100.0f;
	float Percent = MaxSanity > 0.0f ? Data.NewValue / MaxSanity : 0.0f;

	HorrorUI->UpdateSanityBar(Percent);

	// Update warning icon only when crossing the 30% threshold (edge detection)
	bool bShouldShowWarning = Percent < 0.3f;
	if (bShouldShowWarning != bSanityWarningActive)
	{
		bSanityWarningActive = bShouldShowWarning;
		HorrorUI->UpdateSanityWarning(Percent, bSanityWarningActive);
	}
}

void AHorrorPlayerController::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI) return;

	// Use cached PlayerState for max attribute lookup
	ASerenePlayerState* PS = CachedPlayerState.Get();
	float MaxBattery = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxBattery() : 100.0f;
	float Percent = MaxBattery > 0.0f ? Data.NewValue / MaxBattery : 0.0f;

	HorrorUI->UpdateBatteryBar(Percent);
}

// ----------------------------------------
// Interaction System
// ----------------------------------------

void AHorrorPlayerController::StartInteractionChecking()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			InteractionCheckTimerHandle,
			this,
			&AHorrorPlayerController::CheckForInteractable,
			InteractionCheckInterval,
			true  // looping
		);
	}
}

void AHorrorPlayerController::StopInteractionChecking()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InteractionCheckTimerHandle);
	}

	// Clear any current focus
	if (CurrentInteractable.IsValid())
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			IInteractableTarget::Execute_OnUnfocused(CurrentInteractable.Get(), ControlledPawn);
		}
		CurrentInteractable.Reset();
	}

	if (HorrorUI)
	{
		HorrorUI->HideInteractionPrompt();
	}
}

void AHorrorPlayerController::CheckForInteractable()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	// Fire line trace from camera
	FVector Start;
	FRotator Rotation;
	GetPlayerViewPoint(Start, Rotation);
	FVector End = Start + Rotation.Vector() * InteractionRange;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ControlledPawn);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult, Start, End, ECC_Visibility, Params);

	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	// Check if hit actor implements interface
	IInteractableTarget* NewInteractable = nullptr;
	if (HitActor)
	{
		NewInteractable = Cast<IInteractableTarget>(HitActor);
	}

	// Handle focus change
	AActor* NewInteractableActor = NewInteractable ? HitActor : nullptr;

	if (NewInteractableActor != CurrentInteractable.Get())
	{
		// Unfocus old
		if (CurrentInteractable.IsValid())
		{
			IInteractableTarget::Execute_OnUnfocused(CurrentInteractable.Get(), ControlledPawn);
		}

		if (HorrorUI)
		{
			HorrorUI->HideInteractionPrompt();
		}

		// Focus new
		CurrentInteractable = NewInteractableActor;
		if (CurrentInteractable.IsValid())
		{
			IInteractableTarget::Execute_OnFocused(CurrentInteractable.Get(), ControlledPawn);
			FText Prompt = IInteractableTarget::Execute_GetInteractionPrompt(CurrentInteractable.Get());

			if (HorrorUI)
			{
				HorrorUI->ShowInteractionPrompt(Prompt);
			}
		}
	}
}

void AHorrorPlayerController::OnInteractPressed()
{
	if (CurrentInteractable.IsValid())
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			IInteractableTarget::Execute_OnInteract(CurrentInteractable.Get(), ControlledPawn);
		}
	}
}

// ----------------------------------------
// Inventory UI
// ----------------------------------------

void AHorrorPlayerController::ToggleInventory()
{
	if (bInventoryOpen)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}

void AHorrorPlayerController::OpenInventory()
{
	if (!InventoryWidget) return;

	bInventoryOpen = true;
	InventoryWidget->SetVisibility(ESlateVisibility::Visible);
	InventoryWidget->RefreshInventory();

	// Pause game per CONTEXT.md
	UGameplayStatics::SetGamePaused(GetWorld(), true);

	// Switch to UI input mode
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(InventoryWidget->TakeWidget());
	SetInputMode(InputMode);
	bShowMouseCursor = true;
}

void AHorrorPlayerController::CloseInventory()
{
	if (!InventoryWidget) return;

	bInventoryOpen = false;
	InventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

	// Unpause game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	// Return to game input mode
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}
