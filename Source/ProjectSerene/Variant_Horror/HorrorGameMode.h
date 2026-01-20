// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HorrorGameMode.generated.h"

/**
 *  GameMode for the first person horror game variant
 *
 *  Configures the game to use SerenePlayerState for GAS attribute management.
 *  The PlayerState owns the AbilitySystemComponent and AttributeSet, which
 *  allows attributes to persist across respawns and level transitions.
 */
UCLASS(abstract)
class PROJECTSERENE_API AHorrorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	/** Constructor - sets up default classes including PlayerState */
	AHorrorGameMode();
};
