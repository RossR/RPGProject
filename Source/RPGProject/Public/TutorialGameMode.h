// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "TutorialGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API ATutorialGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	void StartPlay() override;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UUserWidget> ObjectiveWidgetClass;

	UUserWidget* ObjectiveWidget = nullptr;

};