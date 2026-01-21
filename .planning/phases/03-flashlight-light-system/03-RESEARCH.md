# Phase 3: Flashlight & Light System - Research

**Researched:** 2026-01-21
**Domain:** UE5 Flashlight, SpotLightComponent, GAS Battery Drain, Timeline Interpolation
**Confidence:** HIGH

## Summary

Phase 3 implements a player-controlled flashlight that drains battery (via GAS), provides visibility in darkness, but explicitly does NOT protect sanity. The existing codebase already has a `USpotLightComponent` on `HorrorCharacter` and a fully functional Battery attribute in `SereneAttributeSet`. The implementation requires: (1) toggle input binding, (2) battery drain via Gameplay Effect, (3) warm-up/fade-in via timeline interpolation, (4) low-battery flicker via procedural noise, (5) death sequence when battery depletes, and (6) sprint beam instability.

The flashlight does NOT provide sanity protection - `SanityPerceptionComponent` already excludes the owner actor from light detection. This is a visibility-only tool that creates tension: you need it to see, but it drains battery and (in Phase 7) will attract enemies.

**Primary recommendation:** Create a new `UFlashlightComponent` ActorComponent to encapsulate all flashlight logic (state machine, timelines, battery binding), keeping `HorrorCharacter` clean. Use the established GAS pattern with a new `GE_BatteryDrain` Gameplay Effect.

## Standard Stack

The established libraries/tools for this domain:

### Core
| Library/Class | Version | Purpose | Why Standard |
|---------------|---------|---------|--------------|
| USpotLightComponent | UE5.4+ | Light source rendering | Already in HorrorCharacter; native UE light |
| UTimelineComponent | UE5.4+ | Smooth intensity interpolation | Standard UE pattern for time-based effects |
| UGameplayEffect | GAS | Battery drain over time | Matches existing stamina/sanity drain pattern |
| FOnTimelineFloat | UE5.4+ | Callback-based interpolation | Avoids tick overhead, clean C++ pattern |

### Supporting
| Library/Class | Version | Purpose | When to Use |
|---------------|---------|---------|-------------|
| UCurveFloat | UE5.4+ | Warm-up/flicker curves | Define intensity profiles in editor |
| FMath::PerlinNoise1D | UE5.4+ | Procedural flicker | Low-battery randomized flickering |
| FMath::Lerp | UE5.4+ | Value interpolation | Sprint sway, intensity blending |
| Enhanced Input | UE5.4+ | Toggle binding | F key toggle action |
| USoundBase | UE5.4+ | Mechanical click | Toggle feedback sound |

### Alternatives Considered
| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| UTimelineComponent | Tick + manual interpolation | Timeline cleaner, avoids constant tick overhead |
| Procedural flicker | Pre-baked animation curve | Procedural more varied, less predictable |
| ActorComponent | Direct in HorrorCharacter | Component provides better modularity |

**Installation:**
No additional packages required - all classes are part of UE5 core and GAS (already enabled in project).

## Architecture Patterns

### Recommended Project Structure
```
Source/ProjectSerene/
├── GAS/
│   └── SereneGameplayTags.cpp     # Add State.FlashlightOn tag
├── Variant_Horror/
│   ├── Components/
│   │   └── FlashlightComponent.cpp/h  # NEW: Flashlight state machine
│   └── HorrorCharacter.cpp        # Input binding, delegates to component
Content/Variant_Horror/
├── Input/
│   └── IA_ToggleFlashlight.uasset   # NEW: Input Action
├── Blueprints/
│   └── Gameplay/
│       ├── GameplayEffects/
│       │   └── GE_BatteryDrain.uasset   # NEW: Periodic battery drain
│       └── Curves/
│           ├── C_FlashlightWarmup.uasset  # Intensity 0->1 over 0.5s
│           └── C_FlashlightFlicker.uasset # Flicker pattern curve
```

### Pattern 1: Flashlight State Machine
**What:** Finite state machine for flashlight states (Off, WarmingUp, On, Flickering, DyingOut)
**When to use:** Managing complex behavior with multiple states and transitions
**Example:**
```cpp
// FlashlightComponent.h - State enum
UENUM(BlueprintType)
enum class EFlashlightState : uint8
{
    Off,            // Light disabled, no battery drain
    WarmingUp,      // Fading in (~0.5s), no drain yet
    On,             // Full brightness, draining battery
    Flickering,     // Battery < 10%, subtle flicker
    DyingOut        // Battery depleted, dramatic 1-2s death
};

// State transition logic
void UFlashlightComponent::TransitionToState(EFlashlightState NewState)
{
    if (CurrentState == NewState) return;

    // Exit current state
    switch (CurrentState)
    {
    case EFlashlightState::On:
    case EFlashlightState::Flickering:
        StopBatteryDrain();
        break;
    }

    CurrentState = NewState;

    // Enter new state
    switch (NewState)
    {
    case EFlashlightState::WarmingUp:
        WarmupTimeline->PlayFromStart();
        break;
    case EFlashlightState::On:
        StartBatteryDrain();
        break;
    case EFlashlightState::Flickering:
        StartBatteryDrain(); // Continue drain
        StartFlickerLoop();
        break;
    case EFlashlightState::DyingOut:
        StopBatteryDrain();
        DeathTimeline->PlayFromStart();
        break;
    }
}
```

### Pattern 2: Timeline-Based Intensity Interpolation
**What:** UTimelineComponent with curve for smooth intensity changes
**When to use:** Warm-up fade-in, death sequence fade-out
**Example:**
```cpp
// Source: Unreal Community Wiki - Timeline in C++
// FlashlightComponent.cpp - BeginPlay setup
void UFlashlightComponent::BeginPlay()
{
    Super::BeginPlay();

    if (WarmupCurve)
    {
        WarmupTimeline = NewObject<UTimelineComponent>(this, FName("WarmupTimeline"));
        WarmupTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
        GetOwner()->BlueprintCreatedComponents.Add(WarmupTimeline);

        FOnTimelineFloat WarmupCallback;
        WarmupCallback.BindUFunction(this, FName("OnWarmupTick"));
        WarmupTimeline->AddInterpFloat(WarmupCurve, WarmupCallback);

        FOnTimelineEventStatic WarmupFinished;
        WarmupFinished.BindUFunction(this, FName("OnWarmupComplete"));
        WarmupTimeline->SetTimelineFinishedFunc(WarmupFinished);

        WarmupTimeline->RegisterComponent();
    }
}

UFUNCTION()
void UFlashlightComponent::OnWarmupTick(float Value)
{
    // Value is 0.0 -> 1.0 from curve
    SpotLight->SetIntensity(BaseIntensity * Value);
}

UFUNCTION()
void UFlashlightComponent::OnWarmupComplete()
{
    TransitionToState(EFlashlightState::On);
}
```

### Pattern 3: GAS Battery Integration
**What:** Listen to Battery attribute changes, react to thresholds
**When to use:** Flicker trigger (10%), death trigger (0%)
**Example:**
```cpp
// FlashlightComponent.cpp - Binding to Battery changes
void UFlashlightComponent::SetupBatteryListener()
{
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
    if (!ASI) return;

    UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    if (!ASC) return;

    ASC->GetGameplayAttributeValueChangeDelegate(
        USereneAttributeSet::GetBatteryAttribute())
        .AddUObject(this, &UFlashlightComponent::OnBatteryChanged);
}

void UFlashlightComponent::OnBatteryChanged(const FOnAttributeChangeData& Data)
{
    if (CurrentState == EFlashlightState::Off) return;

    const float MaxBattery = GetMaxBattery();
    const float Percent = MaxBattery > 0.f ? Data.NewValue / MaxBattery : 0.f;

    // Trigger flicker below 10%
    if (Percent <= 0.10f && CurrentState == EFlashlightState::On)
    {
        TransitionToState(EFlashlightState::Flickering);
    }

    // Trigger death at 0%
    if (Data.NewValue <= 0.f && CurrentState != EFlashlightState::Off)
    {
        TransitionToState(EFlashlightState::DyingOut);
    }
}
```

### Pattern 4: Procedural Flicker with Perlin Noise
**What:** Use FMath::PerlinNoise1D for organic flickering
**When to use:** Low battery flicker effect
**Example:**
```cpp
// Source: UE5 FMath documentation
void UFlashlightComponent::TickFlicker(float DeltaTime)
{
    if (CurrentState != EFlashlightState::Flickering) return;

    FlickerTime += DeltaTime;

    // Perlin noise gives smooth -1 to 1 range
    const float Noise = FMath::PerlinNoise1D(FlickerTime * FlickerFrequency);

    // Map to intensity range (0.3 to 1.0 for subtle dimming)
    const float FlickerIntensity = FMath::Lerp(0.3f, 1.0f, (Noise + 1.0f) * 0.5f);

    // Occasional brief full flicker (rare, not strobe)
    const bool bBriefFlicker = FMath::FRand() < 0.02f; // 2% chance per tick
    const float FinalIntensity = bBriefFlicker ? 0.1f : FlickerIntensity;

    SpotLight->SetIntensity(BaseIntensity * FinalIntensity);
}
```

### Pattern 5: Sprint Beam Instability
**What:** Add rotation offset when sprinting for unstable beam feel
**When to use:** Sprint + flashlight on
**Example:**
```cpp
// FlashlightComponent.cpp - Called from character tick or timer
void UFlashlightComponent::UpdateSprintSway(float DeltaTime, bool bIsSprinting)
{
    if (!bIsSprinting || CurrentState == EFlashlightState::Off)
    {
        // Return to base rotation
        SpotLight->SetRelativeRotation(FMath::RInterpTo(
            SpotLight->GetRelativeRotation(),
            BaseRotation,
            DeltaTime,
            10.f
        ));
        return;
    }

    SwayTime += DeltaTime;

    // Procedural sway using sin waves at different frequencies
    const float SwayX = FMath::Sin(SwayTime * 8.f) * MaxSwayPitch;
    const float SwayY = FMath::Sin(SwayTime * 6.f) * MaxSwayYaw;

    FRotator SwayOffset(SwayX, SwayY, 0.f);
    SpotLight->SetRelativeRotation(BaseRotation + SwayOffset);
}
```

### Anti-Patterns to Avoid
- **Tick-based interpolation without timeline:** Harder to control, less efficient
- **Modifying light in SanityPerceptionComponent:** Separation of concerns - flashlight logic in flashlight component
- **Hard state changes without transitions:** Always use state machine for predictable behavior
- **Strobe-style aggressive flicker:** CONTEXT.md specifies "subtle dimming with occasional brief flickers"

## Don't Hand-Roll

Problems that look simple but have existing solutions:

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Intensity interpolation | Manual lerp in Tick | UTimelineComponent + UCurveFloat | Automatic time management, editor-editable curves |
| Battery drain | Manual timer decrement | GAS Gameplay Effect (Infinite, Period) | Matches existing stamina/sanity pattern, unified system |
| Toggle input | Manual key polling | Enhanced Input IA_ToggleFlashlight | Project already uses Enhanced Input |
| Flicker randomization | FMath::FRand only | FMath::PerlinNoise1D + occasional FRand spike | Perlin gives organic variation, not jarring |
| State persistence | Boolean flags | Enum-based state machine | Cleaner transitions, easier debugging |

**Key insight:** The codebase already established patterns for GAS-based attribute drain (stamina, sanity) and component-based systems (SanityPerceptionComponent). Flashlight should follow these same patterns for consistency.

## Common Pitfalls

### Pitfall 1: Forgetting Timeline TickComponent
**What goes wrong:** Timeline callbacks never fire
**Why it happens:** UTimelineComponent created at runtime needs manual tick
**How to avoid:** Either call TickComponent in owner's Tick, or add to BlueprintCreatedComponents
**Warning signs:** Timeline plays but callback functions never execute

### Pitfall 2: Double Battery Drain
**What goes wrong:** Battery drains twice as fast
**Why it happens:** Starting drain in both WarmingUp and On states, or not stopping when transitioning
**How to avoid:** Clear state machine logic - only drain in On/Flickering states
**Warning signs:** Battery depletes faster than expected

### Pitfall 3: State.LowBattery Tag Not Triggering Flicker
**What goes wrong:** Component doesn't react to low battery
**Why it happens:** Relying only on tag instead of attribute delegate
**How to avoid:** Listen to Battery attribute changes directly, use tag as supplementary
**Warning signs:** Flicker never starts even at low battery

### Pitfall 4: Flashlight Provides Sanity Protection
**What goes wrong:** Player flashlight counts as protective light
**Why it happens:** SanityPerceptionComponent finds player's SpotLightComponent
**How to avoid:** SanityPerceptionComponent already excludes owner actor - verify this works
**Warning signs:** Sanity regenerates when only flashlight is on

### Pitfall 5: Timeline Curves Not Loaded
**What goes wrong:** Null curve reference, crash on play
**Why it happens:** ConstructorHelpers path wrong, or UPROPERTY not EditDefaultsOnly
**How to avoid:** Use UPROPERTY(EditDefaultsOnly) and assign in Blueprint, not ConstructorHelpers
**Warning signs:** Null pointer on WarmupCurve in BeginPlay

### Pitfall 6: Moveable Light Performance
**What goes wrong:** Poor frame rate with flashlight
**Why it happens:** Moveable lights with shadow casting are expensive
**How to avoid:** Consider disabling cast shadows, or use lower shadow resolution scale
**Warning signs:** FPS drops significantly when flashlight is on

## Code Examples

Verified patterns from official sources and project conventions:

### Enhanced Input Toggle Binding
```cpp
// HorrorCharacter.h
UPROPERTY(EditAnywhere, Category = "Input")
UInputAction* ToggleFlashlightAction;

// HorrorCharacter.cpp - SetupPlayerInputComponent
if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
{
    // ETriggerEvent::Started for toggle (single press)
    EnhancedInput->BindAction(ToggleFlashlightAction, ETriggerEvent::Started,
        this, &AHorrorCharacter::ToggleFlashlight);
}

void AHorrorCharacter::ToggleFlashlight()
{
    if (FlashlightComponent)
    {
        FlashlightComponent->Toggle();
    }
}
```

### GE_BatteryDrain Blueprint Specification
```
// Create in Content/Variant_Horror/Blueprints/Gameplay/GameplayEffects/
Class: GameplayEffect (Blueprint)
Name: GE_BatteryDrain

Duration Policy: Infinite
Period: 0.5 (ticks every 0.5 seconds)

Modifiers:
- Attribute: SereneAttributeSet.Battery
- Modifier Op: Add
- Modifier Magnitude:
  - Type: Scalable Float
  - Value: -2.0 (drains 2 battery per tick = 4/sec = 25 seconds to drain 100)

// Adjust drain rate for desired flashlight duration
```

### Spotlight Visibility Toggle
```cpp
// Source: UE5 API Documentation
void UFlashlightComponent::SetLightEnabled(bool bEnabled)
{
    if (SpotLight)
    {
        SpotLight->SetVisibility(bEnabled);
    }
}

// For intensity-based control (used during warm-up/death)
void UFlashlightComponent::SetLightIntensity(float Intensity)
{
    if (SpotLight)
    {
        SpotLight->SetIntensity(Intensity);
    }
}
```

### Mechanical Click Sound
```cpp
// FlashlightComponent.h
UPROPERTY(EditDefaultsOnly, Category = "Audio")
USoundBase* ToggleClickSound;

// FlashlightComponent.cpp
void UFlashlightComponent::PlayToggleSound()
{
    if (ToggleClickSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ToggleClickSound,
            GetOwner()->GetActorLocation(),
            1.0f,  // Volume
            1.0f   // Pitch
        );
    }
}
```

### Volumetric Beam Setup
```cpp
// FlashlightComponent.cpp - Configure volumetric scattering
void UFlashlightComponent::ConfigureVolumetricBeam()
{
    if (SpotLight)
    {
        // Enable volumetric contribution for dusty beam effect
        // Requires Exponential Height Fog in scene with Volumetric Fog enabled
        SpotLight->SetVolumetricScatteringIntensity(1.5f);

        // Optional: Shadow settings for light shafts through dust
        SpotLight->SetCastShadows(true);
        // Note: Shadow-casting spotlights are ~3x more expensive
    }
}
```

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| Manual float interpolation | UTimelineComponent | UE4+ | Cleaner code, editor curves |
| Direct attribute modification | GAS Gameplay Effects | GAS introduction | Unified attribute system |
| Legacy Input System | Enhanced Input System | UE5 | Standard for new projects |
| Blueprint-only flashlight | C++ Component + BP config | Best practice | Performance, maintainability |

**Deprecated/outdated:**
- LegacyCameraShake (use CameraShakeBase in UE5)
- Old Input System (use Enhanced Input)
- Direct attribute Set* calls (use Gameplay Effects for modifications)

## Open Questions

Things that couldn't be fully resolved:

1. **Exact Battery Drain Rate**
   - What we know: Linear drain, flicker at 10%, total duration is design decision
   - What's unclear: Desired total flashlight runtime (30 sec? 60 sec? 120 sec?)
   - Recommendation: Start with 60 seconds (drain rate adjustable in GE_BatteryDrain)

2. **Volumetric Fog Scene Setup**
   - What we know: SpotLight VolumetricScatteringIntensity enables dusty beam
   - What's unclear: Whether Exponential Height Fog is already in test level
   - Recommendation: Document fog setup as level design task, not code task

3. **Hidden Detail Reveal (UV-style)**
   - What we know: Flashlight reveals hidden messages/evidence
   - What's unclear: Technical implementation (emissive material response? Post-process?)
   - Recommendation: Defer to Phase 5 (Investigation System) - note requirement only

## Sources

### Primary (HIGH confidence)
- **Existing codebase:** HorrorCharacter.cpp, SanityPerceptionComponent.cpp, SereneAttributeSet.cpp
- **UE5.7 Documentation:** [SpotLightComponent](https://dev.epicgames.com/documentation/en-us/unreal-engine/spot-lights-in-unreal-engine), [Volumetric Fog](https://dev.epicgames.com/documentation/en-us/unreal-engine/volumetric-fog-in-unreal-engine)
- **UE5.7 Documentation:** [Enhanced Input](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine), [Timelines](https://dev.epicgames.com/documentation/en-us/unreal-engine/timelines-in-unreal-engine)

### Secondary (MEDIUM confidence)
- **Unreal Community Wiki:** [Timeline in C++](https://unrealcommunity.wiki/timeline-in-cpp-1uktygtd) - Complete code example
- **Epic Forums:** [SpotLight intensity C++](https://forums.unrealengine.com/t/how-do-i-change-the-intensity-of-an-existing-spotlight-in-c/358954)
- **Epic Community Tutorials:** [Horror Game Flashlight](https://dev.epicgames.com/community/learning/tutorials/jL92/how-to-make-a-horror-game-in-unreal-engine-5-flashlight-part-4)

### Tertiary (LOW confidence)
- **WebSearch patterns:** Flashlight flicker implementations vary widely; procedural noise approach verified against FMath documentation

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH - All classes are documented UE5 core features
- Architecture: HIGH - Follows established project patterns (GAS, components)
- Pitfalls: HIGH - Based on direct codebase analysis and documented issues
- Code examples: HIGH - Verified against UE5 documentation and project conventions

**Research date:** 2026-01-21
**Valid until:** 2026-02-21 (30 days - stable domain, UE5 mature)
