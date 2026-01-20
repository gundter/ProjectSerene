# Phase 2: Sanity & Perception - Research

**Researched:** 2026-01-20
**Domain:** UE5 GAS Gameplay Effects, Post-Process Effects, MetaSounds, Light Detection
**Confidence:** HIGH

## Summary

This phase implements the core sanity mechanic where darkness drains sanity and light restores it (capped at 80%), with visual and audio distortion effects that scale with sanity level. The existing GAS foundation from Phase 1 provides the Sanity attribute, threshold tags (State.LowSanity at 30%), and the proven pattern for attribute change delegates.

The implementation requires four interconnected systems:
1. **Light Detection Component** - Proximity-based detection of light sources using sphere overlap or distance checks
2. **Sanity Drain/Regen via GAS** - Infinite duration Gameplay Effects with periodic execution
3. **Post-Process Distortion** - Camera-attached post-process settings modified based on sanity percentage
4. **Audio Distortion** - MetaSounds with exposed parameters driven by sanity level

**Primary recommendation:** Use an Actor Component (`USanityPerceptionComponent`) attached to HorrorCharacter that owns the light detection logic, manages GAS effect application/removal, and drives post-process and audio parameters based on sanity changes.

## Standard Stack

The established libraries/tools for this domain:

### Core (Already in Project)
| Library | Version | Purpose | Why Standard |
|---------|---------|---------|--------------|
| GAS (GameplayAbilities) | UE 5.x | Attribute management, Gameplay Effects | Already established in Phase 1, proven pattern for stamina |
| SereneAttributeSet | Project | Sanity attribute with threshold tags | Existing infrastructure |
| FPostProcessSettings | UE 5.x | Visual distortion effects | Native UE camera integration |
| MetaSounds | UE 5.x | Procedural audio distortion | Sample-accurate, parameter-driven audio |

### Supporting
| Library | Version | Purpose | When to Use |
|---------|---------|---------|-------------|
| FTimerHandle | UE 5.x | Periodic light detection checks | Every 0.1-0.2 seconds for light proximity |
| UGameplayStatics | UE 5.x | GetAllActorsOfClass for lights | Initial light caching, level changes |
| UAudioComponent | UE 5.x | MetaSound parameter interface | SetFloatParameter for sanity-driven audio |
| UCameraComponent | UE 5.x | Post-process settings on player camera | Direct FPostProcessSettings access |

### Alternatives Considered
| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| Proximity light detection | LightAwareness plugin | Plugin adds GPU sampling; proximity is simpler, matches design (binary detection) |
| Camera post-process | Post Process Volume | Volume requires world placement; camera-attached follows player automatically |
| MetaSounds | Sound Cues | Sound Cues less flexible for procedural control; MetaSounds supports real-time parameters |
| Timer-based light check | Tick | Timer at 0.1-0.2s is more performant than per-frame tick |

**Installation:**
No additional packages required - all systems are part of UE5 core or already included in project.

## Architecture Patterns

### Recommended Project Structure
```
Source/ProjectSerene/
├── GAS/
│   ├── SereneAttributeSet.h/cpp        # Existing - add sanity threshold tags
│   ├── SereneGameplayTags.h/cpp        # Existing - add new sanity thresholds
│   └── SereneAbilitySystemComponent.h/cpp # Existing
├── Variant_Horror/
│   ├── HorrorCharacter.h/cpp           # Existing - add component attachment
│   ├── Components/
│   │   └── SanityPerceptionComponent.h/cpp  # NEW - light detection + effect management
│   └── UI/
│       └── HorrorUI.h/cpp              # Existing - add warning icon support
Content/
├── Horror/
│   ├── GAS/
│   │   ├── GE_SanityDrain.uasset       # NEW - Infinite, Periodic sanity drain
│   │   └── GE_SanityRegen.uasset       # NEW - Infinite, Periodic sanity regen (capped)
│   ├── Audio/
│   │   └── MS_SanityDistortion.uasset  # NEW - MetaSound with sanity parameters
│   └── UI/
│       └── WBP_SanityWarning.uasset    # NEW - Warning icon widget
```

### Pattern 1: Actor Component for Modular Systems
**What:** Encapsulate light detection, GAS effect management, and perception control in a single component
**When to use:** When functionality needs to be added to an existing actor without modifying core class
**Example:**
```cpp
// Source: UE5 Actor Component pattern, project-specific adaptation
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class USanityPerceptionComponent : public UActorComponent
{
    GENERATED_BODY()

    // Light detection
    UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
    float LightDetectionRadius = 550.0f; // ~5-6m

    UPROPERTY(EditDefaultsOnly, Category = "Light Detection")
    float GracePeriodDuration = 1.5f; // 1-2 seconds

    // GAS Effect classes
    UPROPERTY(EditDefaultsOnly, Category = "GAS|Sanity")
    TSubclassOf<UGameplayEffect> SanityDrainEffect;

    UPROPERTY(EditDefaultsOnly, Category = "GAS|Sanity")
    TSubclassOf<UGameplayEffect> SanityRegenEffect;

    // Effect handles for removal
    FActiveGameplayEffectHandle DrainHandle;
    FActiveGameplayEffectHandle RegenHandle;

    // Timer for light detection
    FTimerHandle LightCheckTimer;
    FTimerHandle GracePeriodTimer;

    // State
    bool bInLight = false;
    bool bInGracePeriod = false;
};
```

### Pattern 2: Gameplay Effects for Attribute Modification (Established in Phase 1)
**What:** Use Infinite duration Gameplay Effects with Period for continuous drain/regen
**When to use:** Any time-based attribute modification
**Example:**
```cpp
// Source: Phase 1 HorrorCharacter stamina pattern, GAS documentation
// GE_SanityDrain Blueprint settings:
// - Duration Policy: Infinite
// - Period: 0.1 (executes every 0.1 seconds)
// - Modifier: Sanity, Add, -1.167 (100->30 in 60 seconds = 70/60 = 1.167/sec, at 0.1s = 0.1167 per tick)

// Applying from C++:
void USanityPerceptionComponent::StartSanityDrain()
{
    UAbilitySystemComponent* ASC = GetOwnerASC();
    if (!ASC || !SanityDrainEffect) return;

    if (DrainHandle.IsValid()) return; // Already active

    FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
    FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(SanityDrainEffect, 1, Context);
    if (Spec.IsValid())
    {
        DrainHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
    }
}

void USanityPerceptionComponent::StopSanityDrain()
{
    if (!DrainHandle.IsValid()) return;

    if (UAbilitySystemComponent* ASC = GetOwnerASC())
    {
        ASC->RemoveActiveGameplayEffect(DrainHandle);
        DrainHandle.Invalidate();
    }
}
```

### Pattern 3: Camera Post-Process Settings at Runtime
**What:** Modify FPostProcessSettings on the camera component directly
**When to use:** Player-centric visual effects that should follow the camera
**Example:**
```cpp
// Source: UE5 UCameraComponent documentation, forum patterns
void USanityPerceptionComponent::UpdateVisualDistortion(float SanityPercent)
{
    AHorrorCharacter* Character = GetOwner<AHorrorCharacter>();
    if (!Character) return;

    UCameraComponent* Camera = Character->GetFirstPersonCameraComponent();
    if (!Camera) return;

    // Enable post-process overrides
    Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
    Camera->PostProcessSettings.bOverride_FilmGrainIntensity = true;
    Camera->PostProcessSettings.bOverride_SceneFringeIntensity = true;
    Camera->PostProcessSettings.bOverride_ColorSaturation = true;

    // Calculate intensity (inverse of sanity - lower sanity = higher effects)
    // Effects scale from 100% sanity (minimal) to 5% sanity (maximum)
    float DistortionScale = 1.0f - FMath::Clamp(SanityPercent, 0.05f, 1.0f);

    // Apply effects with curves
    Camera->PostProcessSettings.VignetteIntensity = FMath::Lerp(0.2f, 0.8f, DistortionScale);
    Camera->PostProcessSettings.FilmGrainIntensity = FMath::Lerp(0.0f, 0.5f, DistortionScale);
    Camera->PostProcessSettings.SceneFringeIntensity = FMath::Lerp(0.0f, 3.0f, DistortionScale);

    // Color desaturation (cold/blue tint at low sanity)
    float Saturation = FMath::Lerp(1.0f, 0.6f, DistortionScale);
    Camera->PostProcessSettings.ColorSaturation = FVector4(Saturation, Saturation, Saturation, 1.0f);
}
```

### Pattern 4: MetaSound Parameter Updates
**What:** Drive MetaSound parameters from gameplay values via Audio Component
**When to use:** Procedural audio that responds to gameplay state
**Example:**
```cpp
// Source: UE5 MetaSounds Quick Start documentation
void USanityPerceptionComponent::UpdateAudioDistortion(float SanityPercent)
{
    if (!AudioComponent || !AudioComponent->IsPlaying()) return;

    // Update float parameters
    float HeartbeatIntensity = SanityPercent < 0.5f ? (1.0f - (SanityPercent / 0.5f)) : 0.0f;
    float WhisperIntensity = SanityPercent < 0.3f ? (1.0f - (SanityPercent / 0.3f)) : 0.0f;
    float MuffleAmount = SanityPercent < 0.2f ? (1.0f - (SanityPercent / 0.2f)) : 0.0f;

    AudioComponent->SetFloatParameter(FName("HeartbeatIntensity"), HeartbeatIntensity);
    AudioComponent->SetFloatParameter(FName("WhisperIntensity"), WhisperIntensity);
    AudioComponent->SetFloatParameter(FName("MuffleAmount"), MuffleAmount);
}
```

### Pattern 5: Light Detection via Distance Check
**What:** Periodic distance calculation to cached light actors
**When to use:** Proximity-based detection without physics overlap
**Example:**
```cpp
// Source: UE5 GameplayStatics, UE5 Timer documentation
void USanityPerceptionComponent::CacheLightActors()
{
    LightActors.Empty();

    // Get all light actors (Point and Spot lights)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALight::StaticClass(), FoundActors);

    for (AActor* Actor : FoundActors)
    {
        // Filter for types that provide protection (exclude flickering)
        if (APointLight* PointLight = Cast<APointLight>(Actor))
        {
            // Check for custom "Protective" tag or component flag
            if (!Actor->ActorHasTag(FName("FlickeringLight")))
            {
                LightActors.Add(Actor);
            }
        }
        else if (ASpotLight* SpotLight = Cast<ASpotLight>(Actor))
        {
            if (!Actor->ActorHasTag(FName("FlickeringLight")))
            {
                LightActors.Add(Actor);
            }
        }
    }
}

void USanityPerceptionComponent::CheckLightProximity()
{
    FVector PlayerLocation = GetOwner()->GetActorLocation();

    for (AActor* LightActor : LightActors)
    {
        if (!LightActor) continue;

        float Distance = FVector::Dist(PlayerLocation, LightActor->GetActorLocation());
        if (Distance <= LightDetectionRadius)
        {
            OnEnterLight();
            return;
        }
    }

    OnExitLight();
}
```

### Anti-Patterns to Avoid
- **Per-frame Tick for light detection:** Use timer at 0.1-0.2s intervals instead - sufficient accuracy, better performance
- **Ambient light sampling:** Design specifies proximity-based, not actual light level - avoid complexity of GPU sampling
- **Individual Post Process Volumes per effect:** Use camera's FPostProcessSettings directly for cleaner implementation
- **Separate components for each system:** One SanityPerceptionComponent encapsulates all related logic
- **Direct attribute modification:** Always use Gameplay Effects for attribute changes to maintain GAS patterns

## Don't Hand-Roll

Problems that look simple but have existing solutions:

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Attribute drain over time | Manual timer with SetAttribute | GE with Infinite Duration + Period | GAS handles clamping, tags, callbacks automatically |
| Visual effect intensity curves | Custom interpolation | FMath::Lerp + FPostProcessSettings | Native UE post-process is GPU-optimized |
| Audio parameter blending | Manual audio manipulation | MetaSounds + SetFloatParameter | Sample-accurate, designed for this use case |
| Light actor discovery | Manual scene iteration | UGameplayStatics::GetAllActorsOfClass | Optimized engine function |
| Attribute change notification | Polling in Tick | GetGameplayAttributeValueChangeDelegate | Event-driven, established Phase 1 pattern |

**Key insight:** Phase 1 established the GAS patterns for stamina. Phase 2 should follow identical patterns for sanity - the only new element is the light detection trigger and perception effects.

## Common Pitfalls

### Pitfall 1: Forgetting Post-Process Override Flags
**What goes wrong:** Setting values like `VignetteIntensity` has no effect
**Why it happens:** FPostProcessSettings requires `bOverride_*` flag to be true before value takes effect
**How to avoid:** Always set `bOverride_PropertyName = true` before modifying the property
**Warning signs:** Visual effects don't change despite code executing

### Pitfall 2: Sanity Regen Exceeding 80% Cap
**What goes wrong:** Sanity recovers to 100% instead of capping at 80%
**Why it happens:** Standard GE regen has no awareness of cap
**How to avoid:** Two options:
1. Use Gameplay Effect Execution Calculation that checks current value
2. Add conditional tag removal when sanity reaches 80% and stop regen effect
**Warning signs:** Player fully recovers in light

### Pitfall 3: Grace Period Not Preventing Immediate Drain
**What goes wrong:** Sanity starts draining the instant player leaves light
**Why it happens:** Direct transition from "in light" to "apply drain"
**How to avoid:** Add grace period state with timer before drain begins
**Warning signs:** Jerky sanity behavior at light boundaries

### Pitfall 4: MetaSound Parameters Not Updating
**What goes wrong:** Audio doesn't respond to sanity changes
**Why it happens:** Using wrong parameter name, AudioComponent not valid, or MetaSound not configured for runtime parameters
**How to avoid:**
1. Verify parameter names match exactly (case-sensitive)
2. Check AudioComponent->IsPlaying() before SetFloatParameter
3. Ensure MetaSound Input is exposed, not just an internal variable
**Warning signs:** Audio plays but stays static

### Pitfall 5: Minimum Sanity Not Enforced at 5%
**What goes wrong:** Sanity reaches 0 or goes negative
**Why it happens:** PreAttributeChange clamps to [0, Max], but design requires minimum 5%
**How to avoid:** Modify PreAttributeChange to clamp Sanity to [MaxSanity * 0.05, MaxSanity]
**Warning signs:** Player reaches 0% sanity

### Pitfall 6: Multiple Threshold Tags Not Managed
**What goes wrong:** Only single LowSanity tag exists, but design needs 50%, 30%, 20% thresholds
**Why it happens:** Phase 1 only implemented 30% threshold for sanity
**How to avoid:** Add additional threshold tags (State.Sanity50, State.Sanity30, State.Sanity20) and update PostGameplayEffectExecute
**Warning signs:** Effects only change at 30% threshold

## Code Examples

Verified patterns from official sources:

### Creating Infinite Duration Gameplay Effect (Blueprint)
```
// In UE Editor, create GE_SanityDrain:
// Source: UE5 GAS Documentation

1. Duration Policy: Infinite
2. Period: 0.1
3. Execute Periodic Effect on Application: true (optional, starts immediately)
4. Modifiers:
   - Attribute: SereneAttributeSet.Sanity
   - Modifier Op: Add
   - Modifier Magnitude: Scalable Float = -0.1167
     (Calculation: 70 points / 60 seconds / 10 ticks per second = 0.1167)
```

### Registering Attribute Change Delegate (Established Pattern)
```cpp
// Source: Phase 1 HorrorPlayerController.cpp - already verified working
void USanityPerceptionComponent::SetupSanityListener()
{
    ASerenePlayerState* PS = GetOwnerPlayerState();
    if (!PS) return;

    UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
    if (!ASC) return;

    ASC->GetGameplayAttributeValueChangeDelegate(
        USereneAttributeSet::GetSanityAttribute())
        .AddUObject(this, &USanityPerceptionComponent::OnSanityChanged);
}

void USanityPerceptionComponent::OnSanityChanged(const FOnAttributeChangeData& Data)
{
    float MaxSanity = GetMaxSanity();
    float Percent = MaxSanity > 0.0f ? Data.NewValue / MaxSanity : 0.0f;

    UpdateVisualDistortion(Percent);
    UpdateAudioDistortion(Percent);
    UpdateUIWarning(Percent);
}
```

### Timer-Based Periodic Check
```cpp
// Source: UE5 Gameplay Timers documentation
void USanityPerceptionComponent::BeginPlay()
{
    Super::BeginPlay();

    // Cache light actors
    CacheLightActors();

    // Start periodic light check (every 0.1 seconds)
    GetWorld()->GetTimerManager().SetTimer(
        LightCheckTimer,
        this,
        &USanityPerceptionComponent::CheckLightProximity,
        0.1f,
        true  // Looping
    );
}

void USanityPerceptionComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(LightCheckTimer);
    GetWorld()->GetTimerManager().ClearTimer(GracePeriodTimer);

    // Clean up active effects
    StopSanityDrain();
    StopSanityRegen();

    Super::EndPlay(EndPlayReason);
}
```

### FPostProcessSettings Modification with Override Flags
```cpp
// Source: UE5 UCameraComponent documentation, forum verification
void USanityPerceptionComponent::InitializePostProcessSettings()
{
    AHorrorCharacter* Character = GetOwner<AHorrorCharacter>();
    if (!Character) return;

    UCameraComponent* Camera = Character->GetFirstPersonCameraComponent();
    if (!Camera) return;

    // Enable camera post-process
    Camera->PostProcessBlendWeight = 1.0f;

    // Set all override flags for properties we'll modify
    Camera->PostProcessSettings.bOverride_VignetteIntensity = true;
    Camera->PostProcessSettings.bOverride_FilmGrainIntensity = true;
    Camera->PostProcessSettings.bOverride_SceneFringeIntensity = true; // Chromatic aberration
    Camera->PostProcessSettings.bOverride_ColorSaturation = true;
    Camera->PostProcessSettings.bOverride_ColorGamma = true;

    // Initialize to minimal values (high sanity)
    Camera->PostProcessSettings.VignetteIntensity = 0.2f;
    Camera->PostProcessSettings.FilmGrainIntensity = 0.0f;
    Camera->PostProcessSettings.SceneFringeIntensity = 0.0f;
    Camera->PostProcessSettings.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
}
```

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| Sound Cues for procedural audio | MetaSounds | UE 5.0 | Sample-accurate, parameter-driven audio |
| PostProcessVolume per effect | Camera FPostProcessSettings | Always available | Follows player, simpler management |
| Manual attribute modification | GAS Gameplay Effects | GAS introduction | Unified system, callbacks, tags |

**Deprecated/outdated:**
- Sound Cues: Still work but MetaSounds preferred for procedural/reactive audio
- Blueprint-only attribute management: C++ GAS integration is more robust

## Open Questions

Things that couldn't be fully resolved:

1. **Exact visual effect intensity curves**
   - What we know: Effects scale with sanity, lower = more intense
   - What's unclear: Exact curve shape (linear, exponential, stepped)
   - Recommendation: Start with linear interpolation, tune in playtesting

2. **Random spike timing for visual glitches**
   - What we know: "Constant baseline intensity with random spikes" per CONTEXT.md
   - What's unclear: Spike frequency, duration, intensity increase
   - Recommendation: Implement baseline first, add spikes as separate system

3. **MetaSound asset structure for sanity audio**
   - What we know: Need heartbeat, breathing, whispers, world muffling
   - What's unclear: Single MetaSound with branches or separate assets
   - Recommendation: Single MetaSound with multiple layer parameters for cohesion

4. **Window/moonlight detection**
   - What we know: Windows and moonlight count as protective light
   - What's unclear: How to detect these vs standard lights
   - Recommendation: Use actor tags ("ProtectiveLight") for level designers to mark

## Sources

### Primary (HIGH confidence)
- UE5 UCameraComponent documentation - FPostProcessSettings, PostProcessBlendWeight
- UE5 Gameplay Effects documentation - Duration policies, periodic execution
- UE5 MetaSounds Quick Start - SetFloatParameter, Audio Component integration
- UE5 Gameplay Timers documentation - FTimerHandle usage
- Project Phase 1 code - Established GAS patterns (SereneAttributeSet, HorrorCharacter)

### Secondary (MEDIUM confidence)
- [Epic Developer Community Forums](https://forums.unrealengine.com/t/how-to-edit-post-process-material-parameter/17930) - Runtime post-process modification
- [tranek/GASDocumentation](https://github.com/tranek/GASDocumentation) - GAS best practices
- [Tom Looman C++ Timers](https://www.tomlooman.com/unreal-engine-cpp-timers/) - Timer patterns

### Tertiary (LOW confidence)
- [Sanity System & Jumpscare Pack](https://www.fab.com/listings/0fc989ac-88c1-4a60-87de-6f324c4333b0) - Reference implementation pattern
- [LightAwareness Plugin](https://github.com/cem-akkaya/LightAwareness) - Alternative approach (not recommended for this design)

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH - All systems are native UE5 or established in Phase 1
- Architecture: HIGH - Component pattern is well-documented, GAS patterns proven
- Post-process: HIGH - FPostProcessSettings is stable API
- MetaSounds: MEDIUM - Runtime parameters documented but implementation details vary
- Light detection: MEDIUM - Proximity pattern clear, window/moonlight tagging unclear
- Pitfalls: HIGH - Based on documented issues and Phase 1 experience

**Research date:** 2026-01-20
**Valid until:** 2026-02-20 (30 days - stable UE5 systems)
