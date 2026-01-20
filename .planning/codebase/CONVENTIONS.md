# Coding Conventions

**Analysis Date:** 2026-01-19

## Naming Patterns

**Files:**
- Class name matches filename: `ShooterCharacter.h`, `ShooterCharacter.cpp`
- Header/implementation pairs share the same base name
- Use PascalCase for all file names

**Classes:**
- Prefix with UE type: `A` for Actors, `U` for UObjects, `F` for structs, `I` for interfaces, `E` for enums
- Project prefix optional but not used: Classes are `AShooterCharacter` not `AProjectSereneShooterCharacter`
- Interface prefix: `IShooterWeaponHolder` (UInterface counterpart: `UShooterWeaponHolder`)

**Functions:**
- PascalCase: `BeginPlay()`, `SetupPlayerInputComponent()`, `DoStartFiring()`
- `Do` prefix for input handlers: `DoAim()`, `DoMove()`, `DoJumpStart()`, `DoStartSprint()`
- `On` prefix for event handlers: `OnPawnDestroyed()`, `OnBulletCountUpdated()`, `OnRespawn()`
- `BP_` prefix for BlueprintImplementableEvents: `BP_OnDeath()`, `BP_UpdateScore()`, `BP_OnProjectileHit()`
- Getters use `Get` prefix with const: `GetFirstPersonMesh() const`, `GetCurrentTarget() const`
- Boolean getters use `Is`/`Should`: `IsDead() const`, `ShouldUseTouchControls() const`

**Variables:**
- PascalCase for member variables: `FirstPersonMesh`, `CurrentBullets`, `MaxHP`
- `b` prefix for booleans: `bIsFiring`, `bSprinting`, `bRecovering`, `bFullAuto`
- Pointer prefix pattern: `TObjectPtr<>` for UPROPERTY pointers, raw pointers for interfaces

**UPROPERTY Specifiers:**
- Components: `VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true")`
- Editable configs: `EditAnywhere, Category="CategoryName"`
- Clamped values: `meta = (ClampMin = 0, ClampMax = 100, Units = "cm")`
- Blueprint callable: `BlueprintCallable, Category="Input"`

## Code Style

**Formatting:**
- Tab indentation (standard Unreal style)
- Opening brace on same line for control structures
- Braces always used even for single statements
- Blank line after `Super::` calls

**Header Guards:**
```cpp
#pragma once
```

**Include Order:**
1. Matching header file (in .cpp)
2. Engine headers
3. Project headers
4. Generated header last

**Example from `ShooterCharacter.cpp`:**
```cpp
#include "ShooterCharacter.h"
#include "ShooterWeapon.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
// ... more engine headers
#include "ShooterGameMode.h"
```

## Import Organization

**Header Dependencies:**
- Use forward declarations in headers when possible
- Include full headers in .cpp files
- Generated headers always included last in .h: `#include "ClassName.generated.h"`

**Forward Declaration Pattern:**
```cpp
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
```

**Path Aliases:**
- Public include paths defined in `ProjectSerene.Build.cs`:
  - `ProjectSerene`
  - `ProjectSerene/Variant_Horror`
  - `ProjectSerene/Variant_Horror/UI`
  - `ProjectSerene/Variant_Shooter`
  - `ProjectSerene/Variant_Shooter/AI`
  - `ProjectSerene/Variant_Shooter/UI`
  - `ProjectSerene/Variant_Shooter/Weapons`
- Headers can be included without full paths: `#include "ShooterWeapon.h"` instead of `#include "Variant_Shooter/Weapons/ShooterWeapon.h"`

## Error Handling

**Patterns:**
- Early return guards for invalid state
- Null/validity checks before dereferencing
- `IsValid()` for TObjectPtr/Actor checks

**Example Guards:**
```cpp
// Early return for invalid state
if (CurrentHP <= 0.0f)
{
    return 0.0f;
}

// Null check before use
if (CurrentWeapon && !IsDead())
{
    CurrentWeapon->StartFiring();
}

// IsValid for TObjectPtr
if (IsValid(BulletCounterUI))
{
    BulletCounterUI->BP_UpdateBulletCounter(0, 0);
}
```

**Cast Pattern:**
```cpp
if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
{
    // Use ShooterCharacter safely
}
```

## Logging

**Framework:** UE_LOG with custom category

**Log Category Definition (`ProjectSerene.h`):**
```cpp
DECLARE_LOG_CATEGORY_EXTERN(LogProjectSerene, Log, All);
```

**Log Category Implementation (`ProjectSerene.cpp`):**
```cpp
DEFINE_LOG_CATEGORY(LogProjectSerene)
```

**Usage Pattern:**
```cpp
UE_LOG(LogProjectSerene, Error, TEXT("Could not spawn mobile controls widget."));
UE_LOG(LogProjectSerene, Error, TEXT("'%s' Failed to find an Enhanced Input Component!"), *GetNameSafe(this));
```

**When to Log:**
- Error conditions: Failed widget creation, missing components
- Use `Error` verbosity for failures that affect gameplay

## Comments

**When to Comment:**
- Brief description for each function in implementation
- Purpose comments for member variables
- `// stub` for intentionally empty implementations
- `// unused` for interface methods not needed by the implementer

**DocComment Pattern for Headers:**
```cpp
/** Brief description of the class */
UCLASS(abstract)
class AProjectSereneCharacter : public ACharacter

/** Brief description of the property */
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
USkeletalMeshComponent* FirstPersonMesh;

/** Brief description of the function */
void DoAim(float Yaw, float Pitch);
```

**Implementation Comments:**
```cpp
void AShooterCharacter::DoStartFiring()
{
    // fire the current weapon
    if (CurrentWeapon && !IsDead())
    {
        CurrentWeapon->StartFiring();
    }
}
```

**Interface Region Comments:**
```cpp
//~Begin IShooterWeaponHolder interface
virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) override;
// ... more interface methods
//~End IShooterWeaponHolder interface
```

## Function Design

**Size:** Functions are focused and typically under 50 lines

**Parameters:**
- Use `const` references for complex types: `const FInputActionValue& Value`
- Use `const` for TSubclassOf references: `const TSubclassOf<AShooterWeapon>& WeaponClass`
- Use raw pointers for output/modification: `AShooterWeapon* Weapon`

**Return Values:**
- Return `nullptr` for not-found conditions
- Return early on error conditions
- Use `const` on member getters

**Virtual Function Pattern:**
```cpp
// Header
virtual void DoAim(float Yaw, float Pitch);

// Base implementation
void AProjectSereneCharacter::DoAim(float Yaw, float Pitch)
{
    if (GetController())
    {
        AddControllerYawInput(Yaw);
        AddControllerPitchInput(Pitch);
    }
}

// Override in derived class
void AShooterCharacter::DoAim(float Yaw, float Pitch)
{
    if (!IsDead())
    {
        Super::DoAim(Yaw, Pitch);
    }
}
```

## Module Design

**Class Hierarchy:**
- Base classes are `abstract` (UCLASS specifier)
- Concrete implementations exist as Blueprint subclasses
- Variants extend base classes: `AShooterCharacter : AProjectSereneCharacter`

**Component Creation Pattern:**
```cpp
// In constructor
FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
FirstPersonMesh->SetupAttachment(GetMesh());
FirstPersonMesh->SetOnlyOwnerSee(true);
FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));
```

**Delegate Pattern:**
```cpp
// Declaration in header
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBulletCountUpdatedDelegate, int32, MagazineSize, int32, Bullets);

// Member declaration
FBulletCountUpdatedDelegate OnBulletCountUpdated;

// Broadcasting
OnBulletCountUpdated.Broadcast(MagazineSize, CurrentAmmo);

// Binding
ShooterCharacter->OnBulletCountUpdated.AddDynamic(this, &AShooterPlayerController::OnBulletCountUpdated);
```

**Timer Pattern:**
```cpp
// Member
FTimerHandle RespawnTimer;

// Setting timer
GetWorld()->GetTimerManager().SetTimer(RespawnTimer, this, &AShooterCharacter::OnRespawn, RespawnTime, false);

// Clearing in EndPlay
GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
```

**Interface Implementation:**
```cpp
// Interface class implements pure virtuals
class PROJECTSERENE_API IShooterWeaponHolder
{
    GENERATED_BODY()
public:
    virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) = 0;
    // ...
};

// Implementing class
class PROJECTSERENE_API AShooterCharacter : public AProjectSereneCharacter, public IShooterWeaponHolder
{
    // Override interface methods
    virtual void AttachWeaponMeshes(AShooterWeapon* Weapon) override;
};
```

## API Specifiers

**Class Export:**
```cpp
class PROJECTSERENE_API AShooterCharacter : public AProjectSereneCharacter
```

**Blueprint Integration:**
- `BlueprintCallable` - Callable from Blueprint
- `BlueprintImplementableEvent` - Implemented in Blueprint
- `BlueprintPure` - Pure function (no side effects)
- `BlueprintReadOnly` - Readable but not writable in Blueprint

**Config:**
```cpp
UCLASS(abstract, config="Game")
class PROJECTSERENE_API AShooterPlayerController : public APlayerController
```

---

*Convention analysis: 2026-01-19*
