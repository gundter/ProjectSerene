# Requirements: Project Serene

**Defined:** 2026-01-19
**Core Value:** The player experiences unreliable perception — they can never fully trust what they see, creating genuine paranoia and tension.

## v1 Requirements (Demo)

Requirements for demo release. Each maps to roadmap phases.

### Player Attributes (GAS)

- [ ] **ATTR-01**: Gameplay Ability System (GAS) configured for attribute management
- [ ] **ATTR-02**: Health attribute (0-100) tracked via GAS AttributeSet
- [ ] **ATTR-03**: Stamina attribute (0-100) tracked via GAS AttributeSet
- [ ] **ATTR-04**: Sanity attribute (0-100) tracked via GAS AttributeSet
- [ ] **ATTR-05**: Flashlight battery attribute tracked via GAS AttributeSet
- [ ] **ATTR-06**: Attribute changes broadcast via GAS delegates for UI/effects binding

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
| ATTR-01 | TBD | Pending |
| ATTR-02 | TBD | Pending |
| ATTR-03 | TBD | Pending |
| ATTR-04 | TBD | Pending |
| ATTR-05 | TBD | Pending |
| ATTR-06 | TBD | Pending |
| SANI-01 | TBD | Pending |
| SANI-02 | TBD | Pending |
| SANI-03 | TBD | Pending |
| SANI-04 | TBD | Pending |
| SANI-05 | TBD | Pending |
| LITE-01 | TBD | Pending |
| LITE-02 | TBD | Pending |
| LITE-03 | TBD | Pending |
| LITE-04 | TBD | Pending |
| LITE-05 | TBD | Pending |
| CONS-01 | TBD | Pending |
| CONS-02 | TBD | Pending |
| CONS-03 | TBD | Pending |
| CONS-04 | TBD | Pending |
| CONS-05 | TBD | Pending |
| CONS-06 | TBD | Pending |
| CONS-07 | TBD | Pending |
| CONS-08 | TBD | Pending |
| HALL-01 | TBD | Pending |
| HALL-02 | TBD | Pending |
| HALL-03 | TBD | Pending |
| HALL-04 | TBD | Pending |
| HALL-05 | TBD | Pending |
| HALL-06 | TBD | Pending |
| HALL-07 | TBD | Pending |
| INVS-01 | TBD | Pending |
| INVS-02 | TBD | Pending |
| INVS-03 | TBD | Pending |
| INVS-04 | TBD | Pending |
| INVS-05 | TBD | Pending |
| INVS-06 | TBD | Pending |
| INVS-07 | TBD | Pending |
| INVS-08 | TBD | Pending |
| MELE-01 | TBD | Pending |
| MELE-02 | TBD | Pending |
| MELE-03 | TBD | Pending |
| MELE-04 | TBD | Pending |
| PSTL-01 | TBD | Pending |
| PSTL-02 | TBD | Pending |
| PSTL-03 | TBD | Pending |
| PSTL-04 | TBD | Pending |
| PSTL-05 | TBD | Pending |
| PSTL-06 | TBD | Pending |
| PSTL-07 | TBD | Pending |
| ENEM-01 | TBD | Pending |
| ENEM-02 | TBD | Pending |
| ENEM-03 | TBD | Pending |
| ENEM-04 | TBD | Pending |
| ENEM-05 | TBD | Pending |
| ENEM-06 | TBD | Pending |
| ENEM-07 | TBD | Pending |
| ENEM-08 | TBD | Pending |
| NARR-01 | TBD | Pending |
| NARR-02 | TBD | Pending |
| NARR-03 | TBD | Pending |
| NARR-04 | TBD | Pending |
| NARR-05 | TBD | Pending |
| NARR-06 | TBD | Pending |
| NARR-07 | TBD | Pending |
| NARR-08 | TBD | Pending |
| ENVR-01 | TBD | Pending |
| ENVR-02 | TBD | Pending |
| ENVR-03 | TBD | Pending |
| ENVR-04 | TBD | Pending |
| ENVR-05 | TBD | Pending |

**Coverage:**
- v1 requirements: 62 total
- Mapped to phases: 0
- Unmapped: 62 (pending roadmap creation)

---
*Requirements defined: 2026-01-19*
*Last updated: 2026-01-19 after adjustments (GAS, consumables expansion)*
