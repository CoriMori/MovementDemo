// Copyright Epic Games, Inc. All Rights Reserved.

#include "MovementDemoGameMode.h"
#include "MovementDemoCharacter.h"
#include "Player/PlayerStateBase.h"
#include "UI/HUDBase.h"
#include "UObject/ConstructorHelpers.h"

AMovementDemoGameMode::AMovementDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/Player/BP_PlayerBase"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//set the default hud class
	static ConstructorHelpers::FClassFinder<AHUDBase> PlayerHUDBPClass(TEXT("/Game/Blueprints/BP_HUDMain"));
	if (PlayerHUDBPClass.Class != NULL)
	{
		HUDClass = PlayerHUDBPClass.Class;
	}

	//set default player state
	PlayerStateClass = APlayerStateBase::StaticClass();
}
