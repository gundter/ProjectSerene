# Technology Stack

**Analysis Date:** 2026-01-19

## Languages

**Primary:**
- C++ (Unreal Engine standard) - All gameplay code in `Source/ProjectSerene/`

**Secondary:**
- C# - Build system files (`*.Build.cs`, `*.Target.cs`)
- Blueprints (visual scripting) - `Content/**/*.uasset`

## Runtime

**Environment:**
- Unreal Engine 5.7
- Windows 11 SDK 22621

**Build Settings:**
- Build Settings Version: V6
- Include Order Version: Unreal5_7
- PCH Usage: UseExplicitOrSharedPCHs

## Frameworks

**Core:**
- Unreal Engine 5.7 - Game engine framework

**AI Systems:**
- AIModule - Core AI functionality
- StateTreeModule - Behavior tree alternative for AI logic
- GameplayStateTreeModule - Gameplay-specific StateTree integration

**Input:**
- EnhancedInput - Modern input handling system

**UI:**
- UMG (Unreal Motion Graphics) - Widget-based UI system
- Slate - Low-level UI framework (public dependency)

**Plugins (Enabled):**
- ModelingToolsEditorMode - Editor-only modeling tools
- StateTree - Core StateTree plugin
- GameplayStateTree - Gameplay integration for StateTree

## Key Dependencies

**Public Module Dependencies:**
- `Core` - Fundamental Unreal types
- `CoreUObject` - UObject system
- `Engine` - Core engine functionality
- `InputCore` - Input fundamentals
- `EnhancedInput` - Enhanced input actions/mappings
- `AIModule` - AI perception, controllers, navigation
- `StateTreeModule` - StateTree behavior system
- `GameplayStateTreeModule` - StateTree gameplay tasks
- `UMG` - Widget-based UI
- `Slate` - UI primitives

**Critical Gameplay Systems:**
- AI Perception (sight, hearing) - `UAIPerceptionComponent`
- State Trees for AI behavior - `UStateTreeAIComponent`
- Environment Query System (EQS) - Context-based location queries
- Character Movement - `ACharacter` base class
- Damage System - `TakeDamage` virtual

## Configuration

**Project Configuration:**
- `ProjectSerene.uproject` - Project definition
- `Config/DefaultEngine.ini` - Engine settings
- `Config/DefaultGame.ini` - Game settings
- `Config/DefaultInput.ini` - Input configuration

**Key Engine Settings:**
- Ray Tracing: Enabled (`r.RayTracing=True`)
- Substrate: Enabled (`r.Substrate=True`)
- Virtual Shadow Maps: Enabled
- Lumen GI: Enabled (`r.DynamicGlobalIlluminationMethod=1`)
- Static Lighting: Disabled
- Mesh Distance Fields: Enabled

**Render Hardware Interface:**
- Windows: DirectX 12 (`DefaultGraphicsRHI_DX12`)
- Shader Model: SM6 (Windows DX12), SM5 (DX11 fallback)
- Linux: Vulkan SM6
- Mac: Metal SM6

**Input Configuration:**
- Default Input Class: `EnhancedPlayerInput`
- Default Input Component: `EnhancedInputComponent`
- Touch Interface: Disabled by default
- Mouse Capture: Permanent on launch

**Custom Collision Channel:**
- `Projectile` (ECC_GameTraceChannel1) - QueryOnly collision for projectiles

## Platform Requirements

**Development:**
- Visual Studio with these workloads (from `.vsconfig`):
  - NativeDesktop
  - NativeGame
  - ManagedDesktop
  - CoreEditor
- .NET Framework 4.6.2 Targeting Pack
- MSVC v14.38 and v14.44 toolsets
- Clang/LLVM compiler support
- Windows 11 SDK 22621

**Supported Platforms:**
- Windows (Win64) - Primary development target
- Linux (Vulkan)
- Mac (Metal)
- Android (via AndroidFileServerEditor plugin)

**Hardware Targets:**
- Targeted Hardware Class: Desktop
- Graphics Performance: Maximum
- Ray tracing capable GPU recommended

## Project Structure

**Module:**
- Name: `ProjectSerene`
- Type: Runtime
- Loading Phase: Default

**Include Paths:**
- `ProjectSerene/` - Base module
- `ProjectSerene/Variant_Horror/` - Horror gameplay variant
- `ProjectSerene/Variant_Horror/UI/` - Horror UI
- `ProjectSerene/Variant_Shooter/` - Shooter gameplay variant
- `ProjectSerene/Variant_Shooter/AI/` - Shooter AI systems
- `ProjectSerene/Variant_Shooter/UI/` - Shooter UI
- `ProjectSerene/Variant_Shooter/Weapons/` - Weapon systems

**Targets:**
- `ProjectSerene.Target.cs` - Game build target
- `ProjectSereneEditor.Target.cs` - Editor build target

---

*Stack analysis: 2026-01-19*
