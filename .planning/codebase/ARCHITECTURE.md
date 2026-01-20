# Architecture

**Analysis Date:** 2026-01-19

## Pattern Overview

**Overall:** Unreal Engine Game Variant Architecture with Abstract Base Classes

**Key Characteristics:**
- Single C++ module (`ProjectSerene`) with shared base classes and game variant subdirectories
- Abstract base classes define core gameplay framework; concrete implementations live in `Variant_*` folders
- Blueprint-driven configuration layer on top of C++ gameplay logic
- StateTree-based AI behavior for NPC enemies
- Interface-driven weapon system for decoupled weapon/holder interaction

## Layers

**Core Framework Layer:**
- Purpose: Provides abstract base classes for characters, game modes, and player controllers
- Location: `Source/ProjectSerene/`
- Contains: `AProjectSereneCharacter`, `AProjectSereneGameMode`, `AProjectSerenePlayerController`, `AProjectSereneCameraManager`
- Depends on: Unreal Engine core modules (Engine, InputCore, EnhancedInput)
- Used by: All game variants

**Horror Variant Layer:**
- Purpose: Implements horror-style gameplay with stamina-based sprinting and flashlight
- Location: `Source/ProjectSerene/Variant_Horror/`
- Contains: `AHorrorCharacter`, `AHorrorGameMode`, `AHorrorPlayerController`, `UHorrorUI`
- Depends on: Core Framework Layer
- Used by: Horror game mode Blueprints in `Content/Variant_Horror/`

**Shooter Variant Layer:**
- Purpose: Implements first-person shooter gameplay with weapons, AI enemies, and team scoring
- Location: `Source/ProjectSerene/Variant_Shooter/`
- Contains: Character, GameMode, PlayerController, AI, Weapons, UI subsystems
- Depends on: Core Framework Layer, AIModule, StateTreeModule, GameplayStateTreeModule
- Used by: Shooter game mode Blueprints in `Content/Variant_Shooter/`

**Shooter Weapons Subsystem:**
- Purpose: Manages weapon inventory, firing, projectiles, and pickups
- Location: `Source/ProjectSerene/Variant_Shooter/Weapons/`
- Contains: `AShooterWeapon`, `AShooterProjectile`, `AShooterPickup`, `IShooterWeaponHolder`
- Depends on: Core Framework Layer
- Used by: `AShooterCharacter`, `AShooterNPC`

**Shooter AI Subsystem:**
- Purpose: Controls enemy NPC behavior through perception and StateTree
- Location: `Source/ProjectSerene/Variant_Shooter/AI/`
- Contains: `AShooterAIController`, `AShooterNPC`, `AShooterNPCSpawner`, `UEnvQueryContext_Target`, StateTree tasks/conditions
- Depends on: AIModule, StateTreeModule, GameplayStateTreeModule, Weapons Subsystem
- Used by: Shooter game levels

**UI Layer:**
- Purpose: Provides HUD widgets for each game variant
- Location: `Source/ProjectSerene/Variant_Horror/UI/`, `Source/ProjectSerene/Variant_Shooter/UI/`
- Contains: `UHorrorUI`, `UShooterUI`, `UShooterBulletCounterUI`
- Depends on: UMG module
- Used by: Game modes and characters via delegates

## Data Flow

**Player Input Flow:**

1. `AProjectSerenePlayerController::SetupInputComponent()` registers input mapping contexts
2. Enhanced Input system routes input to bound actions
3. `AProjectSereneCharacter` receives input via `MoveInput()`, `LookInput()`, `DoMove()`, `DoAim()`
4. Variant characters override or extend with additional actions (Sprint for Horror, Fire/SwitchWeapon for Shooter)

**Weapon Firing Flow:**

1. `AShooterCharacter::DoStartFiring()` called from input binding
2. Character delegates to `CurrentWeapon->StartFiring()`
3. `AShooterWeapon::Fire()` spawns `AShooterProjectile` and calls `IShooterWeaponHolder` interface methods
4. Interface callbacks update HUD via delegates (`OnBulletCountUpdated`)
5. Projectile collision triggers `ProcessHit()` which applies damage and physics

**AI Perception Flow:**

1. `UAIPerceptionComponent` detects actors via sight/hearing
2. `AShooterAIController::OnPerceptionUpdated()` fires delegate to StateTree
3. `FStateTreeSenseEnemiesTask` processes stimuli and sets target/investigate outputs
4. StateTree transitions states based on outputs (Idle -> Investigate -> Engage)
5. `FStateTreeShootAtTargetTask` triggers `AShooterNPC::StartShooting()`

**State Management:**
- Character state managed through member variables and timers (`bSprinting`, `bIsFiring`, `bIsDead`)
- Game state tracked in `AShooterGameMode` (`TeamScores` map)
- AI state driven by StateTree with output bindings
- UI state updated reactively through dynamic multicast delegates

## Key Abstractions

**IShooterWeaponHolder Interface:**
- Purpose: Decouples weapons from their holders (players or NPCs)
- Examples: `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeaponHolder.h`
- Pattern: Pure virtual interface implemented by both `AShooterCharacter` and `AShooterNPC`
- Methods: `AttachWeaponMeshes()`, `PlayFiringMontage()`, `AddWeaponRecoil()`, `UpdateWeaponHUD()`, `GetWeaponTargetLocation()`

**AProjectSereneCharacter Base:**
- Purpose: Shared first-person character foundation with camera, mesh, and input handling
- Examples: `Source/ProjectSerene/ProjectSereneCharacter.h`
- Pattern: Abstract base class with `UCLASS(abstract)` marker
- Provides: First-person mesh, camera component, movement/look input handling

**StateTree Tasks/Conditions:**
- Purpose: Custom AI behavior building blocks for StateTree graphs
- Examples: `Source/ProjectSerene/Variant_Shooter/AI/ShooterStateTreeUtility.h`
- Pattern: `USTRUCT` types inheriting from `FStateTreeTaskCommonBase` or `FStateTreeConditionCommonBase`
- Includes: `FStateTreeSenseEnemiesTask`, `FStateTreeShootAtTargetTask`, `FStateTreeFaceActorTask`, `FStateTreeLineOfSightToTargetCondition`

**Dynamic Multicast Delegates:**
- Purpose: Event-driven communication between gameplay and UI
- Examples: `FBulletCountUpdatedDelegate`, `FDamagedDelegate`, `FUpdateSprintMeterDelegate`, `FPawnDeathDelegate`
- Pattern: `DECLARE_DYNAMIC_MULTICAST_DELEGATE` macros with Blueprint-bindable events

## Entry Points

**Game Module:**
- Location: `Source/ProjectSerene/ProjectSerene.cpp`
- Triggers: Engine module loading
- Responsibilities: Registers the `ProjectSerene` module, defines `LogProjectSerene` category

**Target Files:**
- Location: `Source/ProjectSerene.Target.cs`, `Source/ProjectSereneEditor.Target.cs`
- Triggers: Build system
- Responsibilities: Configure game and editor build targets

**Game Modes:**
- Location: `Source/ProjectSerene/Variant_Horror/HorrorGameMode.h`, `Source/ProjectSerene/Variant_Shooter/ShooterGameMode.h`
- Triggers: Level loading when set as World Settings game mode
- Responsibilities: Spawn default pawn/controller, manage game UI, track scores

**Level Maps:**
- Location: `Content/FirstPerson/Lvl_FirstPerson.umap`, `Content/Variant_Horror/Lvl_Horror.umap`, `Content/Variant_Shooter/Lvl_Shooter.umap`
- Triggers: Level loading
- Responsibilities: Define world geometry, spawn points, game mode overrides

## Error Handling

**Strategy:** Defensive null checks with early returns, timer-based cleanup for destroyed actors

**Patterns:**
- `IsValid()` checks before pointer dereference
- `EndPlay()` cleanup of timers via `GetWorld()->GetTimerManager().ClearTimer()`
- Owner destruction callbacks via `OnDestroyed` delegate binding
- Death state flags (`bIsDead`) to prevent double-processing

## Cross-Cutting Concerns

**Logging:** Custom log category `LogProjectSerene` declared in `Source/ProjectSerene/ProjectSerene.h`. Secondary category `LogTemplateCharacter` in character header.

**Validation:** UPROPERTY meta tags with `ClampMin`, `ClampMax`, `Units` for editor-time validation. No runtime validation framework.

**Authentication:** Not applicable (single-player/local game)

**AI Perception:** `UAIPerceptionComponent` on `AShooterAIController` with sight/hearing senses. `UPawnNoiseEmitterComponent` on `AShooterCharacter` for noise generation.

**Team System:** Simple `uint8 TeamByte` on characters with actor tags (`DeathTag`) for state identification.

---

*Architecture analysis: 2026-01-19*
