# Project State: Project Serene

**Last Updated:** 2026-01-20

---

## Project Reference

**Core Value:** The player experiences unreliable perception - they can never fully trust what they see, creating genuine paranoia and tension.

**Current Focus:** Phase 2 - Sanity & Perception - Plan 01 complete. SanityPerceptionComponent ready for GAS effects.

**Scope:** Demo/vertical slice - one complete investigation demonstrating core mechanics and tone.

---

## Current Position

**Phase:** 2 of 10 (Sanity & Perception)
**Plan:** 1 of 3 complete
**Status:** In progress

**Progress:**
```
Phase 1  [###] Core Attributes (GAS Foundation) - COMPLETE
Phase 2  [#..] Sanity & Perception - Plan 01 complete
Phase 3  [ ] Flashlight & Light System
Phase 4  [ ] Consumables & Inventory
Phase 5  [ ] Investigation System
Phase 6  [ ] Hallucination System
Phase 7  [ ] AI & Enemies
Phase 8  [ ] Combat System
Phase 9  [ ] Narrative & PTSD
Phase 10 [ ] Environment & Demo Level

Overall: [####......] 4/~30 plans complete (~13%)
```

---

## Performance Metrics

| Metric | Value |
|--------|-------|
| Phases Complete | 1/10 |
| Plans Executed | 4 |
| Last Plan Duration | 12 min |
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

### Patterns Established (Phase 1 + Phase 2)

- ASC on PlayerState, Character as Avatar (IAbilitySystemInterface forwarding)
- PossessedBy is initialization point for GAS on player characters
- Gameplay Effects for attribute modification (drain/regen)
- GetGameplayAttributeValueChangeDelegate for reactive UI updates
- BlueprintImplementableEvent pattern for UI customization
- State tags under State.* namespace for gameplay conditions
- ATTRIBUTE_ACCESSORS macro for standardized attribute access
- **Multi-threshold tag management:** UpdateSanityThresholdTags pattern in AttributeSet
- **ActorComponent for modular systems:** SanityPerceptionComponent pattern
- **Light caching with TWeakObjectPtr:** Safe actor references that auto-clean
- **Grace period state machine:** bInLight + bInGracePeriod + timer callbacks

### Research Flags

| Area | Status | Notes |
|------|--------|-------|
| Hallucination tells | Needs prototyping | Few documented patterns; learnable tells require experimentation |
| PTSD representation | Needs consultation | Mental health consultation recommended before narrative design |
| Sanity-audio integration | Needs prototyping | MetaSounds procedural parameters need testing |

---

## Session Continuity

### Last Session
- 2026-01-20: Completed 02-01-PLAN.md (Sanity Core Foundation)
- Added State_Sanity50, State_Sanity30, State_Sanity20 threshold tags
- Created SanityPerceptionComponent with light detection and GAS effect management
- Attached component to HorrorCharacter
- **Plan 02-01 complete**

### Next Session
- Execute Plan 02-02 (Blueprint Gameplay Effects for sanity drain/regen)
- Create GE_SanityDrain and GE_SanityRegen in Editor
- Configure component with effect classes
- Runtime test light-based sanity system

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
- [ ] Create GE_SanityDrain Blueprint (02-02)
- [ ] Create GE_SanityRegen Blueprint with 80% cap (02-02)
- [ ] Configure SanityPerceptionComponent with effects (02-02)
- [ ] Runtime test sanity drain/regen system
- [ ] Create post-process effects for sanity (02-03)
- [ ] Create audio distortion for sanity (02-03)

---

## Blockers

None currently.

---

*State initialized: 2026-01-19*
*Last updated: 2026-01-20*
