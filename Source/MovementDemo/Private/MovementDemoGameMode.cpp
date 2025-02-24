// Copyright Epic Games, Inc. All Rights Reserved.

#include "MovementDemoGameMode.h"
#include "MovementDemoCharacter.h"
#include "Player/PlayerStateBase.h"
#include "UObject/ConstructorHelpers.h"

AMovementDemoGameMode::AMovementDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_PlayerBase"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PlayerStateClass = APlayerStateBase::StaticClass();
}
