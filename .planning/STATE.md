# Project State: Project Serene

**Last Updated:** 2026-01-21

---

## Project Reference

**Core Value:** The player experiences unreliable perception - they can never fully trust what they see, creating genuine paranoia and tension.

**Current Focus:** Phase 3 - Flashlight & Light System - COMPLETE. Full flashlight system with toggle input, GAS battery drain, warm-up/flicker/death states, sprint sway. Ready for Phase 4.

**Scope:** Demo/vertical slice - one complete investigation demonstrating core mechanics and tone.

**Reviewer Persona:** Senior Unreal Engine Developer (10+ years experience, shipped multiple AAA titles). Reviews focus on: performance optimization, GAS best practices, memory safety, UE coding standards, Blueprint/C++ boundary design, scalability, and maintainability.

---

## Current Position

**Phase:** 3 of 10 (Flashlight & Light System) - COMPLETE
**Plan:** 2 of 2 complete
**Status:** Ready for Phase 4

**Progress:**
```
Phase 1  [###] Core Attributes (GAS Foundation) - COMPLETE
Phase 2  [###] Sanity & Perception - COMPLETE
Phase 3  [##] Flashlight & Light System - COMPLETE
Phase 4  [ ] Consumables & Inventory
Phase 5  [ ] Investigation System
Phase 6  [ ] Hallucination System
Phase 7  [ ] AI & Enemies
Phase 8  [ ] Combat System
Phase 9  [ ] Narrative & PTSD
Phase 10 [ ] Environment & Demo Level

Overall: [########..] 8/~30 plans complete (~27%)
```

---

## Performance Metrics

| Metric | Value |
|--------|-------|
| Phases Complete | 3/10 |
| Plans Executed | 8 |
| Last Plan Duration | ~4 min |
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

### Patterns Established (Phase 1 + Phase 2 + Phase 3)

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

### Research Flags

| Area | Status | Notes |
|------|--------|-------|
| Hallucination tells | Needs prototyping | Few documented patterns; learnable tells require experimentation |
| PTSD representation | Needs consultation | Mental health consultation recommended before narrative design |
| Sanity-audio integration | Framework ready | MetaSound asset needs creation; C++ parameters implemented |

---

## Session Continuity

### Last Session
- 2026-01-21: Completed Phase 3 - Flashlight & Light System
- 03-01: Core FlashlightComponent with GAS battery drain, State.FlashlightOn tag
- 03-02: Visual polish - warm-up fade-in, Perlin noise flicker, death sequence, sprint sway
- Phase verified: 9/9 must-haves confirmed in codebase
- **Blueprint configuration required:** See SUMMARY files for Input Action, Gameplay Effect, Float Curves

### Next Session
- Plan Phase 4 (Consumables & Inventory)
- Or execute Phase 4 if already planned

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

---

## Blockers

None currently.

---

*State initialized: 2026-01-19*
*Last updated: 2026-01-21*
*Phase 3 complete: 2026-01-21*
