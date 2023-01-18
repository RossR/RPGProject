// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "RPGProjectGameModeBase.generated.h"


UCLASS()
class RPGPROJECT_API ARPGProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ARPGProjectGameModeBase();

	void StartPlay() override;


public:		// --- FUNCTIONS --- \\



public:		// --- VARIABLES--- \\



protected:	// --- FUNCTIONS --- \\



protected:	// --- VARIABLES --- \\



private:	// --- FUNCTIONS --- \\



private:	// --- VARIABLES --- \\



};