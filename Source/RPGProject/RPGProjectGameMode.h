// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "RPGProjectGameMode.generated.h"

UCLASS(minimalapi)
class ARPGProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ARPGProjectGameMode();
	//void StartPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> ObjectiveWidgetClass;
	
	UUserWidget* ObjectiveWidget = nullptr;
};



