// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGProjectGameMode.h"
#include "RPGProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPGProjectGameMode::ARPGProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
