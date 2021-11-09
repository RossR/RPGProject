// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Subsystems/WorldSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveComponent.h"
#include "ObjectiveWorldSubsystem.generated.h"

class UObjectiveHud;

UCLASS()
class RPGPROJECT_API UObjectiveWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable)
	void DisplayObjectivesCompleteWidget();

	UFUNCTION(BlueprintCallable)
	FString GetCurrentObjectiveDescription();

	UFUNCTION(BlueprintCallable)
	void RemoveObjectivesCompleteWidget();

	UFUNCTION(BlueprintCallable)
	void AddObjective(UObjectiveComponent* ObjectiveComponent);

	UFUNCTION(BlueprintCallable)
	void RemoveObjective(UObjectiveComponent* ObjectiveComponent);

	// We call this from maps that we want to display objectives (e.g Main menu will not call this function)
	UFUNCTION(BlueprintCallable)
	void OnMapStart();

protected:

	virtual void Deinitialize() override;

	void CreateObjectiveWidgets();

	void DisplayObjectiveWidget();
	void RemoveObjectiveWidget();

	uint32 GetCompletedObjectiveCount();

	void OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState);

private:
	UObjectiveHud*	ObjectiveWidget = nullptr;
	UUserWidget*	ObjectivesCompleteWidget = nullptr;


	// Add remove them
	// Sign up for callback onchanged
	UPROPERTY(VisibleAnywhere)
	TArray<UObjectiveComponent*> Objectives;
};
