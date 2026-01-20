# Phase 1: Core Attributes (GAS Foundation) - Context

**Gathered:** 2026-01-20
**Status:** Ready for planning

<domain>
## Phase Boundary

Set up Gameplay Ability System (GAS) with four player attributes (Health, Stamina, Sanity, Battery) that broadcast changes for UI and gameplay effects. This phase establishes the attribute foundation that all other systems depend on.

</domain>

<decisions>
## Implementation Decisions

### Architecture
- GAS and Attributes owned by Player State (not Character)
- Character and Player Controller access components through Player State
- Use Gameplay Tags and Gameplay Tasks throughout
- Follow industry-standard GAS implementation patterns

### Attribute Starting Values
- Health: 100 (full)
- Stamina: 100 (full)
- Sanity: 100 (full)
- Battery: 100 (full)

### Stamina Behavior
- Regenerates automatically but slowly when not sprinting
- Faster regen when standing still vs walking
- At 0 stamina: forced to walk (no sprint)
- Must reach 20% stamina before sprinting again

### Health Behavior
- Passive regen up to 30% threshold only
- Above 30% requires med kits
- At 0 health: instant death
- Low health (sub-30%): limp animation, same speed

### Sanity Behavior
- Light exposure restores sanity, capped at 80% max
- Medication (Phase 4) can restore to 100%
- Darkness drain: linear rate, ~2 min to reach critical from full
- Light regen faster than darkness drain
- At 0 sanity: severe debuff (max hallucinations, impaired) but not death
- Can recover from 0 with light (slowly)
- Low sanity: perception effects only, controls stay responsive

### Battery Behavior
- Moderate drain rate (~5-7 min of continuous use)
- At 0 battery: flicker warning (1-2 sec) then flashlight off

### Universal Attribute Behaviors
- 2-3 second grace period before drain/regen starts
- Attributes can overfill above 100 temporarily (from powerful items)
- Visual/audio warnings intensify gradually as values drop (not threshold-based)

### HUD Display
- Bars only (no numbers)
- Consistent style across all attribute bars
- Contextual visibility: show on change, fade after a few seconds

### Claude's Discretion
- Exact HUD positioning
- Overfill drain rate back to 100
- Specific regen/drain rate tuning within the described feel
- HUD fade timing

</decisions>

<specifics>
## Specific Ideas

- "GAS/Attributes on Player State, accessed through Player State" — industry standard pattern for persistence across levels
- "Use Gameplay Tags and Gameplay Tasks" — explicit requirement for proper GAS integration
- Limp animation at low health adds physicality without gameplay penalty

</specifics>

<deferred>
## Deferred Ideas

None — discussion stayed within phase scope

</deferred>

---

*Phase: 01-core-attributes*
*Context gathered: 2026-01-20*
