// Copyright Project Serene. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 * SereneGameplayTags
 *
 * Native gameplay tags for Project Serene.
 * These tags are defined in C++ for compile-time safety and performance.
 *
 * Tag hierarchy:
 * - State.*: Player state tags (applied based on attribute thresholds or actions)
 *   - State.LowHealth: Health below 30%
 *   - State.LowStamina: Stamina below 20%
 *   - State.LowSanity: Sanity below critical threshold (30%)
 *   - State.LowBattery: Battery below 20%
 *   - State.Dead: Health reached zero
 *   - State.Sprinting: Character is currently sprinting
 *   - State.Recovering: Stamina recovering after depletion (must reach 20% to sprint again)
 *   - State.Sanity50: Sanity below 50% (perception effects begin)
 *   - State.Sanity30: Sanity below 30% (explicit threshold, same as LowSanity)
 *   - State.Sanity20: Sanity below 20% (critical level, audio muffling)
 */
namespace SereneGameplayTags
{
	// ----------------------------------------
	// Attribute Threshold State Tags
	// Applied/removed automatically by AttributeSet based on attribute values
	// ----------------------------------------

	/** Health below 30% of MaxHealth */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowHealth);

	/** Stamina below 20% of MaxStamina */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowStamina);

	/** Sanity below critical threshold (30% of MaxSanity) - legacy tag for compatibility */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowSanity);

	/** Battery below 20% of MaxBattery */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_LowBattery);

	/** Health reached zero - player is dead */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);

	// ----------------------------------------
	// Sanity Threshold Tags (Multiple Levels)
	// Used by perception and audio systems
	// ----------------------------------------

	/** Sanity below 50% - perception effects begin (visual distortion starts) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sanity50);

	/** Sanity below 30% - same threshold as LowSanity (hallucinations intensify) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sanity30);

	/** Sanity below 20% - critical level (audio muffling, severe effects) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sanity20);

	// ----------------------------------------
	// Action State Tags
	// Applied/removed by gameplay systems
	// ----------------------------------------

	/** Character is currently sprinting (consuming stamina) */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Sprinting);

	/** Stamina is recovering after depletion - must reach 20% before sprinting allowed */
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Recovering);
}
