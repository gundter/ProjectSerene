# Technology Stack: Psychological Horror FPS

**Project:** Project Serene
**Domain:** Story-driven psychological horror FPS (detective PTSD, cult investigation)
**Engine:** Unreal Engine 5.7
**Researched:** 2026-01-19
**Overall Confidence:** HIGH (verified with official UE5.7 docs and current sources)

---

## Executive Summary

Project Serene builds on the UE5.7 First Person Horror template, which already provides solid foundations (HorrorCharacter with stamina/sprint, HorrorGameMode, HorrorUI). The project has Lumen GI, Virtual Shadow Maps, and Substrate materials enabled -- all production-ready in UE5.7.

**Key stack recommendations:**
- **Audio:** MetaSounds for procedural audio + Wwise for adaptive music/spatial audio
- **AI:** State Trees (already in project) for enemy behavior, AIPerception for senses
- **Visuals:** Post Process Volumes with sanity-driven parameter blending
- **Input:** Enhanced Input (already configured) for weapon/investigation actions

The existing codebase is well-structured. Extend it rather than replace it.

---

## Recommended Stack

### Core Framework (Already Configured)

| Technology | Version | Purpose | Confidence |
|------------|---------|---------|------------|
| Unreal Engine | 5.7 | Game engine | HIGH - Official |
| C++ | C++20 | Gameplay systems | HIGH - Project verified |
| Blueprints | UE5.7 | Rapid prototyping, designer tuning | HIGH |
| Enhanced Input | Built-in | Input actions/contexts | HIGH - Already configured |

**Why:** The project already uses UE5.7's modern stack. The Horror template variant provides stamina/sprint mechanics and flashlight (SpotLight) - build on this foundation.

### Audio Stack

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| MetaSounds | UE5.7 built-in | Procedural audio, real-time synthesis | Zero cost, native integration, procedural heartbeat/breathing for sanity | HIGH |
| Wwise | 2025.1.3+ | Adaptive music, spatial audio, dialogue | Industry standard for horror, 3D audio positioning, tension music systems | MEDIUM |

**Recommendation:** Use MetaSounds as primary for procedural/reactive sounds (heartbeat intensity tied to sanity, environmental ambience). Add Wwise for adaptive music and professional spatial audio IF budget allows and team has audio expertise.

**Rationale:**
- MetaSounds in UE5.7 has Channel Agnostic Types (CAT) for multichannel audio and Audio Insights for profiling
- Horror games require dynamic audio that responds to sanity state - MetaSounds excels here
- Wwise provides superior adaptive music systems (layered tension tracks that respond to gameplay state)
- FMOD is simpler but Wwise has better horror game track record (Silent Hill, Resident Evil use middleware)

**If budget-constrained:** MetaSounds alone is sufficient for an indie horror game. The UE5.7 Audio Subtitle Plugin also supports dialogue/captions.

**What NOT to use:**
- Legacy UE4 Audio Engine - Deprecated, lacks procedural capabilities
- Pure Wwise without MetaSounds - Loses native UE integration benefits for simple sounds

### Visual Effects / Post-Processing

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Lumen GI | Production (5.7) | Dynamic global illumination | Real-time flashlight bounce lighting critical for horror atmosphere | HIGH |
| Virtual Shadow Maps | Production (5.7) | Dynamic shadows | Sharp, detailed shadows without baking | HIGH |
| Substrate | Production (5.7) | Advanced materials | Blood/sweat on skin, wet surfaces, decay materials | HIGH |
| MegaLights | Beta (5.7) | Many dynamic lights | Flickering lights, candles in cult scenes | MEDIUM |
| Post Process Volumes | Built-in | Sanity effects | Vignette, grain, chromatic aberration, color grading | HIGH |

**Post-Processing Stack for Sanity System:**

```
Sanity Level: HIGH (80-100%)
- Minimal vignette
- Subtle film grain
- Normal color grading
- Clear vision

Sanity Level: MEDIUM (40-79%)
- Increased vignette intensity
- Noticeable grain
- Slight desaturation
- Occasional chromatic aberration pulses

Sanity Level: LOW (0-39%)
- Heavy vignette
- Strong grain
- Severe desaturation with color shifts
- Persistent chromatic aberration
- Bloom on light sources (hallucination auras)
- Distortion/warping (lens distortion)
```

**Implementation:** Use multiple Post Process Volumes with different settings. Blend between them via Material Parameter Collection driven by Sanity float value.

**Performance Notes:**
- Lumen Software Ray Tracing (SRT) for 60 FPS target
- Enable "Generate Mesh Distance Fields" in Lightmass settings (critical for Lumen)
- MegaLights has fixed performance budget - more lights = lower quality per light

**What NOT to use:**
- Baked lightmaps - Defeats dynamic flashlight horror
- Hardware Ray Tracing on low-end PCs - Too expensive, SRT sufficient for horror
- Over-aggressive auto-exposure - Causes strobe-like effects with Lumen, use manual exposure

### AI / Enemy Systems

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| State Trees | Production (5.7) | Enemy behavior logic | Already in project, debugger improved in 5.7 | HIGH |
| AIPerception | Built-in | Sight/hearing detection | Event-driven, performant, realistic detection | HIGH |
| Behavior Trees | Built-in | Alternative/complex AI | More tutorials available, proven pattern | HIGH |
| EQS (Environment Query System) | Built-in | Spatial queries | Find patrol points, cover, investigation locations | HIGH |

**Recommended AI Architecture:**

```
Enemy Types:
1. Real Cult Members - Full AI (patrol, investigate, chase, attack)
2. Hallucinations - Simplified AI (threaten, approach, fade on engagement)

State Tree States:
- Idle/Patrol - Wander through patrol points
- Investigate - Move to noise source, search area
- Chase - Pursue detected player
- Attack - Melee/ranged combat
- Retreat - Flee to reinforcement position

Key Blackboard Values:
- TargetActor (Object) - Detected player
- LastKnownLocation (Vector) - Where player was last seen
- ThreatLevel (Float) - Escalation state
- IsHallucination (Bool) - For tell system differentiation
```

**AIPerception Configuration:**
- Sight: 60-degree cone, 2000cm range, age-based memory (loses target after N seconds)
- Hearing: 3000cm range, reacts to footsteps/gunshots
- Team: Enemies share perception via team ID

**State Tree Rewind Debugger** (new in 5.7) allows stepping through behavior logic - use for tuning enemy reactions.

**What NOT to use:**
- Simple Blueprint tick-based detection - Not performant, not realistic
- Pawn Sensing Component - Deprecated, use AIPerception instead
- Overly complex nested Behavior Trees - State Trees are cleaner for horror AI states

### Weapons / Combat

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Enhanced Input Actions | Built-in | Fire, aim, reload, melee | Context-based input switching | HIGH |
| Damage System (TakeDamage) | Built-in | Health/damage | Standardized, works with AI | HIGH |
| Line Traces / Projectiles | Built-in | Gunshots, hit detection | Pistol hitscan or slow projectiles | HIGH |
| Physical Animation (PHAT) | Built-in | Hit reactions | Procedural stumble/flinch | MEDIUM |

**Weapon Architecture:**

```cpp
// Recommended class hierarchy
UWeaponBase (abstract)
  UPistolWeapon - Hitscan, limited ammo
  UMeleeWeapon - Improvised weapons (pipe, bottle)
  UFlashlight - Already exists as SpotLight, add battery drain
```

**Combat Design Notes:**
- Pistol: Semi-automatic, 15-round magazine, scarce ammo encourages caution
- Melee: Improvised weapons break after N uses, high risk/reward
- Flashlight: Dual purpose (sanity restoration via light + enemy reveal)

**Hallucination Combat:**
- Hallucinations "die" in one hit but don't drop items
- Real enemies require multiple hits
- Visual tell system (see FEATURES.md) helps distinguish

**What NOT to use:**
- Full weapon wheel/arsenal - Not appropriate for psychological horror, limits choices
- Regenerating health - Undermines tension
- Unlimited ammo - Removes resource management

### Investigation / Dialogue System

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Data Tables | Built-in | Evidence database | Structured data, designer-friendly | HIGH |
| Widget Blueprints (UMG) | Built-in | Evidence UI, dialogue UI | Existing HorrorUI foundation | HIGH |
| Struct-based Dialogue | Custom | Branching dialogue | Simple, no external dependencies | MEDIUM |

**Investigation Architecture:**

```cpp
// Evidence system
USTRUCT()
struct FEvidenceItem
{
    FName EvidenceID;
    FText DisplayName;
    FText Description;
    UTexture2D* Icon;
    TArray<FName> RelatedEvidenceIDs;
    bool bDiscovered;
};

// Use Data Tables for evidence definitions
// Track discovered evidence in SaveGame
```

**Dialogue Approach:**
- Simple dialogue trees via Data Tables (speaker, text, response options)
- No need for heavyweight dialogue middleware for demo scope
- The Sinking City 2 (Frogwares) uses optional investigation - good model

**What NOT to use:**
- Yarn Spinner/Ink (external dialogue tools) - Overkill for demo scope
- Complex dialogue plugins from marketplace - Adds dependencies
- Fully dynamic dialogue - Demo scope is linear

### Sanity System (Custom)

| Technology | Version | Purpose | Why | Confidence |
|------------|---------|---------|-----|------------|
| Actor Component | Built-in | SanityComponent on player | Modular, reusable | HIGH |
| Material Parameter Collection | Built-in | Drive post-process from sanity | Real-time visual response | HIGH |
| Timelines | Built-in | Smooth sanity transitions | Better than tick-based lerping | HIGH |
| Delegates/Events | Built-in | Sanity threshold notifications | Spawn hallucinations at thresholds | HIGH |

**Sanity Architecture:**

```cpp
UCLASS()
class USanityComponent : public UActorComponent
{
    UPROPERTY()
    float CurrentSanity = 100.0f; // 0-100

    UPROPERTY()
    float SanityDecayRateInDark = 5.0f; // Per second

    UPROPERTY()
    float SanityRecoveryRateInLight = 2.0f; // Per second, capped

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FSanityThresholdCrossed,
        float, NewSanity,
        bool, bDecreasing
    );

    FSanityThresholdCrossed OnSanityThresholdCrossed;

    // Called when crossing 75%, 50%, 25% thresholds
};
```

**Light Detection:**
- Use sphere traces to detect overlapping light sources
- Check light intensity at player location
- SpotLight (flashlight) counts as light source

**Sanity-Driven Systems:**
1. Post-process effects (via Material Parameter Collection)
2. Hallucination spawning (via threshold delegates)
3. Audio intensity (MetaSounds parameters)
4. Hallucination tell visibility (clearer tells at higher sanity)

### Marketplace Assets (Recommended)

| Asset | Purpose | Why Recommended | Confidence |
|-------|---------|-----------------|------------|
| Ultra Dynamic Sky | Day/night, weather | Industry standard, well-maintained | MEDIUM |
| Horror Master Pro V1 | Reference/learning | Includes AI, flickering lights, footsteps - study patterns | LOW |
| Post Process Shader - World Effect | Distortion effects | Quick hallucination visuals | LOW |

**Caution on Marketplace:**
- Use marketplace assets for learning/reference, not as core dependencies
- Horror-specific packs often have code quality issues
- Build custom systems based on observed patterns

**What NOT to buy:**
- Complete "Horror Engine" packs - Too opinionated, hard to customize
- Asset packs with heavy Blueprint dependencies - Maintenance burden
- Old UE4 assets not updated for UE5 - Compatibility issues

---

## Alternatives Considered

| Category | Recommended | Alternative | Why Not Alternative |
|----------|-------------|-------------|---------------------|
| Audio Middleware | MetaSounds + Wwise | FMOD | FMOD simpler but Wwise better for spatial audio/horror |
| Audio Middleware | MetaSounds + Wwise | MetaSounds only | Viable for indie scope, loses adaptive music polish |
| AI Behavior | State Trees | Behavior Trees | State Trees already in project, cleaner for horror states |
| Dialogue | Data Tables | Yarn Spinner | External dependency overkill for demo |
| Sanity Effects | Post Process Volumes | Material Functions | Post Process gives global screen effects |
| Investigation | Custom Data Tables | Narrative plugins | Demo scope doesn't need full narrative system |

---

## Plugin Configuration

### Required Plugins (Enable in .uproject)

```json
{
  "Plugins": [
    {"Name": "EnhancedInput", "Enabled": true},
    {"Name": "StateTree", "Enabled": true},
    {"Name": "GameplayStateTree", "Enabled": true},
    {"Name": "AIModule", "Enabled": true},
    {"Name": "NavigationSystem", "Enabled": true}
  ]
}
```

### Optional Plugins

```json
{
  "Plugins": [
    {"Name": "Wwise", "Enabled": true},  // If using Wwise
    {"Name": "NiagaraFluids", "Enabled": true},  // For blood VFX
    {"Name": "ChaosDestruction", "Enabled": true}  // For breakable objects
  ]
}
```

### Module Dependencies (ProjectSerene.Build.cs)

Current dependencies are correct. Add for new systems:

```csharp
PublicDependencyModuleNames.AddRange(new string[] {
    // Existing...
    "Niagara",  // For particle effects
    "PhysicsCore",  // For physical materials
    "NavigationSystem"  // For AI navigation
});
```

---

## Performance Targets

| Setting | Value | Rationale |
|---------|-------|-----------|
| Target FPS | 60 | Horror needs smooth visuals |
| Lumen Mode | Software Ray Tracing | HRT too expensive for 60fps |
| Shadow Quality | High (Virtual Shadow Maps) | Sharp shadows critical for horror |
| Post Process | Medium | Room for sanity effects overhead |
| AI Perception | Event-driven | Not tick-based for performance |

### Optimization Priorities

1. **Audio:** MetaSounds voice limiting (max 32 concurrent voices)
2. **Lighting:** Limit dynamic lights per area (MegaLights budget)
3. **AI:** Pool hallucination actors, don't spawn/destroy rapidly
4. **Post Process:** Single post-process volume chain, blend via parameters

---

## What NOT to Use (Anti-Stack)

| Technology | Why Avoid |
|------------|-----------|
| Baked Lightmaps | Defeats dynamic flashlight-based horror |
| Legacy Input System | Deprecated, Enhanced Input is default |
| Pawn Sensing | Deprecated, use AIPerception |
| Hardware Ray Tracing (primary) | 60fps not achievable on target hardware |
| Blueprint-heavy AI | Performance issues, hard to debug |
| Heavy marketplace horror packs | Code quality varies, customization difficult |
| VR support (for now) | Lumen performance issues in VR, different design needs |
| Multiplayer | Out of scope, adds massive complexity |
| Open world streaming | Demo scope is discrete locations |

---

## Installation / Setup

### Wwise Integration (If Using)

```bash
# Prerequisites
# - Wwise 2025.1.3+ installed via Wwise Launcher
# - Audiokinetic account

# Integration steps:
# 1. Open Wwise Launcher
# 2. Select UE 5.7 integration
# 3. Choose "Project Plugin" installation
# 4. Copy Wwise SDK to project Plugins/Wwise/ThirdParty/
# 5. Regenerate project files
# 6. Configure Wwise Project Path in Project Settings > Wwise
```

### MetaSounds Setup

```cpp
// Already available in UE5.7
// Enable Audio Insights for debugging:
// Edit > Editor Preferences > General > Experimental > Enable Audio Insights
```

### Post Process Volume Setup

```
1. Place Post_Process_Volume_Sanity_High (default settings)
2. Place Post_Process_Volume_Sanity_Low (heavy effects)
3. Create Material Parameter Collection "MPC_Sanity"
4. Add Scalar Parameter "SanityLevel" (0-1)
5. Use Post Process Material that reads MPC and lerps settings
```

---

## Sources

### Official Documentation (HIGH confidence)
- [Unreal Engine 5.7 Release Notes](https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-engine-5-7-release-notes)
- [Unreal Engine 5.7 What's New](https://dev.epicgames.com/documentation/en-us/unreal-engine/whats-new)
- [Behavior Tree Quick Start Guide](https://dev.epicgames.com/documentation/en-us/unreal-engine/behavior-tree-in-unreal-engine---quick-start-guide)
- [AI Perception Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/ai-perception-in-unreal-engine)
- [Enhanced Input Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/enhanced-input-in-unreal-engine)
- [Lumen GI Documentation](https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-global-illumination-and-reflections-in-unreal-engine)
- [Post Process Effects](https://dev.epicgames.com/documentation/en-us/unreal-engine/post-process-effects-in-unreal-engine)

### Industry References (MEDIUM confidence)
- [Wwise 2025 Release Cycle](https://www.audiokinetic.com/en/blog/wwise-2025-release-cycle-news/)
- [Wwise UE Integration Tutorial](https://generalistprogrammer.com/tutorials/wwise-unreal-engine-integration-complete-audio-tutorial)
- [MetaSounds vs Wwise Analysis](https://aircada.com/blog/metasounds-vs-wwise)
- [FMOD Adaptive Music for Unreal](https://dev.epicgames.com/community/learning/tutorials/p450/adaptive-music-in-fmod-for-unreal)
- [The Sinking City 2 Developer Interview](https://www.unrealengine.com/en-US/developer-interviews/unreal-engine-5-helps-frogwares-deliver-a-different-sort-of-horror-in-the-sinking-city-2)

### Community Resources (LOW-MEDIUM confidence)
- [Horror Game Development in UE5](https://darkskiesfilm.com/how-to-make-a-horror-game-in-unreal-engine-5/)
- [Lumen Optimization Guide](https://toxigon.com/ue5-lumen-guide)
- [AI Behavior Tree Patterns](https://awesometuts.com/blog/ai-behavior-tree-unreal-engine/)
- [Horror Post-Processing Techniques](https://toxigon.com/ue5-post-processing-effects)

### Marketplace References (LOW confidence - verify quality)
- [Horror Master Pro V1](https://www.unrealengine.com/marketplace/en-US/product/horror-engine-pro-v1)
- [Sanity System & Jumpscare Pack](https://www.fab.com/listings/0fc989ac-88c1-4a60-87de-6f324c4333b0)
- [First Person Horror Kit](https://www.fab.com/listings/c87594f7-33a0-4f8f-9d04-316f69409674)

---

## Confidence Assessment

| Area | Confidence | Reason |
|------|------------|--------|
| Core UE5.7 Stack | HIGH | Verified with official 5.7 release notes |
| Lumen/Post-Process | HIGH | Official docs, production-ready status confirmed |
| State Trees/AI | HIGH | Already in project, official quick start verified |
| MetaSounds | HIGH | Built-in, official docs confirm 5.7 improvements |
| Wwise Integration | MEDIUM | 2025.1.3 release confirmed, UE5.6+ support verified |
| Sanity System Approach | MEDIUM | Common pattern, no official "sanity" system exists |
| Marketplace Assets | LOW | Quality varies, recommend custom implementation |

---

*Stack research complete. Ready for roadmap creation.*
