// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "MyGame.h"
#include "MyGameGameMode.h"
#include "MyGameCharacter.h"

AMyGameGameMode::AMyGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//TotalNumberOfTargets = 3;
}

