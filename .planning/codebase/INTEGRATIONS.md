# External Integrations

**Analysis Date:** 2026-01-19

## APIs & External Services

**None Detected**
- No external web APIs integrated
- No online services configured
- No authentication services
- No analytics or telemetry SDKs

**Online Subsystem:**
- OnlineSubsystem module: Commented out in `ProjectSerene.Build.cs`
- Steam integration: Not enabled (referenced in comments only)

## Data Storage

**Databases:**
- None - Standalone game with no external database

**File Storage:**
- Local filesystem only
- Unreal Engine asset system (`.uasset` files)
- No cloud save integration

**Caching:**
- `DerivedDataCache/` - Engine-managed asset cache
- No custom caching layer

## Authentication & Identity

**Auth Provider:**
- None - No user authentication system

**Player Identity:**
- Local only - No online identity system

## Monitoring & Observability

**Error Tracking:**
- None configured

**Logs:**
- Unreal Engine logging system
- Custom log category: `LogProjectSerene` defined in `Source/ProjectSerene/ProjectSerene.h`
- Secondary category: `LogTemplateCharacter` in `ProjectSereneCharacter.h`

**Usage:**
```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogProjectSerene, Log, All);
DEFINE_LOG_CATEGORY(LogProjectSerene);
```

## CI/CD & Deployment

**Hosting:**
- Local development only
- No deployment pipeline detected

**CI Pipeline:**
- None configured
- No GitHub Actions, Jenkins, or build automation files

**Version Control:**
- Git: Not initialized (no `.git` directory)

## Environment Configuration

**Required env vars:**
- None - All configuration via Unreal Engine `.ini` files

**Configuration Files:**
- `Config/DefaultEngine.ini` - Engine settings
- `Config/DefaultGame.ini` - Game settings
- `Config/DefaultInput.ini` - Input bindings
- `Config/DefaultEditor.ini` - Editor preferences
- `Config/DefaultEditorPerProjectUserSettings.ini` - User-specific editor settings

**Secrets location:**
- `Config/DefaultEngine.ini` contains Android File Server security token
- Token: `SecurityToken=99C2615D4965B2B63F59959EB9382833`
- Note: This is a development-only token, not a production secret

## Webhooks & Callbacks

**Incoming:**
- None

**Outgoing:**
- None

## Internal Integration Points

**Unreal Engine Subsystems Used:**

**AI Perception System:**
- Sight sense for visual detection
- Hearing sense for audio detection
- Configured in `AShooterAIController` via `UAIPerceptionComponent`

**Environment Query System (EQS):**
- `EQS_FindRoamLocation.uasset` - AI roaming queries
- `EQS_FindSnipingLocation.uasset` - Combat position queries
- Custom context: `EnvQueryContext_Target` in `Source/ProjectSerene/Variant_Shooter/AI/`

**StateTree AI System:**
- `ST_Shooter.uasset` - Main AI behavior tree
- `ST_Shooter_ShootAtTarget.uasset` - Combat sub-tree
- Custom tasks in `ShooterStateTreeUtility.h`:
  - `FStateTreeLineOfSightToTargetCondition`
  - `FStateTreeFaceActorTask`
  - `FStateTreeFaceLocationTask`
  - `FStateTreeSetRandomFloatTask`
  - `FStateTreeShootAtTargetTask`
  - `FStateTreeSenseEnemiesTask`

**Input System:**
- Enhanced Input Actions (`IA_*.uasset`)
- Input Mapping Contexts (`IMC_*.uasset`)
- Touch interface support (`UI_TouchInterface_Shooter.uasset`)

**Navigation System:**
- Standard navmesh assumed (no custom configuration detected)

## Plugin Dependencies

**Enabled Plugins:**
| Plugin | Purpose | Scope |
|--------|---------|-------|
| ModelingToolsEditorMode | Level editing tools | Editor only |
| StateTree | AI behavior system | Runtime |
| GameplayStateTree | Gameplay StateTree integration | Runtime |

**Potential Future Integrations (commented in code):**
- OnlineSubsystem - Multiplayer networking
- OnlineSubsystemSteam - Steam platform integration
- SlateCore - Advanced UI (currently using public Slate dependency)

## Third-Party Assets

**None Detected:**
- No Marketplace plugins
- No third-party asset packs (beyond standard engine content)
- Character/weapon assets appear to be from engine templates

---

*Integration audit: 2026-01-19*
