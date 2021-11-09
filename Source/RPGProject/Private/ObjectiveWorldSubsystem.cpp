// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectiveWorldSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "RPGProjectGameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "ObjectiveHud.h"

void UObjectiveWorldSubsystem::Deinitialize()
{
	ObjectiveWidget = nullptr;
	ObjectivesCompleteWidget = nullptr;
}

void UObjectiveWorldSubsystem::CreateObjectiveWidgets()
{
	if (ObjectiveWidget == nullptr)
	{
		ARPGProjectGameModeBase* GameMode = Cast<ARPGProjectGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			ObjectiveWidget = CreateWidget<UObjectiveHud>(PlayerController, GameMode->ObjectiveWidgetClass);
			ObjectivesCompleteWidget = CreateWidget<UUserWidget>(PlayerController, GameMode->ObjectivesCompleteWidgetClass);
		}
	}
}

void UObjectiveWorldSubsystem::DisplayObjectiveWidget()
{
	
	if (ObjectiveWidget)
	{
		if (!ObjectiveWidget->IsInViewport())
		{
			ObjectiveWidget->AddToViewport();
		}
		ObjectiveWidget->UpdateObjectiveText(GetCompletedObjectiveCount(), Objectives.Num());
	}
	else
	{
		ensureMsgf(ObjectiveWidget, TEXT("UObjectiveWorldSubsystem::DisplayObjectiveWidget ObjectiveWidget is nullptr"));
		UE_LOG(LogTemp, Warning, TEXT("ObjectiveWidget is nullptr"));
	}
}

void UObjectiveWorldSubsystem::RemoveObjectiveWidget()
{
	if (ObjectiveWidget)
	{
		ObjectiveWidget->RemoveFromViewport();
	}
}

void UObjectiveWorldSubsystem::DisplayObjectivesCompleteWidget()
{
	if (ObjectivesCompleteWidget)
	{
		ObjectivesCompleteWidget->AddToViewport();
	}
}

FString UObjectiveWorldSubsystem::GetCurrentObjectiveDescription()
{
	if (!Objectives.IsValidIndex(0) || Objectives[0]->GetState() == EObjectiveState::OS_Inactive)
	{
		return TEXT("N/A");
	}

	FString RetObjective = Objectives[0]->GetDescription();
	if (Objectives[0]->GetState() == EObjectiveState::OS_Completed)
	{

		RetObjective += TEXT(" Completed!");
	}

	return RetObjective;
}

void UObjectiveWorldSubsystem::RemoveObjectivesCompleteWidget()
{
	if (ObjectivesCompleteWidget)
	{
		ObjectivesCompleteWidget->RemoveFromViewport();
	}
}

void UObjectiveWorldSubsystem::AddObjective(UObjectiveComponent* ObjectiveComponent)
{
	check(ObjectiveComponent);

	size_t PrevSize = Objectives.Num();
	Objectives.AddUnique(ObjectiveComponent);
	if (Objectives.Num() > PrevSize)
	{
		ObjectiveComponent->OnStateChanged().AddUObject(this, &UObjectiveWorldSubsystem::OnObjectiveStateChanged);
	}
}

void UObjectiveWorldSubsystem::RemoveObjective(UObjectiveComponent* ObjectiveComponent)
{
	int32 numRemoved = ObjectiveComponent->OnStateChanged().RemoveAll(this);
	check(numRemoved);
	Objectives.Remove(ObjectiveComponent);
}

uint32 UObjectiveWorldSubsystem::GetCompletedObjectiveCount()
{
	uint32 ObjectivesCompleted = 0u;
	
	for (const UObjectiveComponent* OC : Objectives)
	{
		if (OC && OC->GetState() == EObjectiveState::OS_Completed)
		{
			++ObjectivesCompleted;
		}
	}

	return ObjectivesCompleted;
}

void UObjectiveWorldSubsystem::OnMapStart()
{
	ARPGProjectGameModeBase* GameMode = Cast<ARPGProjectGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode)
	{
		CreateObjectiveWidgets();
		DisplayObjectiveWidget();
	}

}

void UObjectiveWorldSubsystem::OnObjectiveStateChanged(UObjectiveComponent* ObjectiveComponent, EObjectiveState ObjectiveState)
{
	// Check if the game is over
	if (ObjectiveWidget && ObjectivesCompleteWidget)
	{
		if (GetCompletedObjectiveCount() == Objectives.Num())
		{
			// Game Over
			RemoveObjectiveWidget();
			DisplayObjectivesCompleteWidget();
		}
		else
		{
			DisplayObjectiveWidget();
		}
	}
}