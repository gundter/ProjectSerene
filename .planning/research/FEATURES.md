# Feature Landscape: Psychological Horror FPS

**Domain:** Story-driven psychological horror first-person shooter
**Researched:** 2026-01-19
**Confidence:** HIGH (verified against reference games and industry analysis)

## Executive Summary

Psychological horror FPS games occupy a unique space between pure survival horror (Amnesia, Outlast) and action horror (Dead Space, Resident Evil). Your project's combination of sanity mechanics, hallucination systems, limited combat, and investigation places it firmly in the "thinking person's horror" category alongside SOMA, Alan Wake, and Condemned: Criminal Origins.

The feature landscape divides clearly into three categories:
1. **Table Stakes** - Features players expect in any psychological horror FPS; missing these breaks immersion
2. **Differentiators** - Features that elevate a game above competitors; your sanity/hallucination system falls here
3. **Anti-Features** - Common mistakes that damage psychological horror specifically

---

## Table Stakes

Features users expect. Missing = product feels incomplete or broken.

| Feature | Why Expected | Complexity | Dependencies | Notes |
|---------|--------------|------------|--------------|-------|
| **First-person perspective** | Core genre requirement; enables immersion | Low | None | Fixed camera angle reinforces vulnerability |
| **Flashlight/light source** | Standard navigation tool; thematic for horror | Low | None | Alan Wake proved light-as-weapon works; battery management adds tension |
| **Audio cue system** | Players need audio feedback for threats | Medium | Sound design pipeline | Amnesia/Silent Hill demonstrated music/ambience is 50% of horror |
| **Basic environmental interaction** | Door opening, drawer searching, item pickup | Medium | Physics system | Frictional Games standard; physics adds immersion |
| **Save/checkpoint system** | Prevents frustration from permadeath | Low | None | Horror needs safe harbor moments; checkpoint placement is art |
| **Resource scarcity** | Core survival horror tension | Medium | Inventory system | Batteries, ammo, healing items must feel limited but not impossible |
| **Night vision or light mode** | Players need to see in dark environments | Medium | Rendering/post-processing | Outlast's camcorder night vision is genre-defining |
| **Environmental storytelling** | Notes, recordings, visual narrative | Medium | Level design, writing | Players expect to piece together story; reduces exposition dumps |
| **Chase sequences** | Genre expectation from Outlast, Amnesia | High | AI pathfinding, level design | Must feel dangerous but fair; clear escape routes |
| **Hiding mechanics** | Core survival loop | Medium | AI perception system | Lockers, under beds, shadows; don't make hiding spots obvious |
| **Multiple endings** | Replayability expectation | Medium | Branching narrative system | At minimum: good/bad/secret endings |
| **Jump scare budget** | Players expect some, but sparingly | Low | Audio/visual timing | 3-5 well-placed > 30 predictable |

### Implementation Notes - Table Stakes

**Audio Design is Non-Negotiable:** Research consistently shows sound design creates 50%+ of horror atmosphere. Amnesia's sanity effects, Silent Hill's industrial soundscapes, and Outlast's spatial audio are cited repeatedly. Budget significant time here.

**Light/Dark Mechanics:** Every reference game uses light/dark as core mechanic:
- Amnesia: Sanity degrades in darkness, lantern attracts enemies
- Alan Wake: Light damages enemies, flashlight is weapon
- Outlast: Night vision drains batteries, creates green-tint vulnerability

Your planned light/dark sanity system aligns perfectly with genre expectations.

---

## Differentiators

Features that set product apart. Not expected, but highly valued when done well.

| Feature | Value Proposition | Complexity | Dependencies | Notes |
|---------|-------------------|------------|--------------|-------|
| **Sanity system with visual/gameplay effects** | Core psychological horror; Amnesia proved this works | High | Rendering, AI, sound | Your light/dark affecting perception is genre-appropriate |
| **Hallucinations vs real enemies** | Creates paranoia and uncertainty | Very High | Enemy AI, visual effects, sound | Eternal Darkness pioneered; few games execute well |
| **Visual tells for reality checking** | Rewards observant players | High | Shader system, animation | Subtle difference between hallucinated and real threats |
| **PTSD/trauma as mechanic** | Thematically resonant; underexplored | High | Narrative integration | Hellblade showed this can be done respectfully |
| **Investigation/evidence system** | Differentiates from pure survival horror | High | UI system, database | Condemned: Criminal Origins is benchmark |
| **Detective protagonist** | Fresh perspective vs typical horror protagonists | Medium | Writing, voice acting | Most horror uses "random person"; detective has skills |
| **Cult investigation narrative** | Compelling antagonist type | Medium | World-building, writing | Silent Hill, Outlast 2 show cult horror works |
| **Improvised melee combat** | More grounded than military weapons | Medium | Combat system, breakable weapons | Silent Hill's pipe, Condemned's lead pipes; desperation feel |
| **Reality-shifting environments** | Layers of Fear showed this can be stunning | Very High | Level streaming, art | Requires significant art investment |
| **Unreliable narrator/perception** | Player questions everything they see | High | Narrative design | SOMA, Layers of Fear excel here |
| **Symbolic monster design** | Enemies represent trauma/psychology | High | Art direction, writing | Silent Hill 2's Pyramid Head is gold standard |
| **Dynamic sanity events** | Different effects based on context | Very High | Scripting system | Eternal Darkness's fake game crashes; SOMA's localized effects |

### Your Differentiators Analysis

Based on your project description, your strongest differentiators are:

1. **Hallucination vs Reality System** - Very few games attempt this well. Eternal Darkness pioneered "sanity effects" but most were scripted. Dynamic determination of "is this real?" based on player observation is compelling and underexplored.

2. **Detective with PTSD Investigating Cult** - This protagonist setup is unusual. Most horror uses:
   - Random person (Amnesia, Outlast)
   - Writer (Alan Wake)
   - Engineer/scientist (Dead Space, SOMA)

   A detective brings professional investigation skills + personal trauma. This tension is narratively rich.

3. **Visual Tells for Reality** - If players can learn to distinguish hallucinations from real threats through observation, you create a skill-based horror experience. This rewards replays and creates community discussion.

### Implementation Recommendations

**Sanity System Design:**
- Avoid pure "global meter" like early Amnesia (too gamey)
- Consider SOMA's approach: context-specific, creature-specific effects
- Light exposure should feel like addiction/relief, not just resource management

**Hallucination Implementation:**
- Hallucinated enemies should behave slightly wrong (movement, sound)
- Tells should be subtle but learnable (no floating icons)
- Some hallucinations should be benign to create uncertainty

**Investigation Mechanics:**
- Study Condemned: Criminal Origins for forensic tools (UV light, camera, spectrometer)
- Evidence should connect to larger narrative threads
- Let investigation moments be "breathing room" from horror

---

## Anti-Features

Features to explicitly NOT build. Common mistakes in psychological horror.

| Anti-Feature | Why Avoid | What to Do Instead |
|--------------|-----------|-------------------|
| **Overpowered combat** | Destroys vulnerability; becomes action game | Limit ammo severely; make melee risky and exhausting |
| **Completely defenseless protagonist** | Frustrating when logical options exist | Give limited, desperate options (improvised weapons, environmental hazards) |
| **Predictable jump scare patterns** | Players learn timing, fear dies | Vary timing; sometimes the scare doesn't come |
| **Too much darkness without tools** | Frustrating, not scary | Always provide some way to see (night vision, matches, adjusting eyes) |
| **Over-long game length** | Horror wears thin past 8-10 hours | Focus on 6-10 hour experience with high polish |
| **Forced co-op/multiplayer** | Destroys isolation and atmosphere | Keep single-player focused; multiplayer kills horror |
| **Stream-bait design** | Jump scares for reactions, not fear | Design for the player, not the audience |
| **Pressure puzzles in danger** | Frustrating under stress | Separate puzzle-solving from active threats |
| **Power grid/elevator puzzles** | Cliche; breaks immersion | Find contextually appropriate puzzles |
| **Tentacle monsters with glowing weak spots** | Overused trope | Design enemies around psychological themes |
| **Excessive gore as substitute for horror** | Gross != scary | Use gore sparingly for impact |
| **Sanity meter as punishment** | Makes players avoid horror content | Sanity effects should be interesting, not just debuffs |
| **Linear hallway design** | No exploration, predictable | Semi-open areas with roaming threats |
| **Explicit exposition dumps** | Breaks immersion | Environmental storytelling, found documents |
| **Mental illness as "insanity = violence"** | Harmful stereotype; poor representation | Treat PTSD with respect; consult sensitivity readers |

### Critical Anti-Patterns to Avoid

**The "Back Half Problem":**
Horror games consistently struggle after the midpoint. Research shows:
- Players become desensitized to scares
- Mechanics feel repetitive
- Story revelations reduce mystery

**Prevention:** Front-load mystery, escalate narrative (not just horror), introduce new mechanics/environments in second half.

**The Empowerment Trap:**
As players acquire resources, they feel safer. This is opposite of horror's goal.

**Prevention:**
- Take things away as game progresses
- Introduce stronger threats that obsolete old strategies
- Make protagonist's mental state worse, even if physically equipped

**The Streaming Problem:**
Modern horror often designs for YouTube/Twitch reactions rather than player experience.

**Prevention:**
- Slow-burn atmospheric horror doesn't stream well but plays excellently
- Trust the experience over the reaction
- Long-term dread > momentary screams

---

## Feature Dependencies

```
Core Systems:
  First-Person Controller
    |
    +-- Light/Dark Detection --> Sanity System
    |                              |
    |                              +-- Visual Effects (hallucinations)
    |                              |
    |                              +-- Audio Effects (distortion)
    |                              |
    |                              +-- Enemy Spawning (real vs fake)
    |
    +-- Physics/Interaction --> Investigation System
    |                              |
    |                              +-- Evidence Collection
    |                              |
    |                              +-- Environmental Storytelling
    |
    +-- Combat System --> Melee (improvised weapons)
                     |
                     +-- Ranged (pistol, limited ammo)

AI Systems:
  Enemy AI
    |
    +-- Real Enemies --> Combat encounters
    |                |
    |                +-- Chase sequences
    |
    +-- Hallucinated Enemies --> Visual/behavioral tells
                             |
                             +-- Player paranoia

Narrative Systems:
  PTSD/Trauma System --> Sanity triggers
                     |
                     +-- Flashbacks
                     |
                     +-- Unreliable perception
```

### Build Order Recommendation

**Phase 1: Core Horror Foundation**
- First-person controller
- Light/dark detection
- Basic flashlight
- Environmental interaction
- Basic enemy AI (one type)

**Phase 2: Sanity and Perception**
- Sanity system core
- Visual distortion effects
- Audio distortion
- Hallucination framework

**Phase 3: Combat and Survival**
- Melee system (improvised weapons)
- Pistol combat (limited ammo)
- Resource management
- Chase sequences

**Phase 4: Investigation and Narrative**
- Evidence collection tools
- Environmental storytelling implementation
- Dialogue/document system
- Narrative branching

**Phase 5: Differentiation**
- Hallucination vs real enemy system
- Visual tells
- PTSD flashback integration
- Advanced sanity effects

---

## MVP Recommendation

For MVP, prioritize:

### Must Have (Table Stakes)
1. First-person movement and camera
2. Flashlight with battery management
3. Basic sanity system (visual/audio distortion in darkness)
4. At least one real enemy type with chase behavior
5. Hiding mechanics
6. Environmental interaction (doors, drawers, items)
7. Basic audio design (ambient, threat cues)

### Should Have (Core Differentiator)
1. Hallucination system (even if simple: scripted fake enemies)
2. Evidence collection (basic: examine objects, find notes)
3. Improvised melee (one or two weapon types)

### Defer to Post-MVP
- Branching narrative / multiple endings
- Advanced hallucination detection (visual tells)
- Full investigation toolkit (UV light, camera, etc.)
- Reality-shifting environments
- Pistol combat (can prototype with melee-only initially)
- PTSD flashback sequences

### MVP Scope Rationale

The core horror loop is: **See threat -> Hide/Flee -> Recover in light -> Explore -> Repeat**

Everything in "Must Have" enables this loop. The "Should Have" items add your differentiating flavor without blocking the core experience.

---

## Competitive Landscape

### Direct Competitors (Similar Feature Set)

| Game | Sanity System | Combat | Investigation | Hallucinations |
|------|---------------|--------|---------------|----------------|
| **Amnesia: The Dark Descent** | Yes (global) | None | Minimal | Visual only |
| **SOMA** | Partial (local) | None | Moderate | Context-specific |
| **Condemned: Criminal Origins** | No | Melee-focused | Forensics | No |
| **Alan Wake** | No | Flashlight + gun | Minimal | Plot-based |
| **Layers of Fear** | Implicit | None | Environmental | Extensive |
| **Outlast** | No | None | Camera-based | Minimal |
| **Silent Hill 2** | Implicit | Melee + gun | Environmental | Symbolic |
| **Your Game** | Yes (light-based) | Limited (pistol + melee) | Core mechanic | Distinguishable |

### Your Competitive Position

Your game combines elements that are typically separate:
- Amnesia's sanity system
- Condemned's investigation + melee
- Alan Wake's light-as-weapon
- Silent Hill's symbolic horror

This combination is underexplored. The closest comparison is Condemned: Criminal Origins (2005), which featured investigation + melee but lacked sanity/hallucination systems.

---

## Sources

### Official/Authoritative
- [SOMA Info Portal - Gameplay](https://somainfoportal.weebly.com/general-gameplay.html)
- [Amnesia Wiki - Sanity](https://amnesia.fandom.com/wiki/Sanity)
- [Alan Wake Wiki - Flashlight](https://alanwake.fandom.com/wiki/Flashlight)
- [Condemned: Criminal Origins - Wikipedia](https://en.wikipedia.org/wiki/Condemned:_Criminal_Origins)
- [Eternal Darkness - Wikipedia](https://en.wikipedia.org/wiki/Eternal_Darkness)

### Game Design Analysis
- [Game Design Deep Dive: Amnesia's Sanity Meter](https://www.gamedeveloper.com/design/game-design-deep-dive-i-amnesia-i-s-sanity-meter-)
- [Top 5 Horror Game Mechanics Every Designer Should Know](https://drwedge.uk/2024/10/18/top-5-horror-game-mechanics-every-designer-should-know/)
- [The Problem of Modern Horror Game Design](https://game-wisdom.com/critical/problem-modern-horror-game-design)
- [Why Melee-Focused Horror Games Work Best](https://www.dualshockers.com/melee-combat-horror-games/)
- [Unlocking the Psychology Behind Horror Game Design](https://www.cubix.co/blog/psychology-behind-horror-game-design/)

### Industry Research
- [10 Design Lessons from 30 Years of Horror Games](https://www.gamedeveloper.com/design/10-horror-game-design-tips)
- [The Rise of Narrative-Driven Horror](https://www.relyonhorror.com/articles/the-rise-of-narrative-driven-horror-how-story-became-the-scariest-weapon-of-all/)
- [GameFrame: Lessons on Fear Mechanics from Silent Hill](https://medium.com/@antonio.sadaric1/gameframe-lessons-on-fear-mechanics-from-the-video-game-series-silent-hill-29ca40104e99)

### Mental Health Representation
- [Mental Health Representation in Games - CheckPoint](https://checkpointorg.com/mental-health-representation/)
- [Playing with Trauma in Video Games](https://www.academia.edu/10765455/Playing_with_Trauma_in_Video_Games_Interreactivity_Empathy_Perpetration)
