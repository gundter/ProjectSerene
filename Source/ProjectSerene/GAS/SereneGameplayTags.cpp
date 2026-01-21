// Copyright Project Serene. All Rights Reserved.

#include "SereneGameplayTags.h"

namespace SereneGameplayTags
{
	// ----------------------------------------
	// Attribute Threshold State Tags
	// ----------------------------------------

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowHealth, "State.LowHealth", "Health below 30% of MaxHealth - triggers low health effects");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowStamina, "State.LowStamina", "Stamina below 20% of MaxStamina - sprint may be limited");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowSanity, "State.LowSanity", "Sanity below critical threshold (30%) - hallucinations intensify");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_LowBattery, "State.LowBattery", "Battery below 20% of MaxBattery - flashlight flicker warning imminent");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Dead, "State.Dead", "Health reached zero - player is dead");

	// ----------------------------------------
	// Sanity Threshold Tags (Multiple Levels)
	// ----------------------------------------

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sanity50, "State.Sanity50", "Sanity below 50% - perception effects begin (visual distortion starts)");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sanity30, "State.Sanity30", "Sanity below 30% - hallucinations intensify (same threshold as LowSanity)");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sanity20, "State.Sanity20", "Sanity below 20% - critical level (audio muffling, severe effects)");

	// ----------------------------------------
	// Action State Tags
	// ----------------------------------------

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Sprinting, "State.Sprinting", "Character is currently sprinting - stamina drain active");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_Recovering, "State.Recovering", "Stamina is recovering after depletion - must reach 20% before sprinting allowed");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(State_FlashlightOn, "State.FlashlightOn", "Flashlight is currently on - battery drain active");
}
