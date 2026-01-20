# Research Summary: Project Serene

**Project:** Psychological Horror FPS (Detective PTSD, Cult Investigation)
**Synthesized:** 2026-01-19
**Overall Confidence:** HIGH

---

## Executive Summary

Project Serene is a psychological horror FPS combining sanity mechanics, hallucination systems, investigation gameplay, and limited combat. Research confirms this occupies an underexplored niche between pure survival horror (Amnesia, Outlast) and action horror (Dead Space, Resident Evil). The combination of a detective protagonist with PTSD investigating a cult, distinguishable hallucinations, and Condemned-style forensic investigation has no direct competitor.

The recommended approach builds on the existing UE5.7 Horror template foundation. The SanityComponent becomes the heart of the architecture, broadcasting state changes via delegates to post-processing (perception distortion), the HallucinationManager (entity spawning), audio systems (MetaSounds procedural intensity), and combat (aim sway at low sanity). This observer pattern enables loose coupling and matches established template patterns. State Trees handle AI behavior with the project's existing setup, while MetaSounds provides native procedural audio for sanity-driven effects.

Critical risks center on three areas: (1) sanity becoming a resource management game rather than psychological experience, (2) hallucinations creating frustration instead of fear if players cannot learn to distinguish them from real threats, and (3) PTSD representation causing harm if handled as spectacle rather than character depth. Mitigation requires sanity effects to be primarily perceptual rather than mechanically punishing, hallucinations to have subtle but learnable tells, and mental health consultation during narrative design. Combat must feel desperate and costly, never empowering. Demo scope must be ruthlessly constrained to a single investigation slice.

---

## Key Findings

### From STACK.md

| Technology | Rationale |
|------------|-----------|
| UE5.7 + Existing Horror Template | Foundation already provides HorrorCharacter with stamina/sprint, HorrorGameMode, HorrorUI |
| MetaSounds | Zero-cost native procedural audio for sanity-driven heartbeat, breathing, distortion |
| Wwise (optional) | Industry-standard adaptive music and spatial audio if budget allows |
| Lumen Software Ray Tracing | Dynamic flashlight bounce lighting critical for horror; production-ready |
| State Trees | Already in project; cleaner than Behavior Trees for horror AI states |
| Post Process Volumes + Material Parameter Collection | Sanity-driven vignette, grain, chromatic aberration blending |
| Enhanced Input | Already configured for context-based action switching |

**Critical version requirement:** UE5.7 with "Generate Mesh Distance Fields" enabled for Lumen.

**Anti-stack:** No baked lightmaps (defeats dynamic flashlight), no legacy input system, no Hardware Ray Tracing as primary (60fps not achievable), no heavy marketplace horror packs.

### From FEATURES.md

**Must-Have (Table Stakes):**
- First-person perspective with flashlight/battery management
- Basic sanity system (visual/audio distortion in darkness)
- At least one real enemy type with chase/hide behavior
- Environmental interaction (doors, drawers, items)
- Audio cue system for threats
- Environmental storytelling (notes, recordings)
- Save/checkpoint system

**Should-Have (Differentiators):**
- Hallucination system with distinguishable visual tells
- Evidence collection and investigation mechanics
- Improvised melee combat (desperate, breakable weapons)
- PTSD flashbacks integrated with sanity thresholds

**Defer to Post-MVP:**
- Branching narrative / multiple endings
- Advanced investigation toolkit (UV light, forensic camera)
- Reality-shifting environments
- Pistol combat (can prototype melee-only initially)

**Core loop:** See threat -> Hide/Flee -> Recover in light -> Explore -> Repeat

### From ARCHITECTURE.md

**Core Components:**
1. **SanityComponent** (Actor Component) - Owns sanity state, broadcasts via delegates
2. **PerceptionModifierComponent** - Applies sanity-driven post-process and audio effects
3. **HallucinationManager** (World Subsystem) - Spawns/tracks hallucinations based on sanity thresholds
4. **InvestigationManager** (GameInstance Subsystem) - Tracks evidence, connections, case progress
5. **CombatComponent** - Weapons with sanity-affected aim sway and reload speed
6. **InteractionComponent** - Detects and manages interactable objects
7. **HorrorAIManager** (World Subsystem) - Coordinates AI state and threat levels

**Key Patterns:**
- Observer pattern via dynamic multicast delegates
- Subsystems for manager lifetime (World vs GameInstance)
- Data-driven configuration via Data Assets
- Interface-based interactions (IInteractableInterface)

**Anti-patterns to avoid:**
- Direct references between systems (use delegates)
- Polling instead of events
- Monolithic GameMode
- Hard-coded thresholds

**GAS recommendation:** Do NOT use. Overkill for simple sanity/combat; custom components are simpler and sufficient.

### From PITFALLS.md

**Top 5 Critical Pitfalls:**

| Pitfall | Prevention | Phase |
|---------|------------|-------|
| Sanity becomes resource management game | Make effects perceptual not mechanical; recovery from narrative moments not consumables | Core Mechanics |
| Hallucinations cause frustration not fear | Learnable visual tells (flicker, no shadows, movement); early hallucinations obviously fake | Enemy Design |
| Light/dark becomes unplayable | Test on multiple displays; provide meaningful darkness visibility; adaptive ambient | Core Mechanics |
| Combat undermines horror | Desperate/costly combat; strategic scarcity; fleeing should be viable | Combat Design |
| PTSD misrepresentation | Consult lived experience; symptoms accurate; horror from situation not illness | Narrative Design |

**Red Lines (Project Killers):**
1. Sanity system players can ignore or exploit
2. Hallucinations indistinguishable with no learning curve
3. PTSD portrayed as "detective goes crazy is scary"
4. Demo scope expanding beyond single investigation slice
5. Combat that makes players feel powerful

---

## Implications for Roadmap

Based on combined research, the following phase structure is recommended:

### Suggested Phases

**Phase 1: Core Horror Foundation**
- SanityComponent with delegates
- PerceptionModifierComponent (post-process effects)
- Light/dark detection system
- Flashlight with battery
- First-person controller extensions

*Rationale:* Everything keys off sanity. Must work before other systems have meaning.
*Delivers:* Core loop of sanity degradation in darkness, recovery in light, visual distortion.
*Pitfalls to avoid:* #1 (sanity as resource), #3 (unplayable darkness).
*Research needed:* Standard patterns documented; minimal research.

**Phase 2: Investigation System**
- InteractionComponent
- InvestigationManager
- Evidence collection and board
- Basic interactables (doors, drawers, items)
- Environmental storytelling props

*Rationale:* Investigation provides natural tension valleys and differentiates from pure survival horror.
*Delivers:* Detective gameplay loop; breathing room between horror.
*Pitfalls to avoid:* #7 (pixel hunting).
*Research needed:* Standard patterns; consult Three Clue Rule resources.

**Phase 3: Hallucination System**
- HallucinationManager
- Hallucination actor base class
- Visual tells system (flicker, no shadows, movement)
- Sanity threshold-triggered spawning
- Multiple hallucination types (environmental, auditory, shadow, manifestation)

*Rationale:* Core differentiator; requires Phase 1 sanity system complete.
*Delivers:* Is-it-real uncertainty; psychological horror layer.
*Pitfalls to avoid:* #2 (frustration not fear).
*Research needed:* YES - limited documented patterns for learnable tells.

**Phase 4: AI and Threats**
- HorrorAIManager
- Real enemy AI with State Trees
- AIPerception configuration (sight, hearing)
- Chase and hide mechanics
- Patrol and investigation behaviors

*Rationale:* Real threats provide contrast to hallucinations; requires sanity integration.
*Delivers:* Genuine danger; chase sequences; hide mechanics.
*Pitfalls to avoid:* #9 (stalker predictability).
*Research needed:* Moderate - State Tree patterns documented but horror-specific tuning needed.

**Phase 5: Combat System**
- CombatComponent
- Improvised melee weapons (breakable)
- Pistol (if scope allows; can defer)
- Sanity-affected aim/reload
- Combat as costly/desperate

*Rationale:* Combat is last resort; can be developed in parallel with Phase 3-4.
*Delivers:* Limited self-defense; resource tension.
*Pitfalls to avoid:* #4 (combat undermines horror).
*Research needed:* Minimal - existing ShooterWeaponHolder patterns apply.

**Phase 6: Narrative and PTSD Integration**
- PTSD flashback system
- Trauma triggers and responses
- Dialogue/document system
- Character-specific sanity events
- Narrative branching (if scope allows)

*Rationale:* Requires all core systems functional; needs careful design.
*Delivers:* Detective character depth; personal horror; story integration.
*Pitfalls to avoid:* #5 (PTSD misrepresentation), #10 (false agency).
*Research needed:* YES - mental health consultation recommended.

**Phase 7: Level Design and Polish**
- First demo location
- Tension curve implementation
- Audio design pass
- Performance optimization
- Playtesting and iteration

*Rationale:* Requires Phases 1-5 functional for meaningful testing.
*Delivers:* Playable vertical slice; demo-ready content.
*Pitfalls to avoid:* #6 (tension fatigue), #8 (UE5 performance), #14 (scope creep).
*Research needed:* Minimal - execution phase.

### Research Flags

| Phase | Research Needed | Reason |
|-------|-----------------|--------|
| Phase 3: Hallucinations | YES - deep dive | Few documented patterns for learnable tell systems |
| Phase 6: Narrative/PTSD | YES - external consultation | Mental health representation requires lived experience input |
| Phase 4: AI | MODERATE | State Trees documented but horror-specific behavior tuning lacks resources |
| All others | MINIMAL | Well-documented patterns in UE5 and existing codebase |

### Build Dependencies

```
Phase 1 (Sanity/Perception)
    |
    +---> Phase 2 (Investigation) - can parallel late Phase 1
    |
    +---> Phase 3 (Hallucinations) - requires Phase 1 complete
    |         |
    +---> Phase 4 (AI) - requires Phase 1, can parallel Phase 3
    |         |
    +---> Phase 5 (Combat) - can parallel Phases 3-4
              |
              v
         Phase 6 (Narrative/PTSD) - requires Phases 1-5 functional
              |
              v
         Phase 7 (Level Design/Polish) - requires all
```

---

## Confidence Assessment

| Area | Confidence | Notes |
|------|------------|-------|
| Stack | HIGH | Verified with official UE5.7 docs; existing template patterns |
| Features | HIGH | Cross-referenced against multiple reference games and industry analysis |
| Architecture | MEDIUM-HIGH | Patterns verified; horror-specific integration is custom |
| Pitfalls | MEDIUM | Synthesized from postmortems and industry sources; specific to psychological horror |

### Gaps to Address

1. **Hallucination tell system design** - No definitive patterns exist; will require experimentation and playtesting
2. **PTSD representation** - Recommend consultation with mental health professionals and people with lived experience before narrative design phase
3. **Sanity-audio integration** - MetaSounds procedural parameters need prototyping; no existing examples found for sanity-driven synthesis
4. **Performance baselines** - Need to establish minimum spec and profile Lumen/VSM costs early
5. **Wwise decision** - Budget and team expertise will determine if Wwise integration is worth the complexity

---

## Sources

### Official Documentation (HIGH)
- UE5.7 Release Notes and What's New
- Behavior Tree Quick Start Guide
- AI Perception Documentation
- Enhanced Input Documentation
- Lumen GI Documentation
- Post Process Effects
- Programming Subsystems
- StateTree Overview
- Level Streaming

### Industry Postmortems (HIGH)
- Game Design Deep Dive: Amnesia's Sanity Meter (Thomas Grip)
- The Sinking City 2 Developer Interview (Frogwares)
- Epic CEO on UE5 Performance Optimization

### Game Design Analysis (MEDIUM)
- Horror Game Mechanics (Dr. Wedge)
- The Problem of Modern Horror Game Design (Game Wisdom)
- Three Clue Rule (The Alexandrian)
- Pacing in Horror Games (Cliqist)
- Mental Health Portrayals in Video Games (Psychiatric Times)
- Checkpoint Mental Health Representation

### Reference Games Analyzed
- Amnesia: The Dark Descent (sanity system)
- SOMA (context-specific effects)
- Condemned: Criminal Origins (investigation + melee)
- Alan Wake (light mechanics)
- Silent Hill 2 (symbolic horror)
- Outlast (chase/hide)
- Layers of Fear (perception)
- Hellblade: Senua's Sacrifice (mental health representation)

---

*Research synthesis complete. Ready for roadmap creation.*
