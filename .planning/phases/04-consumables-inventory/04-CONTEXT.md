# Phase 4: Consumables & Inventory - Context

**Gathered:** 2026-01-21
**Status:** Ready for planning

<domain>
## Phase Boundary

Player can collect and use items (batteries, med kits, energy drinks, medication) to manage their attributes through an inventory system. Items are picked up from the world and used via inventory UI or radial menu. Creating resource decisions through scarcity and carry limits.

</domain>

<decisions>
## Implementation Decisions

### Inventory UI
- Categorized tabs: Medical / Tools / Evidence
- Tab opens with Tab key (toggle), click to select item
- Radial menu for quick access with 4 player-assigned slots
- Game pauses when full inventory is open
- Item tooltips show name, quantity, and effect description (e.g., "Restores 50 health")

### Pickup Behavior
- Manual pickup with E key (no auto-pickup)
- Looking at item shows highlight + prompt ("Press E to pick up [Item Name]")
- Stack limits per item type (no total slot limit)
- Cannot pick up items at max stack — "Inventory full" message, item stays in world

### Item Usage
- Brief use animation (1-2 seconds), can be interrupted
- Can use items while walking, not while sprinting/aiming/attacking
- No artificial cooldown — animation duration naturally prevents spam
- Minimal feedback: just attribute value changes on HUD

### Resource Balance
- Availability scales with difficulty (Moderate as Normal baseline)
- Batteries: Full restore (0 → 100%)
- Health: Tiered — Bandages (+30%), Med kits (+75%)
- Sanity: Mild medication (+30 fixed), Strong medication (full restore)
- Energy drinks: Restore stamina (amount TBD by planner)
- Stack limits variable by rarity: common items (5), rare items (2)

### Claude's Discretion
- Energy drink restoration amount
- Exact animation timings within 1-2 second range
- Highlight visual style (outline, glow, etc.)
- Tab UI layout details within category structure
- Item pickup sound effects

</decisions>

<specifics>
## Specific Ideas

- Radial menu: Tab to toggle open, click item to use (not hold-and-release)
- Four radial slots forces hard choices about what to quick-access
- Difficulty scaling affects spawn rates, not item effectiveness
- "Inventory full" leaves item in world rather than offering swaps

</specifics>

<deferred>
## Deferred Ideas

None — discussion stayed within phase scope

</deferred>

---

*Phase: 04-consumables-inventory*
*Context gathered: 2026-01-21*
