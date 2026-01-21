// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Horror/HorrorCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

// GAS includes
#include "AbilitySystemComponent.h"
#include "Player/SerenePlayerState.h"
#include "GAS/SereneAbilitySystemComponent.h"
#include "GAS/SereneAttributeSet.h"
#include "GAS/SereneGameplayTags.h"
#include "GameplayEffect.h"

// Sanity system
#include "Components/SanityPerceptionComponent.h"

// Flashlight system
#include "Components/FlashlightComponent.h"

AHorrorCharacter::AHorrorCharacter()
{
	// Create the spotlight (flashlight)
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;

	// Create the sanity perception component (light-based sanity drain/regen)
	SanityPerceptionComponent = CreateDefaultSubobject<USanityPerceptionComponent>(TEXT("SanityPerceptionComponent"));

	// Create the flashlight component and initialize with our SpotLight
	FlashlightComponent = CreateDefaultSubobject<UFlashlightComponent>(TEXT("FlashlightComponent"));
	FlashlightComponent->Initialize(SpotLight);
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// Note: GAS initialization happens in PossessedBy, not BeginPlay,
	// because PlayerState is not yet valid in BeginPlay
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear all timers
	GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimer);
	GetWorld()->GetTimerManager().ClearTimer(SprintMovementCheckTimer);

	// Remove any active gameplay effects
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ActiveDrainHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveDrainHandle);
		}
		if (ActiveRegenHandle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(ActiveRegenHandle);
		}
	}
}

void AHorrorCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Get the PlayerState and initialize GAS
	if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
	{
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (ASC)
		{
			// Initialize ASC with PlayerState as Owner, Character as Avatar
			ASC->InitAbilityActorInfo(PS, this);

			// Initialize attributes to default values (Health=100, Stamina=100, etc.)
			PS->InitializeAttributes();

			// Bind attribute change delegate for stamina to update UI and handle recovery
			ASC->GetGameplayAttributeValueChangeDelegate(
				USereneAttributeSet::GetStaminaAttribute())
				.AddUObject(this, &AHorrorCharacter::OnStaminaChanged);

			// Start stamina regen (will be blocked by sprinting tag when sprint starts)
			StartStaminaRegen();
		}
	}
}

UAbilitySystemComponent* AHorrorCharacter::GetAbilitySystemComponent() const
{
	// Forward to PlayerState's ASC
	if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
	{
		return PS->GetAbilitySystemComponent();
	}
	return nullptr;
}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndSprint);

		// Flashlight toggle (F key)
		EnhancedInputComponent->BindAction(ToggleFlashlightAction, ETriggerEvent::Started, this, &AHorrorCharacter::ToggleFlashlight);
	}
}

void AHorrorCharacter::ToggleFlashlight()
{
	if (FlashlightComponent)
	{
		FlashlightComponent->Toggle();
	}
}

void AHorrorCharacter::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	// Get PlayerState for attribute access
	ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>();
	if (!PS)
	{
		return;
	}

	USereneAttributeSet* AttributeSet = PS->GetAttributeSet();
	if (!AttributeSet)
	{
		return;
	}

	// Calculate stamina percentage for recovery check
	const float MaxStamina = AttributeSet->GetMaxStamina();
	const float Percent = MaxStamina > 0.0f ? Data.NewValue / MaxStamina : 0.0f;

	// Note: UI update is handled by HorrorPlayerController::OnStaminaChanged -> HorrorUI::UpdateStaminaBar

	// Handle recovery threshold: can't sprint again until 20% stamina
	if (bRecovering && Data.NewValue >= MaxStamina * 0.2f)
	{
		bRecovering = false;

		// Restore normal walk speed if not trying to sprint
		if (!bSprintKeyHeld)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}
	}

	// Force stop sprint if stamina depleted
	if (Data.NewValue <= 0.0f && bSprintKeyHeld)
	{
		DoEndSprint();
		bRecovering = true;

		// Set recovering walk speed
		GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
	}
}

void AHorrorCharacter::DoStartSprint()
{
	// Can't sprint if recovering
	if (bRecovering)
	{
		return;
	}

	// Check if we have stamina via GAS
	if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
	{
		USereneAttributeSet* AttributeSet = PS->GetAttributeSet();
		if (AttributeSet && AttributeSet->GetStamina() <= 0.0f)
		{
			return;
		}
	}

	// Set the sprint key held flag
	bSprintKeyHeld = true;

	// Cancel any pending regen delay
	GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimer);

	// Stop regen (will start drain only when moving)
	StopStaminaRegen();

	// Set the sprint walk speed
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	// Add State.Sprinting tag to ASC
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (!ASC->HasMatchingGameplayTag(SereneGameplayTags::State_Sprinting))
		{
			ASC->AddLooseGameplayTag(SereneGameplayTags::State_Sprinting);
		}
	}

	// Start periodic movement check to apply/remove drain based on velocity
	// The OnSprintStateChanged delegate will be broadcast from CheckSprintMovement
	// when actual movement is detected
	GetWorld()->GetTimerManager().SetTimer(
		SprintMovementCheckTimer,
		this,
		&AHorrorCharacter::CheckSprintMovement,
		SprintMovementCheckInterval,
		true,  // Looping
		0.0f   // First delay (immediate first check)
	);
}

void AHorrorCharacter::DoEndSprint()
{
	// Prevent double-calls
	if (!bSprintKeyHeld)
	{
		return;
	}

	// Track if we were actually sprinting (for UI broadcast)
	const bool bWasSprinting = bIsSprinting;

	// Clear the sprint flags
	bSprintKeyHeld = false;
	bIsSprinting = false;

	// Stop the movement check timer
	GetWorld()->GetTimerManager().ClearTimer(SprintMovementCheckTimer);

	// Remove drain effect
	RemoveStaminaDrain();

	// Remove State.Sprinting tag from ASC
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(SereneGameplayTags::State_Sprinting))
		{
			ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_Sprinting);
		}
	}

	// Set walk speed (either normal or recovering)
	if (bRecovering)
	{
		GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	// Start regen after grace period delay
	GetWorld()->GetTimerManager().SetTimer(
		RegenDelayTimer,
		this,
		&AHorrorCharacter::StartStaminaRegen,
		StaminaRegenDelay,
		false
	);

	// Broadcast sprint state changed for UI (only if we were actually sprinting)
	if (bWasSprinting)
	{
		OnSprintStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::StartStaminaRegen()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !StaminaRegenEffect)
	{
		return;
	}

	// Don't start regen if sprint key is held
	if (bSprintKeyHeld)
	{
		return;
	}

	// Don't double-apply if already active
	if (ActiveRegenHandle.IsValid())
	{
		return;
	}

	// Apply the regen effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaRegenEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		ActiveRegenHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHorrorCharacter::StopStaminaRegen()
{
	if (!ActiveRegenHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveActiveGameplayEffect(ActiveRegenHandle);
		ActiveRegenHandle.Invalidate();
	}
}

void AHorrorCharacter::ApplyStaminaDrain()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !StaminaDrainEffect)
	{
		return;
	}

	// Don't double-apply if already active
	if (ActiveDrainHandle.IsValid())
	{
		return;
	}

	// Apply the drain effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(StaminaDrainEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		ActiveDrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AHorrorCharacter::RemoveStaminaDrain()
{
	if (!ActiveDrainHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveActiveGameplayEffect(ActiveDrainHandle);
		ActiveDrainHandle.Invalidate();
	}
}

void AHorrorCharacter::CheckSprintMovement()
{
	// Get the character's current horizontal velocity
	const FVector Velocity = GetVelocity();
	const float HorizontalSpeed = FVector(Velocity.X, Velocity.Y, 0.0f).Size();
	const bool bIsMoving = HorizontalSpeed >= MinSprintVelocity;

	if (bIsMoving)
	{
		// Character is moving - apply drain if not already active
		if (!bIsSprinting)
		{
			bIsSprinting = true;
			ApplyStaminaDrain();

			// Broadcast sprint state changed for UI
			OnSprintStateChanged.Broadcast(true);
		}
	}
	else
	{
		// Character is not moving - remove drain if active
		if (bIsSprinting)
		{
			bIsSprinting = false;
			RemoveStaminaDrain();

			// Broadcast sprint state changed for UI
			OnSprintStateChanged.Broadcast(false);
		}
	}
}
