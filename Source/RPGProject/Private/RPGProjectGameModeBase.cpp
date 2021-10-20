// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGProjectGameModeBase.h"
#include "ObjectiveWorldSubsystem.h"

void ARPGProjectGameModeBase::StartPlay()
{
	Super::StartPlay();
	/*
	UObjectiveWorldSubsystem* ObjectiveWorldSubsystem = GetWorld()->GetSubsystem<UObjectiveWorldSubsystem>();
	
	if (ObjectiveWorldSubsystem)
	{
		ObjectiveWorldSubsystem->CreateObjectiveWidget(ObjectiveWidgetClass);
		ObjectiveWorldSubsystem->DisplayObjectiveWidget();
	}
	*/
}