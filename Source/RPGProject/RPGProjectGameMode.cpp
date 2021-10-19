// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGProjectGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "RPGProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPGProjectGameMode::ARPGProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Actors/Player/BP_PlayerCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	/*Super::StartPlay(); 
	*/

	if (ObjectiveWidget == nullptr)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ObjectiveWidget = CreateWidget<UUserWidget>(PlayerController, ObjectiveWidgetClass);
	}

	if (ObjectiveWidget)
	{
		ObjectiveWidget->AddToViewport();
	}
}