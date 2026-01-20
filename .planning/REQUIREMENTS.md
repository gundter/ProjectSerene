# Requirements: Project Serene

**Defined:** 2026-01-19
**Core Value:** The player experiences unreliable perception - they can never fully trust what they see, creating genuine paranoia and tension.

## v1 Requirements (Demo)

Requirements for demo release. Each maps to roadmap phases.

### Player Attributes (GAS)

- [x] **ATTR-01**: Gameplay Ability System (GAS) configured for attribute management
- [x] **ATTR-02**: Health attribute (0-100) tracked via GAS AttributeSet
- [x] **ATTR-03**: Stamina attribute (0-100) tracked via GAS AttributeSet
- [x] **ATTR-04**: Sanity attribute (0-100) tracked via GAS AttributeSet
- [x] **ATTR-05**: Flashlight battery attribute tracked via GAS AttributeSet
- [x] **ATTR-06**: Attribute changes broadcast via GAS delegates for UI/effects binding

### Sanity System

- [ ] **SANI-01**: Darkness exposure decreases sanity over time (via Gameplay Effect)
- [ ] **SANI-02**: Light exposure slowly restores sanity (capped at threshold, not full)
- [ ] **SANI-03**: Sanity thresholds trigger different gameplay effects
- [ ] **SANI-04**: Post-process effects intensify at lower sanity (vignette, grain, chromatic aberration)
- [ ] **SANI-05**: Audio distortion increases at lower sanity (heartbeat, breathing, ambient warping)

### Flashlight & Light

- [ ] **LITE-01**: Player has flashlight as primary light source
- [ ] **LITE-02**: Flashlight drains battery attribute over time when active
- [ ] **LITE-03**: Player can toggle flashlight on/off
- [ ] **LITE-04**: Being in flashlight cone counts as "in light" for sanity
- [ ] **LITE-05**: Flashlight dims/flickers as battery depletes

### Consumables & Inventory

- [ ] **CONS-01**: Inventory system for collecting and using items
- [ ] **CONS-02**: Batteries restore flashlight battery attribute
- [ ] **CONS-03**: Med kits restore health attribute (partial or full based on type)
- [ ] **CONS-04**: Energy drinks restore stamina attribute
- [ ] **CONS-05**: Mild medication provides partial sanity restoration
- [ ] **CONS-06**: Strong medication provides full sanity restoration
- [ ] **CONS-07**: Items can be picked up from environment
- [ ] **CONS-08**: Items can be used from inventory UI

### Hallucinations

- [ ] **HALL-01**: Hallucinations spawn when sanity drops below thresholds
- [ ] **HALL-02**: Visual tells distinguish hallucinations from real enemies (flicker, no shadows, movement patterns)
- [ ] **HALL-03**: Tell clarity scales with sanity level (higher sanity = clearer tells)
- [ ] **HALL-04**: Shadow figure hallucination type (peripheral, fades when looked at)
- [ ] **HALL-05**: Auditory hallucination type (sounds without source)
- [ ] **HALL-06**: Manifestation hallucination type (appears real, attacks, then vanishes)
- [ ] **HALL-07**: Hallucinations despawn when sanity rises above spawn threshold

### Investigation

- [ ] **INVS-01**: Player can interact with doors (open, close, locked detection)
- [ ] **INVS-02**: Player can interact with containers (drawers, cabinets, boxes)
- [ ] **INVS-03**: Player can pick up and examine items
- [ ] **INVS-04**: Notes and documents can be read and collected
- [ ] **INVS-05**: Audio recordings can be played and collected
- [ ] **INVS-06**: Evidence items can be collected at crime scenes
- [ ] **INVS-07**: Evidence board UI tracks collected evidence
- [ ] **INVS-08**: Evidence can be connected to form deductions

### Combat - Melee

- [ ] **MELE-01**: Player can pick up improvised melee weapons (pipe, board, etc.)
- [ ] **MELE-02**: Melee weapons have durability and break after use
- [ ] **MELE-03**: Melee attack deals damage to enemies
- [ ] **MELE-04**: Melee weapons can be dropped or swapped

### Combat - Pistol

- [ ] **PSTL-01**: Player can acquire and equip pistol
- [ ] **PSTL-02**: Pistol uses limited ammunition
- [ ] **PSTL-03**: Ammo can be found in environment (scarce)
- [ ] **PSTL-04**: Player can aim and fire pistol
- [ ] **PSTL-05**: Aim sway increases at lower sanity levels
- [ ] **PSTL-06**: Reload speed decreases at lower sanity levels
- [ ] **PSTL-07**: Pistol can damage/kill real enemies

### AI & Enemies

- [ ] **ENEM-01**: Cultist enemy type with humanoid appearance
- [ ] **ENEM-02**: Enemy AI uses State Trees for behavior
- [ ] **ENEM-03**: Enemies patrol designated areas
- [ ] **ENEM-04**: Enemies chase player when detected
- [ ] **ENEM-05**: Enemies attack player in melee range
- [ ] **ENEM-06**: AI perception responds to player sight (line of sight, distance)
- [ ] **ENEM-07**: AI perception responds to player sound (footsteps, gunshots)
- [ ] **ENEM-08**: Enemies can be killed (melee or pistol)

### Narrative & PTSD

- [ ] **NARR-01**: Opening establishes detective backstory (capture, torture, escape)
- [ ] **NARR-02**: New case introduction connects to past killer
- [ ] **NARR-03**: PTSD flashbacks trigger at story moments
- [ ] **NARR-04**: Flashbacks use visual/audio distortion effects
- [ ] **NARR-05**: Scripted horror events at key story beats
- [ ] **NARR-06**: Dialogue system for NPC conversations
- [ ] **NARR-07**: Internal monologue for detective's thoughts
- [ ] **NARR-08**: Story events can force sanity changes

### Environment & Demo Level

- [ ] **ENVR-01**: Demo crime scene location fully playable
- [ ] **ENVR-02**: Detective apartment as starting/hub location
- [ ] **ENVR-03**: Appropriate lighting for horror atmosphere (dark with pools of light)
- [ ] **ENVR-04**: Environmental props support investigation and storytelling
- [ ] **ENVR-05**: Save/checkpoint system functional

## v2 Requirements

Deferred to post-demo. Tracked but not in current roadmap.

### Advanced Investigation

- **INVS-09**: UV light tool for hidden evidence
- **INVS-10**: Forensic camera for documenting scenes
- **INVS-11**: Multiple crime scene locations

### Advanced Combat

- **CMBT-01**: Trigger discipline reward system
- **CMBT-02**: Consequence system for shooting civilians/hallucinations

### Advanced AI

- **ENEM-09**: Hide mechanics (player can hide in closets, under beds)
- **ENEM-10**: Enemy search behavior when player hides
- **ENEM-11**: Multiple enemy types with different behaviors

### Advanced Narrative

- **NARR-09**: Branching narrative based on evidence collected
- **NARR-10**: Multiple endings based on player choices
- **NARR-11**: Side investigations

### Polish

- **POLI-01**: Full precinct location
- **POLI-02**: Adaptive music system (Wwise integration)
- **POLI-03**: Advanced accessibility options

## Out of Scope

Explicitly excluded. Documented to prevent scope creep.

| Feature | Reason |
|---------|--------|
| Open world city exploration | Demo focuses on discrete locations; complexity too high |
| Multiplayer/co-op | Single-player psychological experience |
| VR support | Requires fundamentally different design; defer post-launch |
| Supernatural elements | Grounded psychological horror; everything is trauma-based |
| Full branching narrative | Demo has linear narrative; branching deferred to v2 |
| Multiple protagonists | Single detective focus for demo |

## Traceability

Which phases cover which requirements. Updated during roadmap creation.

| Requirement | Phase | Status |
|-------------|-------|--------|
| ATTR-01 | Phase 1 | Complete |
| ATTR-02 | Phase 1 | Complete |
| ATTR-03 | Phase 1 | Complete |
| ATTR-04 | Phase 1 | Complete |
| ATTR-05 | Phase 1 | Complete |
| ATTR-06 | Phase 1 | Complete |
| SANI-01 | Phase 2 | Pending |
| SANI-02 | Phase 2 | Pending |
| SANI-03 | Phase 2 | Pending |
| SANI-04 | Phase 2 | Pending |
| SANI-05 | Phase 2 | Pending |
| LITE-01 | Phase 3 | Pending |
| LITE-02 | Phase 3 | Pending |
| LITE-03 | Phase 3 | Pending |
| LITE-04 | Phase 3 | Pending |
| LITE-05 | Phase 3 | Pending |
| CONS-01 | Phase 4 | Pending |
| CONS-02 | Phase 4 | Pending |
| CONS-03 | Phase 4 | Pending |
| CONS-04 | Phase 4 | Pending |
| CONS-05 | Phase 4 | Pending |
| CONS-06 | Phase 4 | Pending |
| CONS-07 | Phase 4 | Pending |
| CONS-08 | Phase 4 | Pending |
| INVS-01 | Phase 5 | Pending |
| INVS-02 | Phase 5 | Pending |
| INVS-03 | Phase 5 | Pending |
| INVS-04 | Phase 5 | Pending |
| INVS-05 | Phase 5 | Pending |
| INVS-06 | Phase 5 | Pending |
| INVS-07 | Phase 5 | Pending |
| INVS-08 | Phase 5 | Pending |
| HALL-01 | Phase 6 | Pending |
| HALL-02 | Phase 6 | Pending |
| HALL-03 | Phase 6 | Pending |
| HALL-04 | Phase 6 | Pending |
| HALL-05 | Phase 6 | Pending |
| HALL-06 | Phase 6 | Pending |
| HALL-07 | Phase 6 | Pending |
| ENEM-01 | Phase 7 | Pending |
| ENEM-02 | Phase 7 | Pending |
| ENEM-03 | Phase 7 | Pending |
| ENEM-04 | Phase 7 | Pending |
| ENEM-05 | Phase 7 | Pending |
| ENEM-06 | Phase 7 | Pending |
| ENEM-07 | Phase 7 | Pending |
| ENEM-08 | Phase 7 | Pending |
| MELE-01 | Phase 8 | Pending |
| MELE-02 | Phase 8 | Pending |
| MELE-03 | Phase 8 | Pending |
| MELE-04 | Phase 8 | Pending |
| PSTL-01 | Phase 8 | Pending |
| PSTL-02 | Phase 8 | Pending |
| PSTL-03 | Phase 8 | Pending |
| PSTL-04 | Phase 8 | Pending |
| PSTL-05 | Phase 8 | Pending |
| PSTL-06 | Phase 8 | Pending |
| PSTL-07 | Phase 8 | Pending |
| NARR-01 | Phase 9 | Pending |
| NARR-02 | Phase 9 | Pending |
| NARR-03 | Phase 9 | Pending |
| NARR-04 | Phase 9 | Pending |
| NARR-05 | Phase 9 | Pending |
| NARR-06 | Phase 9 | Pending |
| NARR-07 | Phase 9 | Pending |
| NARR-08 | Phase 9 | Pending |
| ENVR-01 | Phase 10 | Pending |
| ENVR-02 | Phase 10 | Pending |
| ENVR-03 | Phase 10 | Pending |
| ENVR-04 | Phase 10 | Pending |
| ENVR-05 | Phase 10 | Pending |

**Coverage:**
- v1 requirements: 62 total
- Mapped to phases: 62
- Unmapped: 0

---
*Requirements defined: 2026-01-19*
*Last updated: 2026-01-19 - Traceability updated with phase mappings*
