// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "ProjectSereneCameraManager.h"
#include "HorrorCharacter.h"
#include "HorrorUI.h"
#include "ProjectSerene.h"
#include "Widgets/Input/SVirtualJoystick.h"

// GAS includes for attribute listeners
#include "AbilitySystemComponent.h"
#include "Player/SerenePlayerState.h"
#include "GAS/SereneAttributeSet.h"

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
	if (!HorrorUI)
	{
		return;
	}

	// Get max health for percentage calculation
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	float MaxHealth = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxHealth() : 100.0f;
	float Percent = MaxHealth > 0.0f ? Data.NewValue / MaxHealth : 0.0f;

	HorrorUI->UpdateHealthBar(Percent);
}

void AHorrorPlayerController::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI)
	{
		return;
	}

	// Get max stamina for percentage calculation
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	float MaxStamina = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxStamina() : 100.0f;
	float Percent = MaxStamina > 0.0f ? Data.NewValue / MaxStamina : 0.0f;

	HorrorUI->UpdateStaminaBar(Percent);
}

void AHorrorPlayerController::OnSanityChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI)
	{
		return;
	}

	// Get max sanity for percentage calculation
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	float MaxSanity = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxSanity() : 100.0f;
	float Percent = MaxSanity > 0.0f ? Data.NewValue / MaxSanity : 0.0f;

	HorrorUI->UpdateSanityBar(Percent);
}

void AHorrorPlayerController::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
	if (!HorrorUI)
	{
		return;
	}

	// Get max battery for percentage calculation
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	float MaxBattery = PS && PS->GetAttributeSet() ? PS->GetAttributeSet()->GetMaxBattery() : 100.0f;
	float Percent = MaxBattery > 0.0f ? Data.NewValue / MaxBattery : 0.0f;

	HorrorUI->UpdateBatteryBar(Percent);
}
