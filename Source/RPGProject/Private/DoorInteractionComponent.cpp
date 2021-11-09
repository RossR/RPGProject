// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
// #include "ObjectiveWorldSubsystem.h"
#include "DrawDebugHelpers.h"
#include "ObjectiveComponent.h"
#include "InteractionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"


constexpr float FLT_METERS(float meters) { return meters * 100.0f; }

static TAutoConsoleVariable<bool> CVarToggleDebugDoor(
	TEXT("RPGProject.DoorInteractionComponent.Debug"),
	false,
	TEXT("Toggle DoorInterationComponent debug display."),
	ECVF_Default);

// Sets default values for this component's properties
UDoorInteractionComponent::UDoorInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	DoorState = EDoorState::DS_Closed;

	CVarToggleDebugDoor.AsVariable()->SetOnChangedCallback(FConsoleVariableDelegate::CreateStatic(&UDoorInteractionComponent::OnDebugToggled));

	// ...
}


// Called when the game starts
void UDoorInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	StartRotation = GetOwner()->GetActorRotation();
	FinalRotation = GetOwner()->GetActorRotation() + DesiredRotation;
	
	// Ensure TimeToRotate is greater than EPSILON
	CurrentRotationTime = 0.0f;

}

void UDoorInteractionComponent::InteractionStart()
{
	Super::InteractionStart();

	UE_LOG(LogTemp, Warning, TEXT("UDoorInterationComponent::InteractionStart called"));
	
	if (InteractingActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UDoorInterationComponent::InteractionStart OpenDoor called"));
		OpenDoor();
		
	}
}

void UDoorInteractionComponent::InteractionCancel()
{
	Super::InteractionCancel();

	UE_LOG(LogTemp, Warning, TEXT("UDoorInterationComponent::InteractionCancel called"));

	CloseDoor();
}

// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DoorState == EDoorState::DS_Closed)
	{
		/*
		if (TriggerCapsule && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
		{
			APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			if (PlayerPawn && TriggerCapsule->IsOverlappingActor(PlayerPawn))
			{
				DoorState = EDoorState::DS_Opening;
			}
			else if (PlayerPawn && TriggerCapsule->IsOverlappingActor(PlayerPawn) == false)
			{
				DoorState = EDoorState::DS_Closed;
			}
		}
		*/
	}
	else if (DoorState == EDoorState::DS_Opening)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		GetOwner()->SetActorRotation(CurrentRotation);
		if (TimeRatio >= 1.0f)
		{
			OnDoorOpen();
		}
		/*
		if (TriggerCapsule && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
		{
			APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			if (PlayerPawn && TriggerCapsule->IsOverlappingActor(PlayerPawn))
			{
				DoorState = EDoorState::DS_Opening;


			}
		*/
	}
	else if (DoorState == EDoorState::DS_Open)
	{
		if (TriggerCapsule && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
		{
			APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
			if (PlayerPawn && TriggerCapsule->IsOverlappingActor(PlayerPawn) == false)
			{
				DoorState = EDoorState::DS_Closing;
			}
		}
	}
	else if (DoorState == EDoorState::DS_Closing)
	{
		CurrentRotationTime -= DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		GetOwner()->SetActorRotation(CurrentRotation);
		if (TimeRatio <= 0.0f)
		{
			OnDoorClose();
		}	
	}

	
	DebugDraw();
}

void UDoorInteractionComponent::OpenDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("ADoorInteractionComponent::OpenDoor called"));

	if (IsOpen() || DoorState == EDoorState::DS_Opening)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ADoorInteractionComponent::OpenDoor door is opening"));

	DoorState = EDoorState::DS_Opening;
	// CurrentRotationTime = 0.0f;
}

void UDoorInteractionComponent::CloseDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("ADoorInteractionComponent::CloseDoor called"));

	if (DoorState == EDoorState::DS_Closed || DoorState == EDoorState::DS_Closing)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("ADoorInteractionComponent::CloseDoor door is closing"));

	DoorState = EDoorState::DS_Closing;
	// CurrentRotationTime = 1.0f;
}

void UDoorInteractionComponent::OnDoorOpen()
{
	DoorState = EDoorState::DS_Open;
	
	UObjectiveComponent* ObjectiveComponent = GetOwner()->FindComponentByClass<UObjectiveComponent>();
	if (ObjectiveComponent)
	{
		ObjectiveComponent->SetState(EObjectiveState::OS_Completed);
	}

	// Tell any listeners that the interaction is successful
	InteractionSuccess.Broadcast();
}

void UDoorInteractionComponent::OnDoorClose()
{
	DoorState = EDoorState::DS_Closed;
}

void UDoorInteractionComponent::OnDebugToggled(IConsoleVariable* Var)
{
	UE_LOG(LogTemp, Warning, TEXT("OnDebugLogged"));
}

void UDoorInteractionComponent::DebugDraw()
{
	if (CVarToggleDebugDoor->GetBool())
	{
		FVector Offset(FLT_METERS(-0.75f), 0.0f, FLT_METERS(2.5f));
		FVector StartLocation = GetOwner()->GetActorLocation() + Offset;
		FString EnumAsString = TEXT("Door State: ") + UEnum::GetDisplayValueAsText(DoorState).ToString();
		DrawDebugString(GetWorld(), Offset, EnumAsString, GetOwner(), FColor::Blue, 0.0f);
	}
}