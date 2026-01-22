# Project State: Project Serene

**Last Updated:** 2026-01-22

---

## Project Reference

**Core Value:** The player experiences unreliable perception - they can never fully trust what they see, creating genuine paranoia and tension.

**Current Focus:** Phase 4 - Consumables & Inventory - COMPLETE. Inventory infrastructure and pickup system implemented. Ready for Phase 5 (Investigation System).

**Scope:** Demo/vertical slice - one complete investigation demonstrating core mechanics and tone.

**Reviewer Persona:** Senior Unreal Engine Developer (10+ years experience, shipped multiple AAA titles). Reviews focus on: performance optimization, GAS best practices, memory safety, UE coding standards, Blueprint/C++ boundary design, scalability, and maintainability.

---

## Current Position

**Phase:** 4 of 10 (Consumables & Inventory) - COMPLETE
**Plan:** 2 of 2 complete
**Status:** Phase 4 complete, ready for Phase 5

**Progress:**
```
Phase 1  [###] Core Attributes (GAS Foundation) - COMPLETE
Phase 2  [###] Sanity & Perception - COMPLETE
Phase 3  [##] Flashlight & Light System - COMPLETE
Phase 4  [##] Consumables & Inventory - COMPLETE
Phase 5  [ ] Investigation System
Phase 6  [ ] Hallucination System
Phase 7  [ ] AI & Enemies
Phase 8  [ ] Combat System
Phase 9  [ ] Narrative & PTSD
Phase 10 [ ] Environment & Demo Level

Overall: [##########] 10/~30 plans complete (~33%)
```

---

## Performance Metrics

| Metric | Value |
|--------|-------|
| Phases Complete | 4/10 |
| Plans Executed | 10 |
| Last Plan Duration | ~5 min |
| Blockers Encountered | 0 |

---

## Accumulated Context

### Key Decisions

| Decision | Rationale | Date |
|----------|-----------|------|
| Use GAS for attributes | Unified system for health, stamina, sanity, battery; enables Gameplay Effects | 2026-01-19 |
| State Trees for AI | Already in project; cleaner than Behavior Trees for horror AI states | 2026-01-19 |
| 10 phases structure | Derived from 62 requirements across 11 categories with natural dependencies | 2026-01-19 |
| Sanity as central mechanic | Everything connects through sanity - perception, hallucinations, combat | 2026-01-19 |
| SetIsReplicated(false) on ASC | Single-player performance optimization; replication scaffolded for future | 2026-01-20 |
| Threshold tags: Health/Sanity 30%, Stamina/Battery 20% | Matches CONTEXT.md low health trigger points | 2026-01-20 |
| ASC on PlayerState | Industry standard for attribute persistence across respawns | 2026-01-20 |
| Timer-based stamina replaced with GAS | Unified approach using Gameplay Effects for all attribute changes | 2026-01-20 |
| PlayerController as UI host | Clean separation - Character handles gameplay, Controller handles UI and GAS bindings | 2026-01-20 |
| BlueprintImplementableEvent for UI | C++ provides interface, Blueprint implements visuals and animations | 2026-01-20 |
| Multi-level sanity tags (50%, 30%, 20%) | Enables perception effects at 50%, hallucinations at 30%, audio muffling at 20% | 2026-01-20 |
| 5% sanity floor | Player can never reach 0 sanity; potential future difficulty option | 2026-01-20 |
| 80% sanity regen cap via GetSanityRegenCapValue() | Light regen capped; medication can restore past 80% | 2026-01-20 |
| Timer-based light detection (0.1s interval) | More performant than Tick; sufficient for gameplay feel | 2026-01-20 |
| Actor tags for light filtering | FlickeringLight excludes, ProtectiveLight includes non-light actors | 2026-01-20 |
| Linear interpolation for visual effects | FMath::Lerp between min/max; can tune to curves in playtesting | 2026-01-20 |
| Camera post-process over volumes | Effects follow player automatically; cleaner than world placement | 2026-01-20 |
| Component-level regen cap enforcement | CheckRegenCap() in component; simpler than custom Gameplay Effect calc | 2026-01-20 |
| Component-based light detection | Search for UPointLightComponent/USpotLightComponent, not ALight actors | 2026-01-21 |
| Spot light cone validation | Use OuterConeAngle to verify player is within actual illumination | 2026-01-21 |
| Line trace occlusion for lights | Walls/floors block light detection; configurable via bCheckLightOcclusion | 2026-01-21 |
| Edge detection for UI warnings | Only fire events when crossing threshold, not on every attribute change | 2026-01-21 |
| FlashlightComponent owns behavior, not light | SpotLight stays on Character (camera attachment), component controls via Initialize() | 2026-01-21 |
| Battery depletion auto-off via GAS delegate | OnBatteryChanged monitors and calls TransitionToState(Off) at 0% | 2026-01-21 |
| UTimelineComponent for flashlight curves | Standard UE pattern for warm-up and death sequences | 2026-01-21 |
| Perlin noise for flicker | FMath::PerlinNoise1D for organic variation without visible patterns | 2026-01-21 |
| 10% flicker threshold | Late warning creates tension, matches horror game conventions | 2026-01-21 |
| Sprint sway in TickComponent | Needs every-frame smoothness for rotation interpolation | 2026-01-21 |
| InventoryComponent on PlayerState | Follows ASC ownership pattern for persistence across respawns | 2026-01-22 |
| UPrimaryDataAsset for items | Enables Asset Manager integration and organized asset discovery | 2026-01-22 |
| TMap with UPROPERTY for inventory | Prevents garbage collection issues with UObject keys | 2026-01-22 |
| MatchesTag for category filtering | Hierarchical matching supports Item.Consumable matching Medical/Tool | 2026-01-22 |
| Timer-based interaction checking | 0.1s interval balances responsiveness with performance | 2026-01-22 |
| GetPlayerViewPoint for line trace | Uses camera location and rotation directly for first-person accuracy | 2026-01-22 |
| WeakObjectPtr for CurrentInteractable | Safe reference that auto-clears if actor is destroyed | 2026-01-22 |
| Custom depth stencil for highlight | Standard value 255 for post-process outline detection | 2026-01-22 |

### Technical Notes

- Existing Horror variant provides HorrorCharacter with stamina/sprint, HorrorGameMode, HorrorUI
- Lumen (Software RT) required for dynamic flashlight lighting
- MetaSounds available for procedural audio (sanity-driven effects)
- Enhanced Input already configured
- **GAS enabled:** GameplayAbilities, GameplayTags, GameplayTasks modules added
- **GAS classes created:** SereneAttributeSet, SereneAbilitySystemComponent, SereneGameplayTags in Source/ProjectSerene/GAS/
- **PlayerState integration:** SerenePlayerState owns ASC and AttributeSet
- **Character integration:** HorrorCharacter accesses GAS via PlayerState, stamina uses Gameplay Effects
- **UI integration:** HorrorPlayerController binds GAS delegates, HorrorUI receives percentage-based updates
- **Sanity perception:** SanityPerceptionComponent attached to HorrorCharacter, manages light detection and GAS effects
- **Sanity tags:** State.Sanity50, State.Sanity30, State.Sanity20 for multi-level perception effects
- **Visual distortion:** Camera post-process settings (vignette, grain, chromatic aberration, saturation) driven by sanity percentage
- **Audio distortion:** MetaSound parameters (HeartbeatIntensity, WhisperIntensity, MuffleAmount) ready for sound design
- **HUD warning:** Edge-detected sanity warning below 30%, Blueprint animation support
- **Light detection:** Cone + occlusion validation, excludes player flashlight
- **Flashlight system:** FlashlightComponent with full state machine (Off, WarmingUp, On, Flickering, DyingOut), GAS battery drain, State.FlashlightOn tag, timeline-driven curves, Perlin noise flicker, sprint sway
- **Inventory system:** InventoryComponent on PlayerState with TMap storage, ItemDataAsset for item definitions, Item.* gameplay tags for categorization
- **Interaction system:** IInteractableTarget interface, timer-based line trace detection, custom depth highlight, ConsumablePickup actor

### Patterns Established (Phase 1 + Phase 2 + Phase 3 + Phase 4)

- ASC on PlayerState, Character as Avatar (IAbilitySystemInterface forwarding)
- PossessedBy is initialization point for GAS on player characters
- Gameplay Effects for attribute modification (drain/regen)
- GetGameplayAttributeValueChangeDelegate for reactive UI updates
- BlueprintImplementableEvent pattern for UI customization
- State tags under State.* namespace for gameplay conditions
- ATTRIBUTE_ACCESSORS macro for standardized attribute access
- **Multi-threshold tag management:** UpdateSanityThresholdTags pattern in AttributeSet
- **ActorComponent for modular systems:** SanityPerceptionComponent, FlashlightComponent patterns
- **Light caching with TWeakObjectPtr:** Safe actor references that auto-clean
- **Grace period state machine:** bInLight + bInGracePeriod + timer callbacks
- **Camera post-process modification:** bOverride_* flags + value setting
- **Sanity-driven visual effects:** OnSanityChanged delegate triggers UpdateVisualDistortion
- **IsIlluminatedByLight pattern:** Distance + cone angle + line trace occlusion
- **TActorIterator with component filtering:** Flexible actor discovery by component type
- **Edge detection for UI events:** Track previous state, only fire on change
- **Component delegation pattern:** FlashlightComponent.Initialize(SpotLight) for external light control
- **Timeline-component pattern:** NewObject<UTimelineComponent> with curve binding for animations
- **Perlin noise flicker pattern:** FMath::PerlinNoise1D for organic procedural variation
- **Sprint sway rotation pattern:** Sinusoidal offset with smooth return interpolation
- **UPrimaryDataAsset pattern:** GetPrimaryAssetId() returns Type:AssetName format for Asset Manager
- **Item effect application:** ASC->MakeOutgoingSpec + ApplyGameplayEffectSpecToSelf pattern
- **OnInventoryChanged delegate:** UI binding for inventory updates
- **Quick slot array pattern:** Fixed size array with NUM_QUICK_SLOTS constant
- **IInteractableTarget interface:** Execute_* static methods for calling interface methods
- **Timer-based interaction checking:** FTimerHandle with looping timer at 0.1s interval
- **Focus state management:** OnFocused/OnUnfocused pair for highlight control

### Research Flags

| Area | Status | Notes |
|------|--------|-------|
| Hallucination tells | Needs prototyping | Few documented patterns; learnable tells require experimentation |
| PTSD representation | Needs consultation | Mental health consultation recommended before narrative design |
| Sanity-audio integration | Framework ready | MetaSound asset needs creation; C++ parameters implemented |

---

## Session Continuity

### Last Session
- 2026-01-22: Completed 04-02 Pickup System
- IInteractableTarget interface for world object interaction
- ConsumablePickup actor with inventory integration
- Timer-based line trace detection on HorrorPlayerController
- Interaction prompt UI forwarding to HorrorUI

### Next Session
- Execute Phase 5 (Investigation System)
- Or configure Blueprint assets for Phase 4

### Pending Items
- [x] Enable GAS plugin
- [x] Create AttributeSet with core attributes
- [x] Create native gameplay tags
- [x] Create SerenePlayerState with ASC ownership (01-02)
- [x] Integrate HorrorCharacter with GAS stamina (01-02)
- [x] Wire HUD to GAS attributes (01-03)
- [x] Runtime test stamina system (verified)
- [x] Create SanityPerceptionComponent (02-01)
- [x] Add multi-level sanity threshold tags (02-01)
- [x] Implement visual distortion system (02-02)
- [x] Document GE_SanityDrain and GE_SanityRegen specifications (02-02)
- [x] Create GE_SanityDrain Blueprint in Editor
- [x] Create GE_SanityRegen Blueprint in Editor
- [x] Configure BP_HorrorCharacter with sanity effects
- [x] Runtime test sanity drain/regen system
- [x] Create audio distortion framework (02-03)
- [x] Add HUD warning icon (02-03)
- [x] Runtime test complete sanity feedback loop
- [x] Create FlashlightComponent with GAS battery drain (03-01)
- [x] Add State.FlashlightOn gameplay tag (03-01)
- [x] Integrate flashlight into HorrorCharacter (03-01)
- [x] Extend flashlight with warm-up, flicker, death states (03-02)
- [x] Add timeline-based curve sequences (03-02)
- [x] Implement Perlin noise flicker (03-02)
- [x] Add sprint sway (03-02)
- [ ] **USER:** Create IA_ToggleFlashlight Input Action in Editor
- [ ] **USER:** Create GE_BatteryDrain Gameplay Effect in Editor
- [ ] **USER:** Create C_FlashlightWarmup Float Curve in Editor
- [ ] **USER:** Create C_FlashlightDeath Float Curve in Editor
- [ ] **USER:** Configure BP_HorrorCharacter with flashlight settings and curves
- [ ] **USER:** Runtime test flashlight toggle, battery drain, warm-up, flicker, death
- [ ] **OPTIONAL:** Create MS_SanityDistortion MetaSound asset for audio
- [x] Create ItemDataAsset class (04-01)
- [x] Create InventoryComponent with GAS integration (04-01)
- [x] Add Item.Consumable.*, Item.Evidence gameplay tags (04-01)
- [x] Integrate InventoryComponent into SerenePlayerState (04-01)
- [x] Create IInteractableTarget interface (04-02)
- [x] Create ConsumablePickup actor (04-02)
- [x] Add interaction detection to HorrorPlayerController (04-02)
- [ ] **USER:** Create IA_Interact Input Action in Editor
- [ ] **USER:** Add IA_Interact to IMC_Horror (bound to E key)
- [ ] **USER:** Configure BP_HorrorPlayerController InteractAction property
- [ ] **USER:** Implement interaction prompt in BP_HorrorUI
- [ ] **USER:** Create BP_ConsumablePickup Blueprints
- [ ] **USER:** Create item data assets (DA_Bandage, DA_Battery, etc.)
- [ ] **USER:** Create item Gameplay Effects (GE_UseBandage, GE_UseBattery)
- [ ] **USER:** Set up outline post-process material

---

## Blockers

None currently.

---

*State initialized: 2026-01-19*
*Last updated: 2026-01-22*
*Phase 3 complete: 2026-01-21*
*Phase 4 complete: 2026-01-22*
