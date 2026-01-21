// Copyright Project Serene. All Rights Reserved.

#include "FlashlightComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "Components/SpotLightComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Kismet/GameplayStatics.h"
#include "GAS/SereneAttributeSet.h"
#include "GAS/SereneGameplayTags.h"
#include "ProjectSerene.h"

UFlashlightComponent::UFlashlightComponent()
{
	// Enable tick for sprint sway
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UFlashlightComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache owner reference to avoid repeated GetOwner() calls
	CachedOwner = GetOwner();

	// Cache the ASC for faster access
	CachedASC = GetOwnerASC();

	// Setup timeline components for warm-up and death sequences
	SetupTimelines();

	// Cache the base rotation of the spotlight for sprint sway
	if (SpotLight.IsValid())
	{
		BaseRotation = SpotLight->GetRelativeRotation();
	}

	// Bind to Battery attribute changes for auto-off when depleted
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(
			USereneAttributeSet::GetBatteryAttribute())
			.AddUObject(this, &UFlashlightComponent::OnBatteryChanged);
	}

	// Ensure flashlight starts off
	if (SpotLight.IsValid())
	{
		SetLightEnabled(false);
	}
}

void UFlashlightComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	// Clean up battery drain effect
	StopBatteryDrain();

	// Clean up flicker timer
	StopFlickerLoop();

	// Stop timelines if playing
	if (WarmupTimeline)
	{
		WarmupTimeline->Stop();
	}
	if (DeathTimeline)
	{
		DeathTimeline->Stop();
	}

	// Clear any world timers
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FlickerTimer);
	}

	// Remove flashlight tag if active
	if (CurrentState != EFlashlightState::Off)
	{
		RemoveFlashlightTag();
	}

	Super::EndPlay(EndPlayReason);
}

void UFlashlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Only process sprint sway if flashlight is producing light
	if (!SpotLight.IsValid() || CurrentState == EFlashlightState::Off)
	{
		return;
	}

	// Check if owner is sprinting via GAS tag
	bool bIsSprinting = false;
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		bIsSprinting = ASC->HasMatchingGameplayTag(SereneGameplayTags::State_Sprinting);
	}

	if (bIsSprinting)
	{
		// Accumulate time and apply sinusoidal sway
		SwayTime += DeltaTime;
		const float SwayX = FMath::Sin(SwayTime * 8.0f) * MaxSwayPitch;
		const float SwayY = FMath::Sin(SwayTime * 6.0f) * MaxSwayYaw;
		SpotLight->SetRelativeRotation(BaseRotation + FRotator(SwayX, SwayY, 0.0f));
	}
	else
	{
		// Smoothly return to base rotation when not sprinting
		const FRotator CurrentRot = SpotLight->GetRelativeRotation();
		const FRotator NewRot = FMath::RInterpTo(CurrentRot, BaseRotation, DeltaTime, 10.0f);
		SpotLight->SetRelativeRotation(NewRot);
		SwayTime = 0.0f;
	}
}

void UFlashlightComponent::Initialize(USpotLightComponent* InSpotLight)
{
	if (!InSpotLight)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("FlashlightComponent::Initialize called with null SpotLight"));
		return;
	}

	SpotLight = InSpotLight;

	// Cache the base rotation for sprint sway
	BaseRotation = InSpotLight->GetRelativeRotation();

	// Ensure light starts off (may be called before BeginPlay)
	SetLightEnabled(false);
}

void UFlashlightComponent::Toggle()
{
	if (CurrentState == EFlashlightState::Off)
	{
		// Check if we have battery to turn on
		if (GetCurrentBattery() <= 0.0f)
		{
			UE_LOG(LogProjectSerene, Verbose, TEXT("FlashlightComponent: Cannot turn on - battery depleted"));
			return;
		}

		// Turn on via warm-up
		PlayToggleSound(true);
		TransitionToState(EFlashlightState::WarmingUp);
	}
	else if (CurrentState == EFlashlightState::On || CurrentState == EFlashlightState::Flickering)
	{
		// Turn off from stable on states
		PlayToggleSound(false);
		TransitionToState(EFlashlightState::Off);
	}
	// Ignore toggle during WarmingUp or DyingOut - player must wait
}

void UFlashlightComponent::TransitionToState(EFlashlightState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	// Exit current state
	switch (CurrentState)
	{
	case EFlashlightState::On:
		StopBatteryDrain();
		break;

	case EFlashlightState::Flickering:
		StopBatteryDrain();
		StopFlickerLoop();
		break;

	case EFlashlightState::WarmingUp:
		if (WarmupTimeline)
		{
			WarmupTimeline->Stop();
		}
		break;

	case EFlashlightState::DyingOut:
		if (DeathTimeline)
		{
			DeathTimeline->Stop();
		}
		break;

	case EFlashlightState::Off:
	default:
		// Nothing to clean up when leaving Off state
		break;
	}

	// Set new state
	CurrentState = NewState;

	// Enter new state
	switch (NewState)
	{
	case EFlashlightState::Off:
		SetLightEnabled(false);
		RemoveFlashlightTag();
		break;

	case EFlashlightState::WarmingUp:
		SetLightEnabled(true);
		SetLightIntensity(0.0f); // Start dark
		AddFlashlightTag();
		if (WarmupTimeline && WarmupCurve)
		{
			WarmupTimeline->PlayFromStart();
		}
		else
		{
			// No curve configured, skip directly to On state
			TransitionToState(EFlashlightState::On);
		}
		break;

	case EFlashlightState::On:
		SetLightIntensity(BaseIntensity);
		StartBatteryDrain();
		break;

	case EFlashlightState::Flickering:
		// Don't set intensity here - flicker loop handles it
		StartBatteryDrain();
		StartFlickerLoop();
		break;

	case EFlashlightState::DyingOut:
		StopBatteryDrain();
		RemoveFlashlightTag();
		if (DeathTimeline && DeathCurve)
		{
			DeathTimeline->PlayFromStart();
		}
		else
		{
			// No curve configured, skip directly to Off state
			TransitionToState(EFlashlightState::Off);
		}
		break;
	}
}

void UFlashlightComponent::SetupTimelines()
{
	AActor* Owner = CachedOwner.Get();
	if (!Owner)
	{
		return;
	}

	// Setup warm-up timeline
	if (WarmupCurve)
	{
		WarmupTimeline = NewObject<UTimelineComponent>(Owner, FName("FlashlightWarmupTimeline"));
		WarmupTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		Owner->AddOwnedComponent(WarmupTimeline);

		FOnTimelineFloat WarmupCallback;
		WarmupCallback.BindUFunction(this, FName("OnWarmupTick"));
		WarmupTimeline->AddInterpFloat(WarmupCurve, WarmupCallback);

		FOnTimelineEvent WarmupFinished;
		WarmupFinished.BindUFunction(this, FName("OnWarmupFinished"));
		WarmupTimeline->SetTimelineFinishedFunc(WarmupFinished);

		WarmupTimeline->RegisterComponent();
	}

	// Setup death timeline
	if (DeathCurve)
	{
		DeathTimeline = NewObject<UTimelineComponent>(Owner, FName("FlashlightDeathTimeline"));
		DeathTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		Owner->AddOwnedComponent(DeathTimeline);

		FOnTimelineFloat DeathCallback;
		DeathCallback.BindUFunction(this, FName("OnDeathTick"));
		DeathTimeline->AddInterpFloat(DeathCurve, DeathCallback);

		FOnTimelineEvent DeathFinished;
		DeathFinished.BindUFunction(this, FName("OnDeathFinished"));
		DeathTimeline->SetTimelineFinishedFunc(DeathFinished);

		DeathTimeline->RegisterComponent();
	}
}

void UFlashlightComponent::OnWarmupTick(float Value)
{
	// Value from curve is 0->1, scale by base intensity
	SetLightIntensity(BaseIntensity * Value);
}

void UFlashlightComponent::OnWarmupFinished()
{
	// Warm-up complete, transition to On state
	TransitionToState(EFlashlightState::On);
}

void UFlashlightComponent::OnDeathTick(float Value)
{
	// Value from curve is 1->0
	// Add dramatic flicker during death using Perlin noise
	const float TimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float Noise = FMath::PerlinNoise1D(TimeSeconds * 15.0f);
	const float Flicker = FMath::Lerp(0.2f, 1.0f, (Noise + 1.0f) * 0.5f);
	SetLightIntensity(BaseIntensity * Value * Flicker);
}

void UFlashlightComponent::OnDeathFinished()
{
	// Death sequence complete, transition to Off state
	TransitionToState(EFlashlightState::Off);
}

void UFlashlightComponent::StartFlickerLoop()
{
	FlickerTime = 0.0f;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().SetTimer(
		FlickerTimer,
		[this]()
		{
			if (CurrentState != EFlashlightState::Flickering)
			{
				return;
			}

			FlickerTime += 0.05f; // 20Hz update

			// Perlin noise for organic variation
			const float Noise = FMath::PerlinNoise1D(FlickerTime * FlickerFrequency);
			float FlickerIntensity = FMath::Lerp(FlickerMinIntensity, 1.0f, (Noise + 1.0f) * 0.5f);

			// Occasional brief full flicker (2% chance per tick)
			if (FMath::FRand() < 0.02f)
			{
				FlickerIntensity = 0.1f;
			}

			SetLightIntensity(BaseIntensity * FlickerIntensity);
		},
		0.05f, // 20Hz
		true   // Looping
	);
}

void UFlashlightComponent::StopFlickerLoop()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FlickerTimer);
	}
}

void UFlashlightComponent::StartBatteryDrain()
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC || !BatteryDrainEffect)
	{
		UE_LOG(LogProjectSerene, Warning, TEXT("FlashlightComponent: Cannot start battery drain - missing ASC or BatteryDrainEffect"));
		return;
	}

	// Don't double-apply if already active
	if (DrainHandle.IsValid())
	{
		return;
	}

	// Apply the drain effect
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(CachedOwner.Get());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(BatteryDrainEffect, 1, ContextHandle);
	if (SpecHandle.IsValid())
	{
		DrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void UFlashlightComponent::StopBatteryDrain()
{
	if (!DrainHandle.IsValid())
	{
		return;
	}

	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		ASC->RemoveActiveGameplayEffect(DrainHandle);
		DrainHandle.Invalidate();
	}
}

void UFlashlightComponent::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
	// Ignore battery changes during transient states or when off
	if (CurrentState == EFlashlightState::Off ||
		CurrentState == EFlashlightState::WarmingUp ||
		CurrentState == EFlashlightState::DyingOut)
	{
		return;
	}

	const float MaxBattery = GetMaxBattery();
	const float Percent = MaxBattery > 0.0f ? Data.NewValue / MaxBattery : 0.0f;

	// Battery depleted - start death sequence
	if (Data.NewValue <= 0.0f)
	{
		UE_LOG(LogProjectSerene, Log, TEXT("FlashlightComponent: Battery depleted - starting death sequence"));
		TransitionToState(EFlashlightState::DyingOut);
		return;
	}

	// Trigger flicker below threshold (if currently in On state)
	if (Percent <= FlickerThreshold && CurrentState == EFlashlightState::On)
	{
		UE_LOG(LogProjectSerene, Log, TEXT("FlashlightComponent: Battery below %.0f%% - entering flicker state"), FlickerThreshold * 100.0f);
		TransitionToState(EFlashlightState::Flickering);
	}
}

void UFlashlightComponent::SetLightEnabled(bool bEnabled)
{
	if (!SpotLight.IsValid())
	{
		return;
	}

	SpotLight->SetVisibility(bEnabled);
}

void UFlashlightComponent::SetLightIntensity(float Intensity)
{
	if (!SpotLight.IsValid())
	{
		return;
	}

	SpotLight->SetIntensity(Intensity);
}

UAbilitySystemComponent* UFlashlightComponent::GetOwnerASC() const
{
	AActor* Owner = CachedOwner.Get();
	if (!Owner)
	{
		Owner = GetOwner();
	}

	if (!Owner)
	{
		return nullptr;
	}

	// Try to get ASC via IAbilitySystemInterface
	if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Owner))
	{
		return ASI->GetAbilitySystemComponent();
	}

	return nullptr;
}

void UFlashlightComponent::AddFlashlightTag()
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC)
	{
		return;
	}

	if (!ASC->HasMatchingGameplayTag(SereneGameplayTags::State_FlashlightOn))
	{
		ASC->AddLooseGameplayTag(SereneGameplayTags::State_FlashlightOn);
	}
}

void UFlashlightComponent::RemoveFlashlightTag()
{
	UAbilitySystemComponent* ASC = CachedASC.Get();
	if (!ASC)
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(SereneGameplayTags::State_FlashlightOn))
	{
		ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_FlashlightOn);
	}
}

void UFlashlightComponent::PlayToggleSound(bool bTurningOn)
{
	USoundBase* Sound = bTurningOn ? ToggleOnSound : ToggleOffSound;
	if (Sound && CachedOwner.IsValid())
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Sound,
			CachedOwner->GetActorLocation()
		);
	}
}

float UFlashlightComponent::GetCurrentBattery() const
{
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		return ASC->GetNumericAttribute(USereneAttributeSet::GetBatteryAttribute());
	}
	return 0.0f;
}

float UFlashlightComponent::GetMaxBattery() const
{
	if (UAbilitySystemComponent* ASC = CachedASC.Get())
	{
		return ASC->GetNumericAttribute(USereneAttributeSet::GetMaxBatteryAttribute());
	}
	return 100.0f;
}
