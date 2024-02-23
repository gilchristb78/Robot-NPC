// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobitGameMode.h"
#include "RobitCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARobitGameMode::ARobitGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
