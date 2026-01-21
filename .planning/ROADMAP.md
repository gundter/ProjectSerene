# Roadmap: Project Serene

**Created:** 2026-01-19
**Depth:** Comprehensive (8-12 phases)
**Core Value:** The player experiences unreliable perception - they can never fully trust what they see, creating genuine paranoia and tension.

---

## Overview

Project Serene is a psychological horror FPS demo featuring a detective with PTSD investigating a cult. The roadmap delivers the demo through 10 phases, progressing from foundational GAS attributes through sanity/perception systems, investigation mechanics, hallucinations, AI threats, combat, narrative integration, and final level polish. Each phase builds on prior work, with sanity being the central mechanic that connects perception, hallucinations, and combat.

---

## Phase 1: Core Attributes (GAS Foundation)

**Goal:** Player attributes exist and respond to gameplay, enabling all dependent systems.

**Dependencies:** None (foundation)

**Plans:** 3 plans

Plans:
- [x] 01-01-PLAN.md — Enable GAS plugin, create AttributeSet and core GAS classes
- [x] 01-02-PLAN.md — Create PlayerState with GAS ownership, integrate Character with GAS stamina
- [x] 01-03-PLAN.md — Wire HUD to GAS delegates, verify end-to-end

**Requirements:**
- ATTR-01: Gameplay Ability System (GAS) configured for attribute management
- ATTR-02: Health attribute (0-100) tracked via GAS AttributeSet
- ATTR-03: Stamina attribute (0-100) tracked via GAS AttributeSet
- ATTR-04: Sanity attribute (0-100) tracked via GAS AttributeSet
- ATTR-05: Flashlight battery attribute tracked via GAS AttributeSet
- ATTR-06: Attribute changes broadcast via GAS delegates for UI/effects binding

**Success Criteria:**
1. Player can see Health, Stamina, Sanity, and Battery values on HUD
2. Attribute changes from debug commands immediately reflect in UI
3. Gameplay Effects can modify any attribute (verified via test effect)
4. Attribute delegates fire correctly when values change (verified via logging)

---

## Phase 2: Sanity & Perception

**Goal:** Darkness degrades sanity, triggering visual and audio distortion that makes the world feel unreliable.

**Dependencies:** Phase 1 (Sanity attribute must exist)

**Plans:** 3 plans

Plans:
- [x] 02-01-PLAN.md — Core sanity mechanics: threshold tags, AttributeSet updates, SanityPerceptionComponent with light detection
- [x] 02-02-PLAN.md — Gameplay Effects for drain/regen, visual distortion via post-process
- [x] 02-03-PLAN.md — Audio distortion, HUD warning icon, end-to-end verification

**Requirements:**
- SANI-01: Darkness exposure decreases sanity over time (via Gameplay Effect)
- SANI-02: Light exposure slowly restores sanity (capped at threshold, not full)
- SANI-03: Sanity thresholds trigger different gameplay effects
- SANI-04: Post-process effects intensify at lower sanity (vignette, grain, chromatic aberration)
- SANI-05: Audio distortion increases at lower sanity (heartbeat, breathing, ambient warping)

**Success Criteria:**
1. Player standing in darkness sees sanity decrease on HUD over time
2. Player standing in light sees sanity slowly recover (but not past 80%)
3. Screen visuals become distorted (vignette, grain, color shift) as sanity drops below 50
4. Audio becomes unsettling (heartbeat, breathing) as sanity drops below 30
5. Crossing sanity thresholds (70, 50, 30) triggers noticeable perception changes

---

## Phase 3: Flashlight & Light System

**Goal:** Player controls their light source (visibility tool only, not sanity protection), creating a risk/reward loop between visibility and battery management.

**Dependencies:** Phase 1 (Battery attribute), Phase 2 (Light detection for sanity)

**Plans:** 2 plans

Plans:
- [x] 03-01-PLAN.md — Core FlashlightComponent with toggle input and GAS battery drain
- [x] 03-02-PLAN.md — Visual polish: warm-up fade-in, low-battery flicker, death sequence, sprint sway

**Requirements:**
- LITE-01: Player has flashlight as primary light source
- LITE-02: Flashlight drains battery attribute over time when active
- LITE-03: Player can toggle flashlight on/off
- LITE-04: Being in flashlight cone counts as "in light" for sanity
- LITE-05: Flashlight dims/flickers as battery depletes

**Note on LITE-04:** Per 03-CONTEXT.md decision, flashlight does NOT protect sanity - it's a visibility tool only. This requirement is superseded by the context decision. Environmental lights remain the only sanity protection.

**Success Criteria:**
1. Player can press F to toggle flashlight on/off
2. Flashlight beam illuminates dark areas realistically (Lumen)
3. Battery percentage decreases visibly on HUD while flashlight is active
4. Flashlight flickers and dims noticeably when battery drops below 10%
5. Flashlight has warm-up fade-in (~0.5s) and dramatic death sequence (~1.5s)

---

## Phase 4: Consumables & Inventory

**Goal:** Player can collect and use items to manage their attributes, creating resource decisions.

**Dependencies:** Phase 1 (All attributes), Phase 3 (Battery for batteries)

**Requirements:**
- CONS-01: Inventory system for collecting and using items
- CONS-02: Batteries restore flashlight battery attribute
- CONS-03: Med kits restore health attribute (partial or full based on type)
- CONS-04: Energy drinks restore stamina attribute
- CONS-05: Mild medication provides partial sanity restoration
- CONS-06: Strong medication provides full sanity restoration
- CONS-07: Items can be picked up from environment
- CONS-08: Items can be used from inventory UI

**Success Criteria:**
1. Player can press E on a battery pickup and see it added to inventory
2. Player can open inventory (Tab) and see collected items with quantities
3. Using a battery from inventory restores flashlight battery (visible on HUD)
4. Using a med kit restores health appropriately
5. Using medication restores sanity (mild: +30, strong: full restore)

---

## Phase 5: Investigation System

**Goal:** Player can explore environments, interact with objects, and collect evidence like a detective.

**Dependencies:** Phase 4 (Inventory for collecting items)

**Requirements:**
- INVS-01: Player can interact with doors (open, close, locked detection)
- INVS-02: Player can interact with containers (drawers, cabinets, boxes)
- INVS-03: Player can pick up and examine items
- INVS-04: Notes and documents can be read and collected
- INVS-05: Audio recordings can be played and collected
- INVS-06: Evidence items can be collected at crime scenes
- INVS-07: Evidence board UI tracks collected evidence
- INVS-08: Evidence can be connected to form deductions

**Success Criteria:**
1. Player can open/close doors with E key; locked doors show "Locked" prompt
2. Player can open drawers and cabinets to reveal contents inside
3. Player can pick up and rotate items in examine mode (R key)
4. Notes display readable text in a document UI overlay
5. Evidence board UI shows all collected evidence with connection lines between related items

---

## Phase 6: Hallucination System

**Goal:** Low sanity spawns hallucinations that look almost real, creating constant uncertainty about threats.

**Dependencies:** Phase 2 (Sanity thresholds), Phase 3 (Light affects sanity)

**Requirements:**
- HALL-01: Hallucinations spawn when sanity drops below thresholds
- HALL-02: Visual tells distinguish hallucinations from real enemies (flicker, no shadows, movement patterns)
- HALL-03: Tell clarity scales with sanity level (higher sanity = clearer tells)
- HALL-04: Shadow figure hallucination type (peripheral, fades when looked at)
- HALL-05: Auditory hallucination type (sounds without source)
- HALL-06: Manifestation hallucination type (appears real, attacks, then vanishes)
- HALL-07: Hallucinations despawn when sanity rises above spawn threshold

**Success Criteria:**
1. Dropping below 50 sanity causes shadow figures to appear in peripheral vision
2. Dropping below 30 sanity causes manifestation hallucinations that approach player
3. At 60+ sanity, hallucinations visibly flicker and cast no shadows (clear tells)
4. At 30 sanity, hallucinations have subtle tells (faint flicker only)
5. Restoring sanity above threshold causes active hallucinations to fade away

---

## Phase 7: AI & Enemies

**Goal:** Real cultist enemies patrol, detect, chase, and attack the player, creating genuine threats alongside hallucinations.

**Dependencies:** Phase 6 (Hallucinations for contrast), Phase 2 (AI perception of player)

**Requirements:**
- ENEM-01: Cultist enemy type with humanoid appearance
- ENEM-02: Enemy AI uses State Trees for behavior
- ENEM-03: Enemies patrol designated areas
- ENEM-04: Enemies chase player when detected
- ENEM-05: Enemies attack player in melee range
- ENEM-06: AI perception responds to player sight (line of sight, distance)
- ENEM-07: AI perception responds to player sound (footsteps, gunshots)
- ENEM-08: Enemies can be killed (melee or pistol)

**Success Criteria:**
1. Cultist enemies walk patrol routes in demo level
2. Cultist notices player when player enters line of sight (within 15m)
3. Cultist investigates location of loud sounds (gunshots)
4. Cultist chases player upon detection, attacks when in melee range
5. Player can distinguish real enemies from hallucinations (real enemies cast shadows, don't flicker)

---

## Phase 8: Combat System

**Goal:** Player can defend themselves with melee weapons and a pistol, with sanity affecting combat effectiveness.

**Dependencies:** Phase 7 (Enemies to fight), Phase 2 (Sanity affects aim/reload)

**Requirements:**
- MELE-01: Player can pick up improvised melee weapons (pipe, board, etc.)
- MELE-02: Melee weapons have durability and break after use
- MELE-03: Melee attack deals damage to enemies
- MELE-04: Melee weapons can be dropped or swapped
- PSTL-01: Player can acquire and equip pistol
- PSTL-02: Pistol uses limited ammunition
- PSTL-03: Ammo can be found in environment (scarce)
- PSTL-04: Player can aim and fire pistol
- PSTL-05: Aim sway increases at lower sanity levels
- PSTL-06: Reload speed decreases at lower sanity levels
- PSTL-07: Pistol can damage/kill real enemies

**Success Criteria:**
1. Player can pick up a pipe and swing it at enemies with left mouse button
2. Pipe breaks after 3-5 hits, forcing player to find new weapon
3. Player can equip pistol and fire with limited ammo (displayed on HUD)
4. At 30 sanity, pistol aim sway is significantly worse than at 80 sanity
5. Combat feels desperate, not empowering (ammo scarce, weapons break, sanity hurts accuracy)

---

## Phase 9: Narrative & PTSD

**Goal:** Story moments trigger PTSD flashbacks and scripted horror, connecting the detective's trauma to gameplay.

**Dependencies:** Phase 6 (Hallucinations), Phase 5 (Documents/dialogue), Phase 2 (Sanity changes)

**Requirements:**
- NARR-01: Opening establishes detective backstory (capture, torture, escape)
- NARR-02: New case introduction connects to past killer
- NARR-03: PTSD flashbacks trigger at story moments
- NARR-04: Flashbacks use visual/audio distortion effects
- NARR-05: Scripted horror events at key story beats
- NARR-06: Dialogue system for NPC conversations
- NARR-07: Internal monologue for detective's thoughts
- NARR-08: Story events can force sanity changes

**Success Criteria:**
1. Demo opening establishes detective's backstory through brief flashback sequence
2. Finding key evidence triggers PTSD flashback with distorted visuals/audio
3. Detective provides internal monologue when examining significant objects
4. At least one scripted horror moment occurs during crime scene investigation
5. Story events can force sanity to specific values (e.g., flashback drops to 30)

---

## Phase 10: Environment & Demo Level

**Goal:** Complete, polished demo level with both locations playable and atmospheric.

**Dependencies:** All prior phases (integration phase)

**Requirements:**
- ENVR-01: Demo crime scene location fully playable
- ENVR-02: Detective apartment as starting/hub location
- ENVR-03: Appropriate lighting for horror atmosphere (dark with pools of light)
- ENVR-04: Environmental props support investigation and storytelling
- ENVR-05: Save/checkpoint system functional

**Success Criteria:**
1. Player can start in apartment, receive case details, and travel to crime scene
2. Crime scene has appropriate layout with evidence to discover
3. Lighting creates horror atmosphere (darkness with strategic light pools)
4. Player can save progress and resume from last checkpoint
5. Complete demo flow playable from start to end (15-30 minutes)

---

## Progress

| Phase | Name | Requirements | Status |
|-------|------|--------------|--------|
| 1 | Core Attributes (GAS Foundation) | 6 | Complete |
| 2 | Sanity & Perception | 5 | Complete |
| 3 | Flashlight & Light System | 5 | Complete |
| 4 | Consumables & Inventory | 8 | Pending |
| 5 | Investigation System | 8 | Pending |
| 6 | Hallucination System | 7 | Pending |
| 7 | AI & Enemies | 8 | Pending |
| 8 | Combat System | 11 | Pending |
| 9 | Narrative & PTSD | 8 | Pending |
| 10 | Environment & Demo Level | 5 | Pending |

**Total:** 62 requirements mapped across 10 phases

---

## Dependency Graph

```
Phase 1 (GAS Foundation)
    |
    +---> Phase 2 (Sanity & Perception)
    |         |
    |         +---> Phase 6 (Hallucinations)
    |         |         |
    |         +---> Phase 7 (AI & Enemies) <--- requires contrast with hallucinations
    |                   |
    +---> Phase 3 (Flashlight) ---> Phase 4 (Consumables)
    |                                   |
    |                                   +---> Phase 5 (Investigation)
    |
    +---> Phase 8 (Combat) <--- requires Phase 2 (sanity), Phase 7 (enemies)
              |
              v
         Phase 9 (Narrative & PTSD) <--- requires Phases 2, 5, 6
              |
              v
         Phase 10 (Environment & Demo Level) <--- requires all
```

---

*Roadmap created: 2026-01-19*
*Coverage: 62/62 v1 requirements mapped*
*Phase 1 planned: 2026-01-20*
*Phase 2 planned: 2026-01-20*
*Phase 3 planned: 2026-01-21*
*Phase 3 complete: 2026-01-21*
