# Architecture Patterns: Psychological Horror FPS

**Project:** ProjectSerene - Detective PTSD Psychological Horror
**Researched:** 2026-01-19
**Confidence:** MEDIUM-HIGH (patterns verified via official UE5.7 docs and existing template code)

## Executive Summary

The architecture centers on a **SanityComponent** as the heart of the system, broadcasting state changes via delegates to all dependent systems. This follows UE5's component-based architecture with the Observer pattern for loose coupling. The existing Horror template provides a solid foundation with HorrorCharacter, HorrorGameMode, HorrorPlayerController, and HorrorUI - extend rather than replace.

## Recommended Architecture Overview

```
+------------------+     +-------------------+     +--------------------+
|   HorrorGameMode |---->| GameStateManager  |<--->|  SaveGameSubsystem |
+------------------+     | (GameInstance)    |     | (GameInstance)     |
        |                +-------------------+     +--------------------+
        v                        |
+------------------+             v
| HorrorPlayer     |     +-------------------+
| Controller       |<--->|  InvestigationMgr |
+------------------+     |  (GameInstance)   |
        |                +-------------------+
        v                        |
+------------------+             v
|  HorrorCharacter |<----+-------------------+
|                  |     |                   |
| +SanityComponent |<--->| HallucinationMgr  |
| +CombatComponent |     | (World)           |
| +InteractComp    |     +-------------------+
+------------------+             ^
        |                        |
        v                        v
+------------------+     +-------------------+
|    HorrorUI      |     |  HorrorAIManager  |
| +SanityMeter     |     |  (World)          |
| +InvestigationUI |     +-------------------+
| +ObjectiveTracker|
+------------------+
```

## Core Components

### 1. SanityComponent (UActorComponent)

**Responsibility:** Owns sanity state, calculates drain/recovery, broadcasts changes
**Attaches to:** HorrorCharacter
**Confidence:** HIGH (follows established UE5 component patterns)

```cpp
UCLASS(ClassGroup=(Horror), meta=(BlueprintSpawnableComponent))
class USanityComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Attributes
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Sanity")
    float MaxSanity = 100.0f;

    UPROPERTY(BlueprintReadOnly, Category="Sanity")
    float CurrentSanity;

    UPROPERTY(EditAnywhere, Category="Sanity|Drain")
    float DarknessDrainRate = 5.0f;  // Per second in darkness

    UPROPERTY(EditAnywhere, Category="Sanity|Drain")
    float HallucinationDrainRate = 10.0f;  // When viewing hallucinations

    // Thresholds that trigger effects
    UPROPERTY(EditAnywhere, Category="Sanity|Thresholds")
    TArray<FSanityThreshold> Thresholds;

    // Delegates - Observer Pattern
    UPROPERTY(BlueprintAssignable, Category="Sanity|Events")
    FOnSanityChanged OnSanityChanged;  // (float NewValue, float OldValue)

    UPROPERTY(BlueprintAssignable, Category="Sanity|Events")
    FOnSanityThresholdCrossed OnThresholdCrossed;  // (ESanityLevel NewLevel)

    UPROPERTY(BlueprintAssignable, Category="Sanity|Events")
    FOnSanityDepleted OnSanityDepleted;

    // Interface
    UFUNCTION(BlueprintCallable, Category="Sanity")
    void ModifySanity(float Delta, ESanityModifyReason Reason);

    UFUNCTION(BlueprintCallable, Category="Sanity")
    void SetInDarkness(bool bInDarkness);

    UFUNCTION(BlueprintCallable, Category="Sanity")
    void SetInSafeZone(bool bInSafeZone);

    UFUNCTION(BlueprintPure, Category="Sanity")
    ESanityLevel GetCurrentSanityLevel() const;

    UFUNCTION(BlueprintPure, Category="Sanity")
    float GetSanityPercent() const;
};
```

**Why this design:**
- Component-based allows easy attachment to any actor (player, potentially NPCs)
- Delegates enable loose coupling - UI, hallucination system, post-process all subscribe
- Threshold system enables tiered effects (mild -> moderate -> severe -> critical)
- Mirrors existing template pattern (SprintMeter delegates)

### 2. HallucinationManager (UWorldSubsystem)

**Responsibility:** Spawns, tracks, and removes hallucinations based on sanity
**Lifetime:** Per-World (resets between levels)
**Confidence:** MEDIUM (subsystem pattern well-documented, hallucination specifics are custom)

```cpp
UCLASS()
class UHallucinationManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // Configuration
    UPROPERTY(EditAnywhere, Category="Hallucinations")
    TArray<FHallucinationDefinition> HallucinationPool;

    // Active tracking
    UPROPERTY()
    TArray<TObjectPtr<AHallucinationBase>> ActiveHallucinations;

    // Interface
    UFUNCTION(BlueprintCallable, Category="Hallucinations")
    AHallucinationBase* SpawnHallucination(TSubclassOf<AHallucinationBase> Class,
                                            const FTransform& Transform);

    UFUNCTION(BlueprintCallable, Category="Hallucinations")
    void DismissHallucination(AHallucinationBase* Hallucination,
                               EHallucinationDismissReason Reason);

    UFUNCTION(BlueprintCallable, Category="Hallucinations")
    void OnSanityLevelChanged(ESanityLevel NewLevel);

protected:
    void SelectAndSpawnForSanityLevel(ESanityLevel Level);
    void UpdateActiveHallucinations(float DeltaTime);
};
```

**Hallucination Types (Data-Driven):**

| Type | Behavior | Sanity Trigger |
|------|----------|----------------|
| Environmental | Static visual distortion, wrong textures | < 75% |
| Auditory | Whispers, footsteps, false sounds | < 60% |
| Shadow | Fleeting figures at periphery | < 50% |
| Manifestation | Full entities, may interact | < 35% |
| Trauma | PTSD flashbacks (detective specific) | Triggered + < 50% |

### 3. PerceptionModifierComponent (UActorComponent)

**Responsibility:** Applies sanity-driven visual/audio effects
**Attaches to:** HorrorCharacter (or PlayerCameraManager)
**Confidence:** HIGH (Post-process modification is well-documented in UE5.7)

```cpp
UCLASS()
class UPerceptionModifierComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    // Post-process material instances for runtime modification
    UPROPERTY()
    UMaterialInstanceDynamic* SanityPostProcessMaterial;

    // Effect intensity curves mapped to sanity percentage
    UPROPERTY(EditAnywhere, Category="Effects|Visual")
    UCurveFloat* VignetteCurve;

    UPROPERTY(EditAnywhere, Category="Effects|Visual")
    UCurveFloat* ChromaticAberrationCurve;

    UPROPERTY(EditAnywhere, Category="Effects|Visual")
    UCurveFloat* FilmGrainCurve;

    UPROPERTY(EditAnywhere, Category="Effects|Audio")
    USoundMix* LowSanitySoundMix;

public:
    UFUNCTION()
    void OnSanityChanged(float NewSanity, float OldSanity);

    UFUNCTION()
    void OnSanityThresholdCrossed(ESanityLevel NewLevel);

protected:
    void UpdateVisualEffects(float SanityPercent);
    void UpdateAudioEffects(float SanityPercent);
};
```

**Visual Effect Progression:**

| Sanity % | Vignette | Chroma | Grain | Extra |
|----------|----------|--------|-------|-------|
| 100-75 | 0.0 | 0.0 | 0.0 | None |
| 75-50 | 0.2 | 0.1 | 0.2 | Subtle color shift |
| 50-25 | 0.4 | 0.3 | 0.4 | Screen shake on events |
| 25-0 | 0.6+ | 0.5+ | 0.6+ | Distortion, false imagery |

### 4. InvestigationManager (UGameInstanceSubsystem)

**Responsibility:** Tracks clues, evidence connections, case progress
**Lifetime:** Game Instance (persists across levels)
**Confidence:** MEDIUM (pattern documented, detective mechanics custom)

```cpp
UCLASS()
class UInvestigationManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Evidence tracking
    UPROPERTY(BlueprintReadOnly, Category="Investigation")
    TArray<FEvidenceEntry> CollectedEvidence;

    UPROPERTY(BlueprintReadOnly, Category="Investigation")
    TArray<FEvidenceConnection> DiscoveredConnections;

    // Case progress
    UPROPERTY(BlueprintReadOnly, Category="Investigation")
    TMap<FName, FCaseProgress> CaseProgress;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category="Investigation|Events")
    FOnEvidenceCollected OnEvidenceCollected;

    UPROPERTY(BlueprintAssignable, Category="Investigation|Events")
    FOnConnectionDiscovered OnConnectionDiscovered;

    UPROPERTY(BlueprintAssignable, Category="Investigation|Events")
    FOnCaseProgressUpdated OnCaseProgressUpdated;

    // Interface
    UFUNCTION(BlueprintCallable, Category="Investigation")
    void CollectEvidence(const FEvidenceEntry& Evidence);

    UFUNCTION(BlueprintCallable, Category="Investigation")
    bool TryCreateConnection(FName EvidenceA, FName EvidenceB);

    UFUNCTION(BlueprintCallable, Category="Investigation")
    void UpdateCaseProgress(FName CaseID, FName ObjectiveID);

    UFUNCTION(BlueprintPure, Category="Investigation")
    bool HasEvidence(FName EvidenceID) const;

    UFUNCTION(BlueprintPure, Category="Investigation")
    bool IsConnectionValid(FName EvidenceA, FName EvidenceB) const;
};
```

**Evidence System Data Flow:**
```
InteractableEvidence (Actor)
       |
       v [Player Interacts]
InteractionComponent::OnInteract
       |
       v [Validates, Plays Anim]
InvestigationManager::CollectEvidence
       |
       +---> OnEvidenceCollected (Delegate)
       |            |
       |            +--> UI: Show notification
       |            +--> SaveSystem: Auto-save progress
       |            +--> Sanity: Trauma evidence affects sanity
       |
       v [Check Connections]
OnConnectionDiscovered (if auto-detected)
```

### 5. CombatComponent (UActorComponent)

**Responsibility:** Manages weapon state, firing, damage dealing
**Attaches to:** HorrorCharacter
**Confidence:** HIGH (follows existing ShooterWeaponHolder interface pattern)

```cpp
UCLASS()
class UCombatComponent : public UActorComponent, public IShooterWeaponHolder
{
    GENERATED_BODY()

public:
    // Weapon slots (limited for horror: pistol + melee)
    UPROPERTY(EditAnywhere, Category="Combat")
    TSubclassOf<AHorrorWeapon> PistolClass;

    UPROPERTY(EditAnywhere, Category="Combat")
    TSubclassOf<AHorrorWeapon> MeleeClass;

    UPROPERTY(BlueprintReadOnly, Category="Combat")
    AHorrorWeapon* CurrentWeapon;

    // Sanity integration
    UPROPERTY(EditAnywhere, Category="Combat|Sanity")
    float AimSwayMultiplierAtLowSanity = 2.0f;

    UPROPERTY(EditAnywhere, Category="Combat|Sanity")
    float ReloadSpeedMultiplierAtLowSanity = 0.7f;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category="Combat|Events")
    FOnWeaponFired OnWeaponFired;

    UPROPERTY(BlueprintAssignable, Category="Combat|Events")
    FOnAmmoChanged OnAmmoChanged;

    // Interface
    UFUNCTION(BlueprintCallable, Category="Combat")
    void Fire();

    UFUNCTION(BlueprintCallable, Category="Combat")
    void Reload();

    UFUNCTION(BlueprintCallable, Category="Combat")
    void SwitchWeapon();

protected:
    // IShooterWeaponHolder implementation
    virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) override;
    virtual void PlayFiringMontage(UAnimMontage* Montage) override;
    virtual void AddWeaponRecoil(float Recoil) override;
    virtual void UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize) override;
    virtual FVector GetWeaponTargetLocation() override;

    // Sanity-affected calculations
    float GetCurrentAimSway() const;
    float GetCurrentReloadSpeed() const;
};
```

### 6. InteractionComponent (UActorComponent)

**Responsibility:** Detects and manages interactable objects
**Attaches to:** HorrorCharacter
**Confidence:** HIGH (standard UE5 interaction pattern)

```cpp
UCLASS()
class UInteractionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category="Interaction")
    float InteractionRange = 200.0f;

    UPROPERTY(BlueprintReadOnly, Category="Interaction")
    AActor* CurrentInteractable;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
    FOnInteractableFound OnInteractableFound;

    UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
    FOnInteractableLost OnInteractableLost;

    UPROPERTY(BlueprintAssignable, Category="Interaction|Events")
    FOnInteractionComplete OnInteractionComplete;

    // Interface
    UFUNCTION(BlueprintCallable, Category="Interaction")
    void TryInteract();

    UFUNCTION(BlueprintCallable, Category="Interaction")
    void UpdateInteractableTrace();

protected:
    UPROPERTY()
    TScriptInterface<IInteractableInterface> CurrentInteractableInterface;
};
```

### 7. HorrorAIManager (UWorldSubsystem)

**Responsibility:** Coordinates AI behaviors, spawning, global AI state
**Lifetime:** Per-World
**Confidence:** MEDIUM (extends existing ShooterAI patterns with StateTree)

```cpp
UCLASS()
class UHorrorAIManager : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // AI registry
    UPROPERTY()
    TArray<TObjectPtr<AHorrorAICharacter>> ActiveAI;

    // Global AI state (affects all AI behavior)
    UPROPERTY(BlueprintReadWrite, Category="AI")
    EGlobalThreatLevel CurrentThreatLevel;

    UPROPERTY(BlueprintReadWrite, Category="AI")
    bool bPlayerDetected;

    // Delegates
    UPROPERTY(BlueprintAssignable, Category="AI|Events")
    FOnThreatLevelChanged OnThreatLevelChanged;

    // Interface
    UFUNCTION(BlueprintCallable, Category="AI")
    void RegisterAI(AHorrorAICharacter* AI);

    UFUNCTION(BlueprintCallable, Category="AI")
    void AlertAllAI(FVector Location);

    UFUNCTION(BlueprintCallable, Category="AI")
    void SetGlobalThreatLevel(EGlobalThreatLevel NewLevel);
};
```

**AI State Machine (StateTree recommended):**
```
[Idle/Patrol]
      |
      v (Sound/Sight stimulus)
[Investigate]
      |
      +---> (Nothing found) --> [Return to Patrol]
      |
      v (Player spotted)
[Alert/Chase]
      |
      +---> (Lost player) --> [Search]
      |
      v (In range)
[Attack]
      |
      v (Player escaped/defeated)
[Return to Patrol]
```

## Data Flow Diagrams

### Sanity System Data Flow

```
                      +------------------+
                      |  SANITY SOURCES  |
                      +------------------+
                             |
    +------------+-----------|----------+------------+
    |            |           |          |            |
    v            v           v          v            v
[Darkness]  [Hallucinations] [Trauma]  [Combat]   [Safe Zone]
   -5/s         -10/s        -20       -3/event    +8/s
    |            |           |          |            |
    +------------+-----------+----------+------------+
                             |
                             v
                    +------------------+
                    | SanityComponent  |
                    | CurrentSanity    |
                    +------------------+
                             |
           OnSanityChanged / OnThresholdCrossed
                             |
    +------------+-----------+-----------+------------+
    |            |           |           |            |
    v            v           v           v            v
[HorrorUI]  [Perception]  [Hallucin-] [Combat]   [AI Manager]
            [Modifier]    [ationMgr]  [Component]
    |            |           |           |            |
    v            v           v           v            v
Update      Post-Process  Spawn/      Aim Sway    Behavior
Sanity      Effects       Dismiss     Adjust      Modifiers
Meter
```

### Investigation System Data Flow

```
+------------------+
| EVIDENCE SOURCES |
+------------------+
        |
   [World Actors]
        |
        v
+-------------------+      +------------------+
| InteractionComp   |----->| InvestigationMgr |
| TryInteract()     |      | CollectEvidence()|
+-------------------+      +------------------+
                                   |
                    +--------------+--------------+
                    |              |              |
                    v              v              v
             [Evidence Board] [Case Progress] [Save System]
                    |              |              |
                    v              v              v
              Connection      Objective       Persist
              Detection       Tracking        State
```

### Level/Location System

```
+---------------------+
|   PERSISTENT LEVEL  |
| (HorrorGameMode)    |
| (Player, UI, etc.)  |
+---------------------+
          |
    Level Streaming
          |
    +-----+-----+-----+-----+
    |     |     |     |     |
    v     v     v     v     v
[Crime] [Apt] [Pre-] [Cult] [...]
[Scene] [ment] [cinct] [HQ]

Each sublevel contains:
- Location-specific actors
- Trigger volumes (sanity zones)
- Hallucination spawn points
- AI patrol paths
- Evidence/Interactables
```

## Component Dependencies & Build Order

### Dependency Graph

```
Layer 0 (Foundation - No Dependencies):
  - Enums/Structs (ESanityLevel, FEvidenceEntry, etc.)
  - Interfaces (IInteractableInterface)
  - Data Assets (HallucinationDefinition, WeaponData)

Layer 1 (Core Components - Minimal Dependencies):
  - SanityComponent (Layer 0 only)
  - InteractionComponent (Layer 0 only)
  - CombatComponent (Layer 0 only, existing weapon interface)

Layer 2 (Manager Systems - Depend on Layer 1):
  - InvestigationManager (depends on evidence structs)
  - HallucinationManager (depends on SanityComponent events)
  - PerceptionModifierComponent (depends on SanityComponent events)

Layer 3 (Integration - Ties Everything Together):
  - HorrorCharacter extensions (integrates Layer 1 components)
  - HorrorUI extensions (subscribes to Layer 1-2 events)
  - HorrorAIManager (responds to Layer 1-2 state)

Layer 4 (Content - Uses All Systems):
  - Levels/Locations
  - Hallucination Actors
  - Evidence Actors
  - AI Characters
```

### Recommended Build Order

| Phase | Components | Rationale |
|-------|------------|-----------|
| 1 | SanityComponent, PerceptionModifier | Core loop - sanity affects perception |
| 2 | InteractionComponent, basic interactables | Player can interact with world |
| 3 | InvestigationManager, evidence system | Detective gameplay loop |
| 4 | HallucinationManager, hallucination actors | Horror layer on top of sanity |
| 5 | CombatComponent, weapons | Combat when needed |
| 6 | HorrorAIManager, AI characters | Threats that drive sanity/combat |
| 7 | Level streaming, locations | Multiple discrete areas |

**Phase dependency notes:**
- Phase 1 must complete first - everything keys off sanity
- Phases 2-3 can partially parallel (both are player-world interaction)
- Phase 4 requires Phase 1 (sanity triggers hallucinations)
- Phase 5 can be developed in parallel with 3-4
- Phase 6 requires Phase 1 (AI affects/affected by sanity)
- Phase 7 requires 1-4 functional for meaningful testing

## Patterns to Follow

### Pattern 1: Observer via Delegates (Recommended)

**What:** Components broadcast state changes via `DECLARE_DYNAMIC_MULTICAST_DELEGATE`
**When:** Any time one system's state affects multiple other systems
**Why:** Loose coupling, Blueprint-friendly, follows template pattern

```cpp
// Declaration
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSanityChanged, float, NewValue, float, OldValue);

// In component
UPROPERTY(BlueprintAssignable, Category="Sanity|Events")
FOnSanityChanged OnSanityChanged;

// Broadcasting
OnSanityChanged.Broadcast(CurrentSanity, OldSanity);

// Subscribing (in other component's BeginPlay)
SanityComponent->OnSanityChanged.AddDynamic(this, &UPerceptionModifier::OnSanityChanged);
```

### Pattern 2: Subsystems for Managers

**What:** Use `UGameInstanceSubsystem` or `UWorldSubsystem` for manager classes
**When:** Global state that doesn't belong to a specific actor
**Why:** Automatic lifecycle management, easy access from anywhere

```cpp
// Access pattern
UInvestigationManager* InvestMgr = GetGameInstance()->GetSubsystem<UInvestigationManager>();
UHallucinationManager* HalluMgr = GetWorld()->GetSubsystem<UHallucinationManager>();
```

### Pattern 3: Data-Driven Configuration

**What:** Use Data Assets for configurable content
**When:** Hallucination definitions, sanity thresholds, evidence definitions
**Why:** Designer-friendly, no code changes for content iteration

```cpp
UCLASS()
class UHallucinationDataAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere)
    TSubclassOf<AHallucinationBase> HallucinationClass;

    UPROPERTY(EditAnywhere)
    ESanityLevel MinSanityToSpawn;

    UPROPERTY(EditAnywhere)
    float SpawnWeight;

    UPROPERTY(EditAnywhere)
    TArray<FName> ValidLocationTags;
};
```

### Pattern 4: Interface-Based Interactions

**What:** Use `UINTERFACE` for interaction contracts
**When:** Multiple actor types need same interaction capability
**Why:** Decouples interaction logic from specific classes

```cpp
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
    GENERATED_BODY()
};

class IInteractableInterface
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool CanInteract(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnInteract(AActor* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FText GetInteractionPrompt();
};
```

## Anti-Patterns to Avoid

### Anti-Pattern 1: Direct References Between Systems

**What:** Systems holding direct pointers to each other
**Why bad:** Tight coupling, initialization order dependencies, hard to test
**Instead:** Use delegates, subsystem lookups, or interfaces

```cpp
// BAD
class USanityComponent
{
    UHallucinationManager* HallucinationMgr;  // Direct reference
    void OnSanityLow() { HallucinationMgr->SpawnHallucination(); }
};

// GOOD
class USanityComponent
{
    FOnSanityThresholdCrossed OnThresholdCrossed;  // Delegate
};
// HallucinationManager subscribes to delegate
```

### Anti-Pattern 2: Polling Instead of Events

**What:** Checking state every tick instead of responding to changes
**Why bad:** Performance waste, can miss rapid changes
**Instead:** Subscribe to delegate events

```cpp
// BAD
void UHorrorUI::Tick(float DeltaTime)
{
    float Sanity = Character->SanityComponent->GetSanity();  // Every frame
    UpdateMeter(Sanity);
}

// GOOD
void UHorrorUI::SetupCharacter(AHorrorCharacter* Char)
{
    Char->SanityComponent->OnSanityChanged.AddDynamic(this, &UHorrorUI::OnSanityChanged);
}
```

### Anti-Pattern 3: Monolithic GameMode

**What:** Putting all game logic in GameMode
**Why bad:** Becomes unmaintainable, hard to test individual systems
**Instead:** Use components and subsystems, GameMode just coordinates

### Anti-Pattern 4: Hard-Coded Thresholds

**What:** Magic numbers in code for sanity levels, spawn rates, etc.
**Why bad:** Requires code changes for tuning, not designer-friendly
**Instead:** Use `UPROPERTY`, curves, or data assets

## GameplayAbilitySystem Consideration

**Recommendation:** Do NOT use GAS for this project.

**Rationale:**
- GAS is designed for complex ability interactions (RPGs, MOBAs)
- This project has simple, linear sanity degradation/recovery
- Combat is minimal (single pistol, basic melee)
- GAS adds significant complexity without proportional benefit
- Custom components with delegates are simpler and sufficient

**When GAS would be appropriate:**
- Multiple ability types with cooldowns
- Complex buff/debuff stacking rules
- Multiplayer with ability prediction
- Character classes with varied ability sets

For ProjectSerene, a simple attribute-based SanityComponent with float values and threshold checks is more maintainable than full GAS integration.

## Sources

### Official Documentation (HIGH confidence)
- [Post Process Effects in UE](https://dev.epicgames.com/documentation/en-us/unreal-engine/post-process-effects-in-unreal-engine)
- [Gameplay Ability System](https://dev.epicgames.com/documentation/en-us/unreal-engine/gameplay-ability-system-for-unreal-engine)
- [Event Dispatchers](https://dev.epicgames.com/documentation/en-us/unreal-engine/event-dispatchers-in-unreal-engine)
- [Programming Subsystems](https://dev.epicgames.com/documentation/en-us/unreal-engine/programming-subsystems-in-unreal-engine)
- [StateTree Overview](https://dev.epicgames.com/documentation/en-us/unreal-engine/state-tree-in-unreal-engine)
- [AI Perception](https://dev.epicgames.com/documentation/en-us/unreal-engine/ai-perception-in-unreal-engine)
- [Level Streaming](https://dev.epicgames.com/documentation/en-us/unreal-engine/level-streaming-in-unreal-engine)

### Community/Industry (MEDIUM confidence)
- [GAS Documentation (tranek)](https://github.com/tranek/GASDocumentation)
- [Amnesia Sanity Meter Deep Dive](https://www.gamedeveloper.com/design/game-design-deep-dive-i-amnesia-i-s-sanity-meter-)
- [The Sinking City 2 UE5 Interview](https://www.unrealengine.com/en-US/developer-interviews/unreal-engine-5-helps-frogwares-deliver-a-different-sort-of-horror-in-the-sinking-city-2)
- [Horror AI Tutorial](https://dev.epicgames.com/community/learning/tutorials/2JzM/horror-ai-unreal-engine-5-tutorial-part-1)
- [Fab Sanity System Pack](https://www.fab.com/listings/0fc989ac-88c1-4a60-87de-6f324c4333b0)

### Existing Codebase (HIGH confidence)
- `HorrorCharacter.h` - Sprint/stamina delegate pattern
- `HorrorUI.h` - UI delegate subscription pattern
- `ShooterWeaponHolder.h` - Interface pattern for weapon systems
- `ShooterAIController.h` - StateTree AI with perception
