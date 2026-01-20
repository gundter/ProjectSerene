# Codebase Concerns

**Analysis Date:** 2025-01-19

## Tech Debt

**Duplicated PlayerController Logic:**
- Issue: Three separate PlayerController classes duplicate nearly identical input mapping and touch control logic
- Files:
  - `Source/ProjectSerene/ProjectSerenePlayerController.cpp` (lines 44-70)
  - `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp` (lines 67-92)
  - `Source/ProjectSerene/Variant_Shooter/ShooterPlayerController.cpp` (lines 55-80)
- Impact: Bug fixes or changes must be applied to three places; risk of inconsistent behavior
- Fix approach: Extract shared input mapping logic into base class `AProjectSerenePlayerController`, have variant controllers inherit from it

**HorrorGameMode Empty Implementation:**
- Issue: `AHorrorGameMode` constructor is a stub with no implementation
- Files: `Source/ProjectSerene/Variant_Horror/HorrorGameMode.cpp` (line 7-8)
- Impact: Class provides no value beyond `AGameModeBase`; may indicate incomplete feature
- Fix approach: Either implement Horror-specific game mode logic or remove class and use base directly

**ProjectSereneGameMode Empty Implementation:**
- Issue: `AProjectSereneGameMode` constructor is a stub with no implementation
- Files: `Source/ProjectSerene/ProjectSereneGameMode.cpp` (line 5-8)
- Impact: Class provides no value beyond `AGameModeBase`
- Fix approach: Either implement shared game mode logic or consider if base class inheritance is actually needed

**ShouldUseTouchControls() Duplicated:**
- Issue: Identical `ShouldUseTouchControls()` method implementation in three PlayerController classes
- Files:
  - `Source/ProjectSerene/ProjectSerenePlayerController.cpp` (lines 72-76)
  - `Source/ProjectSerene/Variant_Horror/HorrorPlayerController.cpp` (lines 94-98)
  - `Source/ProjectSerene/Variant_Shooter/ShooterPlayerController.cpp` (lines 149-153)
- Impact: Code duplication; maintenance burden
- Fix approach: Move to base PlayerController class or create utility function

**PlayFiringMontage Not Implemented:**
- Issue: `AShooterCharacter::PlayFiringMontage()` is a stub (comment says "// stub")
- Files: `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.cpp` (lines 182-185)
- Impact: Firing animations not playing for player character weapons
- Fix approach: Implement animation montage playback using the character's AnimInstance

**Unused Interface Methods in ShooterNPC:**
- Issue: Multiple `IShooterWeaponHolder` interface methods are empty stubs in `AShooterNPC`
- Files: `Source/ProjectSerene/Variant_Shooter/AI/ShooterNPC.cpp` (lines 68-82, 126-139)
- Impact: Dead code; inconsistent behavior between player and NPC weapon holders
- Fix approach: Either implement NPC-specific behavior or document why these are intentionally no-ops

**Build.cs Module Typo:**
- Issue: Module dependency listed as "UMG" instead of "UMG" - likely meant to be "UMG" (UMG is correct for Unreal Motion Graphics)
- Files: `Source/ProjectSerene/ProjectSerene.Build.cs` (line 20)
- Impact: None if "UMG" is valid module name; verify it compiles correctly
- Fix approach: Verify module name is correct; UMG is the correct module for widgets

## Known Bugs

**Potential Null Pointer in HorrorUI::SetupCharacter:**
- Symptoms: Crash if SetupCharacter called with null pointer
- Files: `Source/ProjectSerene/Variant_Horror/UI/HorrorUI.cpp` (lines 7-11)
- Trigger: Call `SetupCharacter(nullptr)` or with invalid pointer
- Workaround: Ensure valid pointer is always passed
- Fix: Add null check before adding dynamic delegates

**Potential Null Widget Access in ShooterGameMode:**
- Symptoms: Crash if ShooterUIClass not set in Blueprint
- Files: `Source/ProjectSerene/Variant_Shooter/ShooterGameMode.cpp` (lines 13-16)
- Trigger: GameMode Blueprint does not configure ShooterUIClass
- Workaround: Always configure class in Blueprint
- Fix: Add null check after CreateWidget before calling AddToViewport

**Weapon Owner Null Check Missing in BeginPlay:**
- Symptoms: Crash if weapon spawned without valid owner
- Files: `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.cpp` (lines 43-54)
- Trigger: Spawn weapon without setting Owner in SpawnParams
- Workaround: Always set Owner when spawning weapons
- Fix: Add null checks for GetOwner() before calling methods on it

**PawnOwner Potential Null in Fire:**
- Symptoms: Crash when calling MakeNoise with null PawnOwner
- Files: `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.cpp` (line 141)
- Trigger: Weapon owned by non-Pawn actor
- Workaround: Only attach weapons to Pawn-derived actors
- Fix: Add null check for PawnOwner before MakeNoise call

## Security Considerations

**Android File Server Security Token Exposed:**
- Risk: Security token hardcoded in config file
- Files: `Config/DefaultEngine.ini` (line 106)
- Current mitigation: `bIncludeInShipping=False` prevents inclusion in shipped builds
- Recommendations: Ensure this setting is never changed; consider removing token from version control

**No Input Validation on Damage:**
- Risk: Negative damage could heal characters; extremely large values could cause overflow
- Files:
  - `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.cpp` (lines 62-83)
  - `Source/ProjectSerene/Variant_Shooter/AI/ShooterNPC.cpp` (lines 36-54)
- Current mitigation: None
- Recommendations: Clamp damage to positive values; consider using FMath::Max(0.0f, Damage)

## Performance Bottlenecks

**Sprint Timer Running Constantly:**
- Problem: Sprint timer ticks every 0.03333 seconds even when not sprinting
- Files: `Source/ProjectSerene/Variant_Horror/HorrorCharacter.cpp` (line 38)
- Cause: Timer started in BeginPlay, never conditionally paused
- Improvement path: Only run timer when actively sprinting or recovering; pause when at full stamina and not sprinting

**Tick Enabled on Actors That Don't Need It:**
- Problem: `AShooterWeapon`, `AShooterProjectile`, `AShooterPickup` have `PrimaryActorTick.bCanEverTick = true` but no Tick implementation
- Files:
  - `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.cpp` (line 17)
  - `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterProjectile.cpp` (line 18)
  - `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterPickup.cpp` (line 15)
- Cause: Default template code not cleaned up
- Improvement path: Set `PrimaryActorTick.bCanEverTick = false` if no Tick override exists

**Line Trace Per Shot for Aim:**
- Problem: Every weapon shot performs a line trace to calculate aim location
- Files:
  - `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.cpp` (lines 198-213)
  - `Source/ProjectSerene/Variant_Shooter/AI/ShooterNPC.cpp` (lines 83-124)
- Cause: Aim calculation done per-shot rather than cached
- Improvement path: Cache aim location when aiming starts; update on timer rather than per-shot for rapid-fire weapons

**Multiple Vertical Line Traces for LOS:**
- Problem: Line of sight check runs up to 5 line traces per check
- Files: `Source/ProjectSerene/Variant_Shooter/AI/ShooterStateTreeUtility.cpp` (lines 52-66)
- Cause: Checking multiple vertical offsets to find clear LOS
- Improvement path: Consider reducing trace count or caching results; use early-out more aggressively

## Fragile Areas

**HorrorUI Delegate Binding:**
- Files: `Source/ProjectSerene/Variant_Horror/UI/HorrorUI.cpp` (lines 7-11)
- Why fragile: No cleanup of delegate bindings; if character destroyed while UI exists, dangling pointer
- Safe modification: Always pair AddDynamic with RemoveDynamic in appropriate cleanup
- Test coverage: No automated tests

**StateTree Weak Context Lambdas:**
- Files: `Source/ProjectSerene/Variant_Shooter/AI/ShooterStateTreeUtility.cpp` (lines 223-336)
- Why fragile: Complex lambda captures with weak context; timing-sensitive delegate unbinding
- Safe modification: Ensure delegate unbinding happens before context becomes invalid
- Test coverage: No automated tests

**Weapon Owner Relationship:**
- Files:
  - `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.cpp`
  - `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.cpp`
- Why fragile: Tight coupling between weapon and owner through interface; owner destruction must properly clean up weapons
- Safe modification: Always use IsValid() checks; ensure OnOwnerDestroyed properly destroys weapon
- Test coverage: No automated tests

**Character Death Flow:**
- Files:
  - `Source/ProjectSerene/Variant_Shooter/ShooterCharacter.cpp` (lines 285-316)
  - `Source/ProjectSerene/Variant_Shooter/AI/ShooterNPC.cpp` (lines 151-188)
- Why fragile: Multiple systems affected by death (movement, input, weapons, UI, timers); order matters
- Safe modification: Test full death/respawn cycle after any changes
- Test coverage: No automated tests

## Scaling Limits

**Single Player Focus:**
- Current capacity: Architecture assumes single local player
- Limit: Multiplayer would require significant refactoring
- Scaling path:
  - PlayerController UI spawning would need network-awareness
  - ShooterGameMode uses PlayerController index 0 hardcoded
  - Score system not replicated

**NPC Spawner Single Instance:**
- Current capacity: One NPC active per spawner at a time
- Limit: `SpawnCount` limits total NPCs but only one alive at once
- Scaling path: Modify spawner to support multiple concurrent NPCs if needed

## Dependencies at Risk

**StateTree Plugin Dependency:**
- Risk: Relatively new Unreal plugin; API may change between engine versions
- Impact: Custom StateTree tasks/conditions in `ShooterStateTreeUtility.h/.cpp` may need updates
- Migration plan: Monitor Epic's StateTree API stability; maintain abstraction layer if frequent breaks occur

**UMG/Slate Dependency:**
- Risk: Heavy reliance on Blueprint-implementable events for UI
- Impact: All UI logic split between C++ and Blueprint; changes require both
- Migration plan: Consider CommonUI plugin for more robust UI architecture if complexity grows

## Missing Critical Features

**No Save/Load System:**
- Problem: No persistence mechanism for game state
- Blocks: Player progression, settings persistence

**No Audio Implementation:**
- Problem: No sound effects or music systems implemented in C++
- Blocks: Complete gameplay experience; AI perception has noise tags but no actual audio

**No Pause Menu:**
- Problem: No pause functionality or menu system
- Blocks: Standard game flow; settings access

**No Reload Mechanic:**
- Problem: Weapons auto-reload when magazine depletes; no manual reload
- Files: `Source/ProjectSerene/Variant_Shooter/Weapons/ShooterWeapon.cpp` (lines 182-189)
- Blocks: Tactical shooter gameplay; reload timing decisions

## Test Coverage Gaps

**No Automated Tests:**
- What's not tested: Entire codebase has zero automated tests
- Files: All files in `Source/ProjectSerene/`
- Risk: Regressions undetected; refactoring dangerous
- Priority: High - establish basic test framework before adding features

**No Blueprint Test Coverage:**
- What's not tested: Blueprint implementations of UI and gameplay logic
- Files: All Blueprint assets in `Content/`
- Risk: UI breakage; gameplay bugs from Blueprint changes
- Priority: Medium - after C++ test framework established

---

*Concerns audit: 2025-01-19*
