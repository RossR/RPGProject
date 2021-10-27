// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGProjectGameModeBase.h"
#include "ObjectiveWorldSubsystem.h"
#include "UObject/ConstructorHelpers.h"

ARPGProjectGameModeBase::ARPGProjectGameModeBase()
{
	/* Bad practice to use ConstructorHelpers as it hard-codes ObjectiveWidgetClass
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_Objective(TEXT("/Game/Widgets/WBP_Objective"));
	if (!ensure(WBP_Objective.Class != nullptr)) return;
	

	ObjectiveWidgetClass = WBP_Objective.Class;
	*/
}

void ARPGProjectGameModeBase::StartPlay()
{
	Super::StartPlay();

	

	UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	
	if (ObjectiveWorldSubsystem)
	{
		ObjectiveWorldSubsystem->CreateObjectiveWidget(ObjectiveWidgetClass);
		ObjectiveWorldSubsystem->DisplayObjectiveWidget();
	}
}