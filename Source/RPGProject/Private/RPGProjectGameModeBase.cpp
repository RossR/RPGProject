// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGProjectGameModeBase.h"
#include "ObjectiveWorldSubsystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

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
		ObjectiveWorldSubsystem->OnMapStart();
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerHUDWidget = CreateWidget<UUserWidget>(PlayerController, PlayerHUDWidgetClass);

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectGameModeBase::ARPGProjectGameModeBase PlayerHUDWidget is null"));
	}
}