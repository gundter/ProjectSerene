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

AHorrorCharacter::AHorrorCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;
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

	// Clear the regen delay timer
	GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimer);

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

			UE_LOG(LogTemp, Log, TEXT("HorrorCharacter: GAS initialized, stamina regen started"));
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

	// Calculate stamina percentage for UI
	const float MaxStamina = AttributeSet->GetMaxStamina();
	const float Percent = MaxStamina > 0.0f ? Data.NewValue / MaxStamina : 0.0f;

	// Broadcast to UI (preserves existing delegate for Blueprint HUD compatibility)
	OnSprintMeterUpdated.Broadcast(Percent);

	// Handle recovery threshold: can't sprint again until 20% stamina
	if (bRecovering && Data.NewValue >= MaxStamina * 0.2f)
	{
		bRecovering = false;

		// Restore normal walk speed if not trying to sprint
		if (!bSprinting)
		{
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		}

		UE_LOG(LogTemp, Log, TEXT("HorrorCharacter: Recovery complete, sprinting allowed"));
	}

	// Force stop sprint if stamina depleted
	if (Data.NewValue <= 0.0f && bSprinting)
	{
		DoEndSprint();
		bRecovering = true;

		// Set recovering walk speed
		GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;

		UE_LOG(LogTemp, Log, TEXT("HorrorCharacter: Stamina depleted, entering recovery mode"));
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

	// Set the sprinting flag
	bSprinting = true;

	// Cancel any pending regen delay
	GetWorld()->GetTimerManager().ClearTimer(RegenDelayTimer);

	// Stop regen and start drain
	StopStaminaRegen();
	ApplyStaminaDrain();

	// Set the sprint walk speed
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	// Add State.Sprinting tag to ASC
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->AddLooseGameplayTag(SereneGameplayTags::State_Sprinting);
	}

	// Broadcast sprint state changed for UI
	OnSprintStateChanged.Broadcast(true);

	UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Sprint started"));
}

void AHorrorCharacter::DoEndSprint()
{
	// Prevent double-calls
	if (!bSprinting)
	{
		return;
	}

	// Clear the sprinting flag
	bSprinting = false;

	// Remove drain effect
	RemoveStaminaDrain();

	// Remove State.Sprinting tag from ASC
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_Sprinting);
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

	// Broadcast sprint state changed for UI
	OnSprintStateChanged.Broadcast(false);

	UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Sprint ended, regen starting in %.1f seconds"), StaminaRegenDelay);
}

void AHorrorCharacter::StartStaminaRegen()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !StaminaRegenEffect)
	{
		return;
	}

	// Don't start regen if we're currently sprinting
	if (bSprinting)
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
		UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Stamina regen effect applied"));
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
		UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Stamina regen effect removed"));
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
		UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Stamina drain effect applied"));
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
		UE_LOG(LogTemp, Verbose, TEXT("HorrorCharacter: Stamina drain effect removed"));
	}
}
