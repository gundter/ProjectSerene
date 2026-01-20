# Phase 1: Core Attributes (GAS Foundation) - Research

**Researched:** 2026-01-20
**Domain:** Unreal Engine Gameplay Ability System (GAS) - Attributes and Effects
**Confidence:** HIGH

## Summary

The Gameplay Ability System (GAS) is Epic's battle-tested framework for attribute management, used in Fortnite and Paragon. For this single-player horror game, GAS provides a unified system for Health, Stamina, Sanity, and Battery attributes with built-in support for Gameplay Effects (instant damage, regen over time, periodic drains) and delegate-based UI binding.

The project requires enabling the GameplayAbilities plugin and adding module dependencies. Per the CONTEXT.md decision, the AbilitySystemComponent (ASC) and AttributeSet will live on Player State (not Character), following the industry-standard pattern for persistent attributes. The existing HorrorCharacter's timer-based stamina system will be replaced with GAS-managed Stamina attribute and Gameplay Effects.

**Primary recommendation:** Create a custom PlayerState class with ASC and a single AttributeSet containing all four attributes (Health, Stamina, Sanity, Battery). Initialize via instant Gameplay Effect to set starting values. Use GetGameplayAttributeValueChangeDelegate for UI binding.

## Standard Stack

The established libraries/tools for this domain:

### Core
| Module | Purpose | Why Standard |
|--------|---------|--------------|
| GameplayAbilities | AbilitySystemComponent, GameplayEffects, AttributeSets | Epic's official GAS plugin |
| GameplayTags | Hierarchical tags for attribute states (LowHealth, etc.) | Required for GAS, enables data-driven state management |
| GameplayTasks | Async tasks for abilities | Required dependency for GAS |

### Supporting
| Module | Purpose | When to Use |
|--------|---------|-------------|
| StateTreeModule | Already in project | AI behavior (later phases) |
| UMG | UI widgets | HUD attribute bars |

### Alternatives Considered
| Instead of | Could Use | Tradeoff |
|------------|-----------|----------|
| GAS Attributes | Raw float variables | Lose Gameplay Effects, no built-in replication, manual delegate wiring |
| ASC on PlayerState | ASC on Character | Simpler but attributes don't persist across respawns; less standard |

**Plugin Enablement (uproject):**
```json
{
    "Name": "GameplayAbilities",
    "Enabled": true
}
```

**Module Dependencies (Build.cs):**
```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    "Core",
    "CoreUObject",
    "Engine",
    "InputCore",
    "EnhancedInput",
    "GameplayAbilities",
    "GameplayTags",
    "GameplayTasks",
    // ... existing modules
});
```

## Architecture Patterns

### Recommended Project Structure
```
Source/ProjectSerene/
├── GAS/
│   ├── SereneAbilitySystemComponent.h/cpp    # Custom ASC subclass
│   ├── SereneAttributeSet.h/cpp              # Health, Stamina, Sanity, Battery
│   └── SereneGameplayTags.h/cpp              # Native gameplay tags
├── Player/
│   ├── SerenePlayerState.h/cpp               # Owns ASC and AttributeSet
│   └── SerenePlayerController.h/cpp          # (modify existing Horror variant)
├── Variant_Horror/
│   ├── HorrorCharacter.h/cpp                 # Modified to use GAS stamina
│   └── ...
└── ...
```

### Pattern 1: ASC on PlayerState (OwnerActor vs AvatarActor)
**What:** AbilitySystemComponent lives on PlayerState, Character is the "Avatar" (physical representation).
**When to use:** Player-controlled characters where attributes should persist across respawns/level transitions.
**Implementation:**

```cpp
// SerenePlayerState.h
UCLASS()
class ASerenePlayerState : public APlayerState, public IAbilitySystemInterface
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TObjectPtr<USereneAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<USereneAttributeSet> AttributeSet;

public:
    ASerenePlayerState();

    // IAbilitySystemInterface
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    USereneAttributeSet* GetAttributeSet() const { return AttributeSet; }
};

// SerenePlayerState.cpp
ASerenePlayerState::ASerenePlayerState()
{
    AbilitySystemComponent = CreateDefaultSubobject<USereneAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
    AbilitySystemComponent->SetIsReplicated(false); // Single-player, no replication needed

    AttributeSet = CreateDefaultSubobject<USereneAttributeSet>(TEXT("AttributeSet"));
}
```

### Pattern 2: Character Accesses ASC via PlayerState
**What:** Character implements IAbilitySystemInterface but forwards to PlayerState's ASC.
**When to use:** When ASC lives on PlayerState but Character needs to be the AvatarActor.

```cpp
// HorrorCharacter.h (modified)
class AHorrorCharacter : public AProjectSereneCharacter, public IAbilitySystemInterface
{
    // ...
public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
    virtual void PossessedBy(AController* NewController) override;
};

// HorrorCharacter.cpp
UAbilitySystemComponent* AHorrorCharacter::GetAbilitySystemComponent() const
{
    if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
    {
        return PS->GetAbilitySystemComponent();
    }
    return nullptr;
}

void AHorrorCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    // Initialize ASC with PlayerState as Owner, Character as Avatar
    if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
    {
        PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, this);
    }
}
```

### Pattern 3: AttributeSet with Accessor Macros
**What:** Use Epic's recommended ATTRIBUTE_ACCESSORS macro for consistent getter/setter generation.
**When to use:** Every attribute in every AttributeSet.

```cpp
// In a common header or AttributeSet header
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// SereneAttributeSet.h
UCLASS()
class USereneAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    USereneAttributeSet();

    // Health
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, Health)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxHealth)

    // Stamina
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Stamina;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, Stamina)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxStamina;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxStamina)

    // Sanity
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Sanity;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, Sanity)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxSanity;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxSanity)

    // Battery
    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData Battery;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, Battery)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData MaxBattery;
    ATTRIBUTE_ACCESSORS(USereneAttributeSet, MaxBattery)

protected:
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
```

### Pattern 4: Attribute Clamping
**What:** Clamp attributes in PreAttributeChange (for CurrentValue) and PostGameplayEffectExecute (for BaseValue).
**When to use:** Always, to prevent invalid attribute values.

```cpp
void USereneAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
    Super::PreAttributeChange(Attribute, NewValue);

    // Clamp current value changes
    if (Attribute == GetHealthAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
    }
    else if (Attribute == GetStaminaAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxStamina());
    }
    else if (Attribute == GetSanityAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxSanity());
    }
    else if (Attribute == GetBatteryAttribute())
    {
        NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxBattery());
    }
}

void USereneAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    // Clamp base value after instant effects
    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
    }
    // ... same for other attributes
}
```

### Pattern 5: UI Binding via Delegates
**What:** Use GetGameplayAttributeValueChangeDelegate for reactive UI updates.
**When to use:** Binding HUD elements to attribute values.

```cpp
// In PlayerController or UI Widget setup
void ASerenePlayerController::SetupAttributeListeners()
{
    if (ASerenePlayerState* PS = GetPlayerState<ASerenePlayerState>())
    {
        UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
        USereneAttributeSet* AS = PS->GetAttributeSet();

        // Bind Health changes
        ASC->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute())
            .AddUObject(this, &ASerenePlayerController::OnHealthChanged);

        // Bind Stamina changes
        ASC->GetGameplayAttributeValueChangeDelegate(AS->GetStaminaAttribute())
            .AddUObject(this, &ASerenePlayerController::OnStaminaChanged);

        // ... same for Sanity, Battery
    }
}

void ASerenePlayerController::OnHealthChanged(const FOnAttributeChangeData& Data)
{
    // Data.NewValue, Data.OldValue available
    // Update HUD
    if (HorrorUI)
    {
        HorrorUI->UpdateHealthBar(Data.NewValue / MaxHealth);
    }
}
```

### Anti-Patterns to Avoid
- **Treating BaseValue as MaxValue:** BaseValue is the permanent value, not a cap. Use separate MaxHealth attribute.
- **Clamping in PreAttributeChange only:** Instant effects bypass PreAttributeChange for BaseValue changes. Must also clamp in PostGameplayEffectExecute.
- **Polling attributes:** Use delegates, not Tick-based polling, for attribute changes.
- **Hand-rolling stamina timers:** The existing HorrorCharacter uses manual timers. Replace with GAS periodic effects for consistency.

## Don't Hand-Roll

Problems that look simple but have existing solutions:

| Problem | Don't Build | Use Instead | Why |
|---------|-------------|-------------|-----|
| Attribute storage | Float variables with manual setters | FGameplayAttributeData + AttributeSet | Built-in replication support, standardized access patterns |
| Attribute clamping | Manual clamping in setters | PreAttributeChange + PostGameplayEffectExecute | GAS-aware, handles all modification sources |
| Attribute change events | Custom delegates per attribute | GetGameplayAttributeValueChangeDelegate | GAS provides this, consistent API |
| Stamina drain/regen | Timer-based drain (current HorrorCharacter) | Infinite GameplayEffect with Period | Unified system, cleaner code, easier tuning via data assets |
| Attribute initialization | Setting values in BeginPlay | Instant GameplayEffect | Data-driven, consistent with GAS patterns |
| Attribute state tracking | Boolean flags (bIsLowHealth) | Gameplay Tags applied via GameplayEffect | Data-driven thresholds, queryable state |

**Key insight:** GAS provides standardized solutions for attribute management. Custom solutions create technical debt and miss integration benefits (prediction, replication readiness, debugging tools).

## Common Pitfalls

### Pitfall 1: Forgetting InitAbilityActorInfo
**What goes wrong:** ASC doesn't know its Owner/Avatar, abilities and effects fail silently.
**Why it happens:** Required call is easy to miss, especially with ASC on PlayerState.
**How to avoid:** Call InitAbilityActorInfo in Character::PossessedBy (server) after PlayerState is valid.
**Warning signs:** GetAbilitySystemComponent returns valid but nothing works; no attribute changes.

### Pitfall 2: Modifying Attributes Directly
**What goes wrong:** Bypasses GameplayEffects, breaks clamping, no events fire.
**Why it happens:** Tempting to call SetHealth() directly for quick tests.
**How to avoid:** Always use Gameplay Effects, even for initialization (instant effects).
**Warning signs:** UI doesn't update when attribute "changes".

### Pitfall 3: Confusion Between BaseValue and CurrentValue
**What goes wrong:** Expecting Duration effects to persist after removal; treating MaxHealth as BaseValue.
**Why it happens:** Mental model mismatch with GAS design.
**How to avoid:**
- Instant effects modify BaseValue (permanent)
- Duration/Infinite effects modify CurrentValue (temporary)
- MaxHealth is a separate attribute, not BaseValue
**Warning signs:** Buffs don't stack as expected; health doesn't return to pre-buff value.

### Pitfall 4: AttributeSet Not Registering
**What goes wrong:** GetNumericAttribute returns 0, effects have no target.
**Why it happens:** AttributeSet created after ASC initialization or not created as subobject.
**How to avoid:** Create AttributeSet in constructor with CreateDefaultSubobject, on same actor as ASC.
**Warning signs:** Effects apply but attributes stay at 0.

### Pitfall 5: Infinite Loops in Attribute Change Handling
**What goes wrong:** Crash or freeze when attribute changes.
**Why it happens:** PreAttributeChange or delegate handler modifies the same attribute.
**How to avoid:** Only clamp values, don't trigger new effects from within attribute handlers.
**Warning signs:** Stack overflow on attribute modification.

### Pitfall 6: Ignoring Single-Player Simplification
**What goes wrong:** Over-engineering for replication in a single-player game.
**Why it happens:** GAS documentation emphasizes multiplayer patterns.
**How to avoid:** SetIsReplicated(false) on ASC, use "Local Only" net execution policy, skip OnRep functions.
**Warning signs:** Unnecessary complexity, performance overhead from unused replication.

## Code Examples

Verified patterns from official sources and community documentation:

### Gameplay Effect: Instant Attribute Initialization
```cpp
// Create in Editor as Blueprint: GE_InitializeAttributes
// Duration Policy: Instant
// Modifiers:
//   - Attribute: SereneAttributeSet.Health, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.MaxHealth, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.Stamina, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.MaxStamina, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.Sanity, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.MaxSanity, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.Battery, ModifierOp: Override, Magnitude: 100
//   - Attribute: SereneAttributeSet.MaxBattery, ModifierOp: Override, Magnitude: 100

// Apply in C++:
void ASerenePlayerState::InitializeAttributes()
{
    if (AbilitySystemComponent && DefaultAttributeEffect)
    {
        FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
        FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, ContextHandle);

        if (SpecHandle.IsValid())
        {
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
        }
    }
}
```

### Gameplay Effect: Periodic Stamina Regen (Infinite with Period)
```cpp
// Create in Editor as Blueprint: GE_StaminaRegen
// Duration Policy: Infinite
// Period: 0.1 (executes every 100ms)
// Modifiers:
//   - Attribute: SereneAttributeSet.Stamina
//   - ModifierOp: Add
//   - Magnitude: 1 (adjust for desired regen rate)
// Application Requirements:
//   - Must NOT have tag: State.Sprinting
```

### Gameplay Effect: Stamina Drain While Sprinting
```cpp
// Create in Editor as Blueprint: GE_SprintStaminaDrain
// Duration Policy: Infinite
// Period: 0.1
// Modifiers:
//   - Attribute: SereneAttributeSet.Stamina
//   - ModifierOp: Add
//   - Magnitude: -3 (adjust for desired drain rate)
// Granted Tags: State.Sprinting
// Ongoing Tag Requirements:
//   - Must have tag: State.Sprinting
```

### Native Gameplay Tags Definition
```cpp
// SereneGameplayTags.h
#pragma once

#include "NativeGameplayTags.h"

namespace SereneGameplayTags
{
    // Attribute state tags
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowHealth);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowStamina);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowSanity);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowBattery);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

    // Action state tags
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sprinting);
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Recovering);
}

// SereneGameplayTags.cpp
#include "SereneGameplayTags.h"

namespace SereneGameplayTags
{
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowHealth, "State.LowHealth", "Health below 30%");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowStamina, "State.LowStamina", "Stamina below 20%");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowSanity, "State.LowSanity", "Sanity below critical threshold");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowBattery, "State.LowBattery", "Battery below 20%");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "Health reached zero");

    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sprinting, "State.Sprinting", "Character is sprinting");
    UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Recovering, "State.Recovering", "Stamina is recovering after depletion");
}
```

### Applying Tags Based on Attribute Thresholds
```cpp
void USereneAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);

    UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();

    if (Data.EvaluatedData.Attribute == GetHealthAttribute())
    {
        float NewHealth = FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth());
        SetHealth(NewHealth);

        // Apply/remove LowHealth tag based on threshold
        float HealthPercent = NewHealth / GetMaxHealth();
        if (HealthPercent <= 0.3f)
        {
            ASC->AddLooseGameplayTag(SereneGameplayTags::State_LowHealth);
        }
        else
        {
            ASC->RemoveLooseGameplayTag(SereneGameplayTags::State_LowHealth);
        }

        // Handle death
        if (NewHealth <= 0.0f)
        {
            ASC->AddLooseGameplayTag(SereneGameplayTags::State_Dead);
        }
    }
    // Similar for other attributes...
}
```

## State of the Art

| Old Approach | Current Approach | When Changed | Impact |
|--------------|------------------|--------------|--------|
| Non-instanced abilities | Instanced abilities required | UE 5.5 | Minor - default is now instanced |
| Manual attribute replication | Built-in DOREPLIFETIME macros | Always in GAS | Use if/when multiplayer needed |
| Behavior Trees for AI | State Trees | UE 5.0+ | Project already uses State Trees |

**Deprecated/outdated:**
- Non-instanced GameplayAbilities: Deprecated in UE 5.5, must use instanced abilities

## Integration with Existing Code

### Replacing HorrorCharacter Stamina System
The existing HorrorCharacter has:
- `float SprintMeter` - Replace with GAS Stamina attribute
- `FTimerHandle SprintTimer` with `SprintFixedTick()` - Replace with Infinite GameplayEffects (regen/drain)
- `OnSprintMeterUpdated` delegate - Replace with GetGameplayAttributeValueChangeDelegate
- `bSprinting`, `bRecovering` flags - Replace with Gameplay Tags (State.Sprinting, State.Recovering)

The existing delegates (FUpdateSprintMeterDelegate, FSprintStateChangedDelegate) can be preserved for backwards compatibility with existing Blueprint UI, but internally they should be driven by GAS attribute changes.

### HorrorUI Integration
The existing HorrorUI already has:
- `BP_SprintMeterUpdated(float Percent)` - Can still be called from GAS delegate handler
- New methods needed: `BP_HealthUpdated`, `BP_SanityUpdated`, `BP_BatteryUpdated`

## Open Questions

Things that couldn't be fully resolved:

1. **Overfill Behavior (Above 100)**
   - What we know: CONTEXT.md specifies attributes can overfill above 100 temporarily
   - What's unclear: Should MaxHealth/etc be dynamically raised, or should clamping be conditional?
   - Recommendation: Use a separate "OverfillAmount" or temporarily raise Max attribute, then apply decay effect

2. **Grace Period Implementation**
   - What we know: 2-3 second delay before drain/regen starts
   - What's unclear: Best GAS pattern for delayed effect activation
   - Recommendation: Use GameplayEffect with "Initial Duration" or apply with delay using Gameplay Tasks

## Sources

### Primary (HIGH confidence)
- [Epic Official GAS Documentation (UE 5.7)](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)
- [Gameplay Attributes and Attribute Sets (UE 5.7)](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-attributes-and-attribute-sets-for-the-gameplay-ability-system-in-unreal-engine)
- [Using Gameplay Tags (UE 5.7)](https://dev.epicgames.com/documentation/en-us/unreal-engine/using-gameplay-tags-in-unreal-engine)
- [Gameplay Effects Documentation (UE 5.7)](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-effects-for-the-gameplay-ability-system-in-unreal-engine)

### Secondary (MEDIUM confidence)
- [GASDocumentation GitHub (tranek)](https://github.com/tranek/GASDocumentation) - Community resource, current with UE 5.3
- [GAS Companion Documentation](https://gascompanion.github.io/) - Plugin docs with good patterns
- [Quod Soler GAS Tutorials](https://www.quodsoler.com/blog/making-sense-of-gameplay-effect-durations) - Duration policies explained
- [Simple GAS Tutorial (landelare)](https://landelare.github.io/2024/01/15/simple-gas-tutorial.html) - Minimal setup guide

### Tertiary (LOW confidence)
- WebSearch results for UE5 GAS patterns 2025 - General patterns confirmed with primary sources

## Metadata

**Confidence breakdown:**
- Standard stack: HIGH - Official Epic documentation confirms modules
- Architecture (ASC on PlayerState): HIGH - Official best practices tutorial, GASDocumentation
- AttributeSet patterns: HIGH - Official documentation with code examples
- Gameplay Effect patterns: HIGH - Official documentation
- UI binding: HIGH - GetGameplayAttributeValueChangeDelegate in official docs
- Pitfalls: HIGH - GASDocumentation extensively covers these

**Research date:** 2026-01-20
**Valid until:** 2026-03-20 (60 days - GAS is stable, major patterns unlikely to change)
