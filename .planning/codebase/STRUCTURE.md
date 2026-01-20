# Codebase Structure

**Analysis Date:** 2026-01-19

## Directory Layout

```
ProjectSerene/
├── Binaries/                    # Compiled game binaries
│   └── Win64/                   # Windows 64-bit builds
├── Config/                      # Engine and project configuration
├── Content/                     # Unreal assets (Blueprints, maps, meshes, materials)
│   ├── Characters/              # Character meshes, animations, materials
│   ├── Collections/             # Asset collections
│   ├── Developers/              # Per-developer sandbox content
│   ├── FirstPerson/             # Base first-person template assets
│   ├── Input/                   # Enhanced Input action/mapping assets
│   ├── LevelPrototyping/        # Prototype level geometry and interactables
│   ├── Variant_Horror/          # Horror game variant assets
│   ├── Variant_Shooter/         # Shooter game variant assets
│   ├── Weapons/                 # Weapon meshes, materials, audio
│   ├── __ExternalActors__/      # World Partition external actor data
│   └── __ExternalObjects__/     # World Partition external object data
├── Intermediate/                # Build intermediate files (generated)
├── Source/                      # C++ source code
│   ├── ProjectSerene/           # Main game module
│   ├── ProjectSerene.Target.cs  # Game build target
│   └── ProjectSereneEditor.Target.cs  # Editor build target
└── ProjectSerene.uproject       # Project descriptor
```

## Directory Purposes

**Source/ProjectSerene/:**
- Purpose: Core C++ game module with base classes
- Contains: Abstract character, game mode, player controller, camera manager
- Key files: `ProjectSerene.h`, `ProjectSerene.cpp`, `ProjectSerene.Build.cs`, `ProjectSereneCharacter.h/cpp`, `ProjectSereneGameMode.h/cpp`, `ProjectSerenePlayerController.h/cpp`, `ProjectSereneCameraManager.h/cpp`

**Source/ProjectSerene/Variant_Horror/:**
- Purpose: Horror game variant C++ implementation
- Contains: Horror character with sprinting, game mode, player controller
- Key files: `HorrorCharacter.h/cpp`, `HorrorGameMode.h/cpp`, `HorrorPlayerController.h/cpp`

**Source/ProjectSerene/Variant_Horror/UI/:**
- Purpose: Horror variant UI widgets
- Contains: Sprint meter display widget
- Key files: `HorrorUI.h/cpp`

**Source/ProjectSerene/Variant_Shooter/:**
- Purpose: Shooter game variant C++ implementation
- Contains: Shooter character with weapons and health, game mode, player controller
- Key files: `ShooterCharacter.h/cpp`, `ShooterGameMode.h/cpp`, `ShooterPlayerController.h/cpp`

**Source/ProjectSerene/Variant_Shooter/AI/:**
- Purpose: Enemy AI logic for shooter variant
- Contains: AI controller, NPC character, spawner, StateTree tasks/conditions, EQS contexts
- Key files: `ShooterAIController.h/cpp`, `ShooterNPC.h/cpp`, `ShooterNPCSpawner.h/cpp`, `ShooterStateTreeUtility.h/cpp`, `EnvQueryContext_Target.h/cpp`

**Source/ProjectSerene/Variant_Shooter/UI/:**
- Purpose: Shooter variant UI widgets
- Contains: Scoreboard and bullet counter displays
- Key files: `ShooterUI.h/cpp`, `ShooterBulletCounterUI.h/cpp`

**Source/ProjectSerene/Variant_Shooter/Weapons/:**
- Purpose: Weapon system implementation
- Contains: Weapon base class, projectiles, pickups, weapon holder interface
- Key files: `ShooterWeapon.h/cpp`, `ShooterProjectile.h/cpp`, `ShooterPickup.h/cpp`, `ShooterWeaponHolder.h`

**Content/FirstPerson/:**
- Purpose: Base template assets shared across variants
- Contains: Base level, animations, materials, Blueprints
- Key files: `Lvl_FirstPerson.umap`, `Blueprints/BP_FirstPersonCharacter.uasset`, `Blueprints/BP_FirstPersonGameMode.uasset`, `Blueprints/BP_FirstPersonPlayerController.uasset`

**Content/Variant_Horror/:**
- Purpose: Horror variant Blueprint assets and content
- Contains: Horror level, character/game mode Blueprints, UI widgets, input mappings
- Key files: `Lvl_Horror.umap`, `Blueprints/BP_HorrorCharacter.uasset`, `Blueprints/BP_HorrorGameMode.uasset`, `Blueprints/BP_HorrorPlayerController.uasset`, `UI/UI_Horror.uasset`, `Input/IMC_Horror.uasset`

**Content/Variant_Shooter/:**
- Purpose: Shooter variant Blueprint assets and content
- Contains: Shooter level, character/game mode Blueprints, AI assets, weapon Blueprints, UI widgets
- Key files: `Lvl_Shooter.umap`, `Blueprints/BP_ShooterCharacter.uasset`, `Blueprints/BP_ShooterGameMode.uasset`, `Blueprints/AI/BP_ShooterNPC.uasset`, `Blueprints/AI/ST_Shooter.uasset`, `Blueprints/Pickups/Weapons/BP_ShooterWeapon_*.uasset`

**Content/Weapons/:**
- Purpose: Weapon art assets (meshes, materials, textures, audio)
- Contains: Pistol, Rifle, GrenadeLauncher subfolders with assets
- Key files: Mesh, material, and texture assets per weapon type

**Content/Characters/Mannequins/:**
- Purpose: Character skeletal meshes, animations, and materials
- Contains: Manny/Quinn mannequin variants with animations for weapons and movement
- Key files: Animations in `Anims/` subfolders organized by weapon type (Pistol, Rifle, Unarmed)

**Content/Input/:**
- Purpose: Shared Enhanced Input assets
- Contains: Input action definitions and touch controls
- Key files: `Actions/` folder with input action assets

**Config/:**
- Purpose: INI configuration files
- Contains: Engine, game, input, editor settings
- Key files: `DefaultEngine.ini`, `DefaultGame.ini`, `DefaultInput.ini`, `DefaultEditor.ini`

## Key File Locations

**Entry Points:**
- `ProjectSerene.uproject`: Project descriptor, plugins, module definitions
- `Source/ProjectSerene.Target.cs`: Game build target configuration
- `Source/ProjectSereneEditor.Target.cs`: Editor build target configuration

**Configuration:**
- `Config/DefaultEngine.ini`: Engine settings
- `Config/DefaultGame.ini`: Game project settings
- `Config/DefaultInput.ini`: Input bindings
- `Source/ProjectSerene/ProjectSerene.Build.cs`: Module build configuration and dependencies

**Core Logic:**
- `Source/ProjectSerene/ProjectSereneCharacter.h/cpp`: Base first-person character
- `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.h/cpp`: Player shooter character
- `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.h/cpp`: Weapon base class
- `Source/ProjectSerene/Variant_Shooter/AI/ShooterAIController.h/cpp`: Enemy AI controller
- `Source/ProjectSerene/Variant_Shooter/AI/ShooterStateTreeUtility.h/cpp`: AI behavior tasks

**Testing:**
- No dedicated test framework detected in this project

**Levels:**
- `Content/FirstPerson/Lvl_FirstPerson.umap`: Base template level
- `Content/Variant_Horror/Lvl_Horror.umap`: Horror game level
- `Content/Variant_Shooter/Lvl_Shooter.umap`: Shooter game level

## Naming Conventions

**Files:**
- C++ source: `PascalCase.h/cpp` matching class name (e.g., `ShooterCharacter.h`)
- Blueprint assets: `BP_` prefix for Blueprints (e.g., `BP_ShooterCharacter.uasset`)
- UI widgets: `UI_` prefix (e.g., `UI_Shooter.uasset`)
- Animation Blueprints: `ABP_` prefix (e.g., `ABP_FP_Pistol.uasset`)
- Data Tables: `DT_` prefix (e.g., `DT_WeaponData.uasset`)
- Input Actions: `IA_` prefix (e.g., `IA_Shoot.uasset`)
- Input Mapping Contexts: `IMC_` prefix (e.g., `IMC_Weapons.uasset`)
- StateTree assets: `ST_` prefix (e.g., `ST_Shooter.uasset`)
- EQS queries: `EQS_` prefix (e.g., `EQS_FindRoamLocation.uasset`)
- Levels: `Lvl_` prefix (e.g., `Lvl_Shooter.umap`)
- Materials: `M_` or `MI_` prefix for materials/material instances

**Directories:**
- Variant folders: `Variant_` prefix (e.g., `Variant_Shooter/`)
- Subsystem folders: PascalCase (e.g., `Weapons/`, `AI/`, `UI/`)
- Asset type folders: PascalCase plural (e.g., `Blueprints/`, `Materials/`, `Meshes/`)

**C++ Classes:**
- Actors: `A` prefix (e.g., `AShooterCharacter`)
- UObjects/Widgets: `U` prefix (e.g., `UShooterUI`)
- Interfaces: `I` prefix (e.g., `IShooterWeaponHolder`)
- Structs: `F` prefix (e.g., `FWeaponTableRow`)
- Components: `U` prefix with `Component` suffix (e.g., `UPawnNoiseEmitterComponent`)

## Where to Add New Code

**New Game Variant:**
- C++ classes: Create `Source/ProjectSerene/Variant_NewVariant/` directory
- Add include path in `Source/ProjectSerene/ProjectSerene.Build.cs` PublicIncludePaths
- Blueprint assets: Create `Content/Variant_NewVariant/` with `Blueprints/`, `UI/`, `Input/` subfolders
- Create level: `Content/Variant_NewVariant/Lvl_NewVariant.umap`

**New Weapon:**
- C++ class (if needed): Add to `Source/ProjectSerene/Variant_Shooter/Weapons/`
- Blueprint: Create `Content/Variant_Shooter/Blueprints/Pickups/Weapons/BP_ShooterWeapon_NewWeapon.uasset`
- Add entry to `Content/Variant_Shooter/Blueprints/Pickups/DT_WeaponData.uasset`
- Mesh/materials: Add to `Content/Weapons/NewWeapon/`

**New AI Behavior:**
- StateTree tasks/conditions: Add structs to `Source/ProjectSerene/Variant_Shooter/AI/ShooterStateTreeUtility.h/cpp`
- EQS queries: Create `Content/Variant_Shooter/Blueprints/AI/EQS_NewQuery.uasset`
- Modify StateTree: Edit `Content/Variant_Shooter/Blueprints/AI/ST_Shooter.uasset`

**New UI Widget:**
- C++ base class: Add to appropriate `Variant_*/UI/` folder
- Blueprint widget: Create in `Content/Variant_*/UI/`
- Mark C++ class as `UCLASS(abstract)` and implement logic in Blueprint

**New Input Action:**
- Create action: `Content/Variant_*/Input/Actions/IA_NewAction.uasset`
- Add to mapping context: `Content/Variant_*/Input/IMC_*.uasset`
- Bind in PlayerController or Character `SetupPlayerInputComponent()`

**Utilities/Shared Code:**
- Add to `Source/ProjectSerene/` root for cross-variant utilities
- Add include path to `ProjectSerene.Build.cs` if creating subdirectory

## Special Directories

**Binaries/:**
- Purpose: Compiled executable and DLL output
- Generated: Yes
- Committed: No (typically gitignored)

**Intermediate/:**
- Purpose: Build system intermediate files, generated code
- Generated: Yes
- Committed: No (typically gitignored)

**Content/__ExternalActors__/:**
- Purpose: World Partition external actor storage (One File Per Actor)
- Generated: Yes (by engine when using World Partition)
- Committed: Yes (contains level actor data)

**Content/__ExternalObjects__/:**
- Purpose: World Partition external object storage
- Generated: Yes (by engine when using World Partition)
- Committed: Yes (contains level object data)

**Content/Developers/:**
- Purpose: Per-developer sandbox content for testing
- Generated: No (manually created)
- Committed: Optional (often gitignored per-developer)

**Saved/:**
- Purpose: Log files, autosaves, local config
- Generated: Yes
- Committed: No (typically gitignored)

---

*Structure analysis: 2026-01-19*
