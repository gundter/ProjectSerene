// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Horror/HorrorGameMode.h"
#include "Player/SerenePlayerState.h"

AHorrorGameMode::AHorrorGameMode()
{
	// Use SerenePlayerState to own the Ability System Component and AttributeSet
	// This allows GAS attributes to persist across respawns and level transitions
	PlayerStateClass = ASerenePlayerState::StaticClass();
}
