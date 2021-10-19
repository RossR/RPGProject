// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"

void ATutorialGameMode::StartPlay()
{

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

