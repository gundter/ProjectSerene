# Domain Pitfalls: Psychological Horror FPS

**Domain:** Story-driven psychological horror FPS with sanity mechanics
**Researched:** 2026-01-19
**Confidence:** MEDIUM (synthesized from multiple credible sources, industry postmortems)

---

## Critical Pitfalls

Mistakes that cause rewrites, player abandonment, or fundamental design failures.

---

### Pitfall 1: Sanity System Becomes a Resource Management Game

**What goes wrong:** Players stop experiencing psychological horror and start optimizing tinderbox counts. The sanity meter becomes "another bar to manage" rather than a tool for dread. Some players hoard resources and trivialize the system; others stress about it systematically rather than emotionally.

**Why it happens:** Sanity mechanics that rely heavily on consumable items (tinderboxes, lamp oil, batteries) create inventory optimization loops. Players with system-gaming mindsets will min-max instead of immerse.

**Consequences:**
- Horror atmosphere destroyed by "gamey" resource hunting
- Playtesting reveals wildly inconsistent experiences (some players never notice sanity, others obsess over it)
- Late-game balance becomes impossible when some players hoarded while others didn't

**Warning signs:**
- Playtesters asking "where can I find more tinderboxes?" instead of commenting on atmosphere
- Players pausing to count inventory before entering dark areas
- Feedback mentions "frustrating" or "tedious" more than "scary"

**Prevention:**
- Make sanity effects primarily aesthetic/perceptual rather than mechanically punishing
- Use sanity as a "placebo mechanic" (Amnesia approach) - players fear consequences that don't mechanically exist
- Tie sanity recovery to narrative moments and safe spaces rather than consumable items
- Keep mechanical impact of low sanity ambiguous so imagination does the work

**Phase to address:** Core Mechanics phase - sanity system design must be right from the start

**Source:** [Game Design Deep Dive: Amnesia's Sanity Meter](https://www.gamedeveloper.com/design/game-design-deep-dive-i-amnesia-i-s-sanity-meter-) - Thomas Grip's detailed postmortem

---

### Pitfall 2: Hallucination/Reality Distinction Creates Frustration, Not Fear

**What goes wrong:** Players can't tell hallucinations from real threats, leading to wasted resources (ammo, healing) on fake enemies, or death from ignoring real ones. Frustration replaces fear.

**Why it happens:** The appeal of "is it real?" tension leads designers to make hallucinations too convincing. Without learnable tells, players feel cheated rather than scared.

**Consequences:**
- Players waste limited ammo on hallucinations, creating softlock situations
- "Trial and error" gameplay replaces psychological tension
- Players either ignore all enemies (risky) or attack all (resource drain)
- Negative reviews citing "cheap deaths" and "unfair mechanics"

**Warning signs:**
- Playtesters dying repeatedly to the same situation
- Questions like "how was I supposed to know that was fake?"
- Players developing meta-strategies (always shoot, never shoot) instead of engaging with the system

**Prevention:**
- **Implement learnable visual tells** that are subtle but consistent (e.g., hallucinations flicker slightly, don't cast shadows, move differently)
- **Gradual revelation:** Early game hallucinations are obviously fake; tells become subtler as sanity drops
- **Different threat levels:** Hallucinations can still be threatening (drain sanity, cause panic effects) without being lethal
- **Environmental tells:** Real enemies affect the world (knock things over, leave tracks); hallucinations don't
- **Audio cues:** Real enemies have spatial audio; hallucinations may have distorted or non-directional sound

**Phase to address:** Enemy Design phase - must establish rules before building content

**Source:** [Horror Game Mechanics - Dr. Wedge](https://drwedge.uk/2024/10/18/top-5-horror-game-mechanics-every-designer-should-know/)

---

### Pitfall 3: Light/Dark Mechanic Becomes Unplayable

**What goes wrong:** Darkness intended for atmosphere becomes "I literally cannot see what I'm doing." Players crank brightness to 100%, ruining the horror aesthetic, or quit in frustration.

**Why it happens:** Darkness is subjective - varies by monitor, room lighting, player vision. What looks atmospheric on a developer's calibrated monitor is unplayable on a laptop in a bright room.

**Consequences:**
- Players either can't play or destroy atmosphere by maxing brightness
- Navigation becomes tedious guesswork
- Combat becomes impossible when players can't see enemies
- Accessibility failure for visually impaired players

**Warning signs:**
- Any playtester adjusting brightness mid-session
- Comments about "bumping into walls" or "can't find the door"
- Players using flashlight continuously, never experiencing dark atmosphere

**Prevention:**
- **Test on multiple displays** including laptops, various monitor calibrations
- **Provide meaningful visibility in darkness** - outlines, ambient occlusion, subtle environmental glow
- **Make darkness purposeful, not constant** - use as a tool for specific scares, not default state
- **Light sources as gameplay** (Alan Wake model) - flashlight as tool, not just visibility aid
- **Adaptive darkness** - if player is struggling, subtly increase ambient light
- **Brightness calibration that actually works** - not just "adjust until logo barely visible"

**Phase to address:** Core Mechanics phase (lighting system), Level Design phase (application)

**Source:** [The Wailer Uses Darkness to Create Fear Without Frustration](https://www.dreadxp.com/editorial/the-wailer-uses-darkness-to-create-fear-without-frustration/), [Frictional Games Blog](https://frictionalgames.com/2009-11-the-struggle-between-light-and-dark/)

---

### Pitfall 4: Combat Undermines Horror Atmosphere

**What goes wrong:** Once players have weapons, they feel empowered rather than vulnerable. The game shifts from psychological horror to action shooter. Or worse: combat is clunky and frustrating, making players dread encounters for the wrong reasons.

**Why it happens:** FPS combat naturally creates power fantasy. Horror requires vulnerability. These goals conflict unless deliberately balanced.

**Consequences:**
- Players become "hunters" instead of "hunted" - fear evaporates
- Action set-pieces feel disconnected from psychological horror
- Clunky combat creates frustration, not tension
- Resource scarcity (intended for tension) creates softlock states

**Warning signs:**
- Playtesters actively seeking combat encounters
- Comments like "I had so much ammo I wasn't worried"
- Complaints about combat being "annoying" rather than "tense"
- Players clearing areas systematically instead of progressing fearfully

**Prevention:**
- **Combat should be desperate, not empowering** - weapons are last resort, not power tools
- **Strategic scarcity** - enough ammo to survive, never enough to feel safe
- **Combat has costs** - noise attracts more enemies, sanity impact from violence
- **Enemies that don't reward direct engagement** - some can't be killed, some are better avoided
- **Make fleeing viable and sometimes necessary** - not every encounter should be combat
- **Improvised melee = risky close range** - high damage potential but dangerous positioning

**Phase to address:** Combat Design phase - establish philosophy before implementing weapons

**Source:** [More Horror Games Should Make Combat Fun](https://bloody-disgusting.com/news/3344199/horror-games-make-combat-fun/), [Silent Hill f combat debates](https://www.gamesradar.com/games/silent-hill/silent-hill-f-fans-cant-decide-if-its-combat-is-ruining-the-whole-experience-with-fights-harder-than-elden-ring-or-if-the-new-horror-game-was-designed-for-you-to-pick-your-battles/)

---

### Pitfall 5: PTSD/Mental Health Representation Causes Harm

**What goes wrong:** PTSD depicted through harmful stereotypes (homicidal veteran, psychotic breaks as gameplay mechanic, mental illness as "scary"). Creates backlash, harms real people with PTSD, and produces shallow narrative.

**Why it happens:** Horror games historically treat mental illness as source of horror rather than human experience. "Insanity" is used as shorthand for "scary and unpredictable."

**Consequences:**
- Reinforces stigma against people with PTSD
- Narrative feels exploitative rather than empathetic
- Criticism from mental health advocates and reviewers
- Missed opportunity for genuine psychological depth

**Warning signs:**
- PTSD symptoms used only to justify violence or "scary moments"
- Mental health as spectacle rather than character depth
- No consultation with people who have lived experience
- Flashbacks/hallucinations purely as horror setpieces with no narrative purpose

**Prevention:**
- **Consult with mental health professionals and people with lived experience** (like Hellblade: Senua's Sacrifice)
- **PTSD symptoms should be accurate** - intrusive memories, avoidance, hyperarousal, not just "sees scary things"
- **Character's mental state is their struggle, not their threat** - they're fighting their condition, not defined by it
- **Horror comes from the situation, not the character's mental illness**
- **Treatment/recovery should be possible** - don't portray mental illness as permanent doom
- **Avoid: "the mentally ill detective who's the real monster" trope**

**Phase to address:** Narrative Design phase - establish character with sensitivity before building mechanics around them

**Source:** [Mental Health Portrayals in Video Games - Psychiatric Times](https://www.psychiatrictimes.com/view/lessons-learned-mental-health-portrayals-video-games), [Checkpoint Mental Health Representation](https://checkpointorg.com/mental-health-representation/)

---

## Moderate Pitfalls

Mistakes that cause delays, technical debt, or diminished player experience.

---

### Pitfall 6: Tension Fatigue from Constant Horror

**What goes wrong:** Game maintains maximum tension throughout. Players become exhausted, desensitized, or simply quit. Horror stops working because there's no contrast.

**Why it happens:** Developers fear "boring" players, so they fill every moment with threats. Misunderstanding that horror requires valleys to make peaks effective.

**Consequences:**
- Players quit from exhaustion before finishing
- Scares stop working partway through
- "The game is too stressful" reviews (not a compliment)
- Players play in short sessions, losing narrative immersion

**Warning signs:**
- Playtesters taking breaks mid-level
- Comments about feeling "exhausted" or "stressed out" (negatively)
- Players developing numbness to scares after first hour
- Session lengths decreasing over time

**Prevention:**
- **Design tension curves** - build up, release, build higher, release
- **Safe spaces are mandatory** - save rooms, well-lit areas, moments of calm
- **Vary encounter types** - not every room needs a threat
- **Investigation moments provide natural valleys** - reading documents, solving puzzles, exploring safely
- **Music and ambience should breathe** - silence is a tool

**Phase to address:** Level Design phase - pacing must be intentional

**Source:** [The Balancing Act of Tension in Horror Game Design](https://www.gamedeveloper.com/design/the-balancing-act-of-tension-in-horror-game-design), [Pacing in Horror Games - Cliqist](https://cliqist.com/2018/10/24/pacing-in-horror-games-why-its-important/)

---

### Pitfall 7: Investigation System Becomes Pixel-Hunting or Brute-Force

**What goes wrong:** Clue-finding degrades into clicking everything or trying every item combination. Detective work becomes tedious fetch quest rather than engaging mystery.

**Why it happens:** Investigation mechanics are hard to balance. Too obvious = no challenge. Too obscure = frustration. Too few solutions = brute-forceable.

**Consequences:**
- Players using guides to progress
- "I found that by accident" experiences
- Investigation feels like obstacle to horror, not part of it
- Immersion breaks when players start systematic clicking

**Warning signs:**
- Playtesters clicking randomly on objects
- Long pauses where players don't know what to do
- "I don't know what the game wants" feedback
- Players solving puzzles without understanding why the solution worked

**Prevention:**
- **Three Clue Rule** - any important conclusion should be reachable through at least three different clues
- **Multiple input channels** - visual, audio, documents, environmental storytelling all point to solutions
- **Make clues discoverable but require interpretation** - finding isn't the puzzle, understanding is
- **Hint system that's diegetic** - detective's notes, internal monologue, not UI hints
- **Fail states that teach** - wrong conclusions have consequences but aren't game-ending
- **Evidence board/journal** - helps players track what they know and make connections

**Phase to address:** Investigation Mechanics phase - design system before creating content

**Source:** [Detective Game Design Problems](https://digitales.games/blog/detective-game-design-problems), [Three Clue Rule - The Alexandrian](https://thealexandrian.net/wordpress/1118/roleplaying-games/three-clue-rule)

---

### Pitfall 8: UE5 Performance Issues from Leaving Features Enabled

**What goes wrong:** Game runs poorly even on good hardware because Nanite, Lumen, and other UE5 features were left at defaults without optimization. Horror games especially suffer because frame drops destroy tension.

**Why it happens:** UE5's default settings are designed for high-end hardware. Developers use features beyond their optimization skill. "Optimization later" becomes "optimization never."

**Consequences:**
- Stuttering during critical horror moments
- Player hardware requirements higher than necessary
- Performance complaints dominate reviews
- Frame rate drops telegraph scares (loading new threats)

**Warning signs:**
- Frame rate drops in editor during testing
- "Works fine on my machine" thinking
- Waiting until late development to test on varied hardware
- Using Nanite/Lumen without understanding their costs

**Prevention:**
- **Optimize early and continuously** - don't leave until final months
- **Test on target minimum spec from day one**
- **Understand what you're enabling** - Lumen is expensive; decide if you need it
- **Profile regularly** - identify performance bottlenecks before they compound
- **Consider alternatives to expensive features** - baked lighting can look great for horror
- **Budget for optimization time** - it's not optional

**Phase to address:** Technical Foundation phase - set performance baselines immediately

**Source:** [Epic CEO on UE5 Performance Issues](https://www.videogameschronicle.com/news/unreal-engine-5-performance-issues-are-mainly-due-to-devs-not-optimising-properly-epic-ceo-tim-sweeney-says/)

---

### Pitfall 9: Stalker Enemies Become Predictable Annoyances

**What goes wrong:** Once players learn stalker enemy rules (patrol patterns, hiding spots that work, speed comparisons), fear transforms into tedium. The monster becomes "a moving obstacle to manage."

**Why it happens:** Stalker mechanics require rules for the player to survive. Once rules are understood, fear based on uncertainty dies.

**Consequences:**
- Encounters become routine hide-and-seek
- Players exploit AI patterns
- "Scary" enemy becomes annoying interruption
- Horror replaced by irritation

**Warning signs:**
- Playtesters sighing when stalker appears (not gasping)
- Players developing reliable strategies
- Comments about encounters feeling "samey"
- Speedrun-style optimization of stealth sections

**Prevention:**
- **Vary stalker behaviors** - don't use single enemy with single pattern
- **Environmental randomization** - hiding spots shouldn't always work
- **Stalker encounters should be rare** - overexposure breeds contempt
- **Make encounters memorable** - unique setpieces rather than repeated mechanics
- **Give stalker narrative presence** - fear the character, not just the mechanic
- **Sometimes stalker is actually avoidable** - not every appearance requires hiding

**Phase to address:** Enemy Design phase, Level Design phase

**Source:** [XDA Horror Tropes Article](https://www.xda-developers.com/worst-horror-gaming-tropes-i-hope-go-extinct-in-2026/)

---

### Pitfall 10: Choices Don't Actually Matter (False Agency)

**What goes wrong:** Game presents meaningful-seeming choices that converge to same outcome. Players feel betrayed when they realize their decisions were illusory.

**Why it happens:** True branching is expensive to develop. Designers want choice feeling without resource cost of multiple paths.

**Consequences:**
- Players feel manipulated and betrayed
- Replay value destroyed when illusion revealed
- Reviews mention "choices don't matter"
- Cynicism about future choices undermines later genuine ones

**Warning signs:**
- Development team talking about choices "feeling" impactful rather than being impactful
- All playthrough videos ending identically
- Dialogue variations without consequence variations
- Players on second playthrough discovering same outcomes

**Prevention:**
- **Fewer choices with real consequences** rather than many false ones
- **Be honest about scope** - a linear horror experience is fine; fake choice is not
- **Small, tangible consequences** better than promised large ones - a character's dialogue changes, an item becomes available
- **Investigation conclusions should actually matter** - wrong deductions have outcomes
- **If offering choice, commit to branches** - at least for the immediate scene

**Phase to address:** Narrative Design phase - decide early what choices you can actually support

**Source:** [The Illusion of Choice - Wayline](https://www.wayline.io/blog/illusion-of-choice-killing-video-game-narratives), [Game Studies on Player Agency](https://gamestudies.org/1901/articles/stang)

---

## Minor Pitfalls

Mistakes that cause polish issues or missed opportunities.

---

### Pitfall 11: Jumpscares Overused Until Ineffective

**What goes wrong:** Game relies on jumpscares for horror rather than atmosphere. Players become desensitized. Horror reputation suffers.

**Prevention:**
- Use jumpscares sparingly - they're punctuation, not sentences
- Build tension before release; unearned jumpscares feel cheap
- Subvert jumpscare expectations sometimes - build up to nothing
- Environmental horror (wrongness, dread) is more sustainable than startles

---

### Pitfall 12: Audio Design Becomes Repetitive

**What goes wrong:** Same scare stings, same ambient drones, same footstep patterns. Audio becomes predictable and loses impact.

**Prevention:**
- Vary audio cues - multiple versions of common sounds
- Silence is a tool - don't fill every moment
- Audio should sometimes lie - sounds that suggest threats that aren't there
- Spatial audio critical for horror - directionality matters

---

### Pitfall 13: Overexplaining the Horror

**What goes wrong:** Game explains every mystery, removing the dread of the unknown. Cult motivations fully detailed, monster origins explained, all questions answered.

**Prevention:**
- Leave some mysteries unsolved
- Player imagination fills gaps more terrifyingly than any explanation
- Resist urge to create detailed lore documents accessible in-game
- "What is that thing?" is scarier than any answer

---

### Pitfall 14: Demo Scope Creep

**What goes wrong:** Demo intended to show core mechanics grows to include "just one more feature." Timeline extends, demo never ships, or ships incomplete.

**Prevention:**
- Define demo scope in writing before development
- Core loop only - sanity, investigation, one combat encounter, one hallucination type
- Demo is vertical slice, not horizontal survey
- Cut features, not quality

**Source:** [Scope Creep in Indie Games - Wayline](https://www.wayline.io/blog/scope-creep-indie-games-avoiding-development-hell)

---

## Phase-Specific Warnings

| Phase | Likely Pitfall | Mitigation | Priority |
|-------|---------------|------------|----------|
| Core Mechanics | Sanity becomes resource game (#1) | Design as perception system, not resource | CRITICAL |
| Core Mechanics | Light/dark unplayable (#3) | Test on multiple displays immediately | HIGH |
| Enemy Design | Hallucination frustration (#2) | Establish learnable tells early | CRITICAL |
| Enemy Design | Stalker predictability (#9) | Variety and rarity over repetition | MEDIUM |
| Combat Design | Combat undermines horror (#4) | Desperate, costly, avoidable combat | HIGH |
| Narrative | PTSD misrepresentation (#5) | Consult lived experience | HIGH |
| Narrative | False agency (#10) | Commit to real consequences or don't offer choice | MEDIUM |
| Investigation | Pixel hunting (#7) | Three Clue Rule, diegetic hints | HIGH |
| Level Design | Tension fatigue (#6) | Explicit tension curves in design docs | MEDIUM |
| Technical | UE5 performance (#8) | Optimize from day one | HIGH |
| Polish | Jumpscare overuse (#11) | Earn scares through atmosphere | LOW |

---

## Red Lines: Things That Will Kill The Project

1. **Sanity system that players can ignore or exploit** - Core mechanic must work or game has no identity
2. **Hallucinations indistinguishable from reality with no learning curve** - Creates rage-quit frustration
3. **PTSD portrayed as "detective goes crazy and is scary"** - Ethical failure and critical backlash
4. **Demo scope expanding beyond single investigation slice** - Project death by ambition
5. **Combat that makes players feel powerful** - Undermines entire horror premise

---

## Sources

### High Confidence (Official Postmortems, Developer Insights)
- [Game Design Deep Dive: Amnesia's Sanity Meter - Gamasutra](https://www.gamedeveloper.com/design/game-design-deep-dive-i-amnesia-i-s-sanity-meter-)
- [The Balancing Act of Tension in Horror Game Design - Gamasutra](https://www.gamedeveloper.com/design/the-balancing-act-of-tension-in-horror-game-design)
- [Detective Game Design Problems - DigiTales Interactive](https://digitales.games/blog/detective-game-design-problems)
- [Epic CEO on UE5 Optimization](https://www.videogameschronicle.com/news/unreal-engine-5-performance-issues-are-mainly-due-to-devs-not-optimising-properly-epic-ceo-tim-sweeney-says/)

### Medium Confidence (Industry Analysis, Academic Research)
- [Mental Health Portrayals in Video Games - Psychiatric Times](https://www.psychiatrictimes.com/view/lessons-learned-mental-health-portrayals-video-games)
- [Checkpoint Mental Health Representation](https://checkpointorg.com/mental-health-representation/)
- [The Illusion of Choice - Wayline](https://www.wayline.io/blog/illusion-of-choice-killing-video-game-narratives)
- [Three Clue Rule - The Alexandrian](https://thealexandrian.net/wordpress/1118/roleplaying-games/three-clue-rule)

### Supporting Sources (Community Wisdom, Analysis)
- [Horror Game Mechanics - Dr. Wedge](https://drwedge.uk/2024/10/18/top-5-horror-game-mechanics-every-designer-should-know/)
- [The Wailer Darkness Design](https://www.dreadxp.com/editorial/the-wailer-uses-darkness-to-create-fear-without-frustration/)
- [Frictional Games Light/Dark Blog](https://frictionalgames.com/2009-11-the-struggle-between-light-and-dark/)
- [Scope Creep in Indie Games - Wayline](https://www.wayline.io/blog/scope-creep-indie-games-avoiding-development-hell)
