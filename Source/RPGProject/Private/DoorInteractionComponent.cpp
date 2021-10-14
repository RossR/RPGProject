// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TriggerBox.h"
#include "Engine/World.h"

#include "DrawDebugHelpers.h"

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


// Called every frame
void UDoorInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TriggerBox && GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (PlayerPawn && TriggerBox->IsOverlappingActor(PlayerPawn))
		{
			DoorState = EDoorState::DS_Open;
		}
		else if (PlayerPawn && TriggerBox->IsOverlappingActor(PlayerPawn) == false)
		{
			DoorState = EDoorState::DS_Closed;
		}
	}

	if (DoorState == EDoorState::DS_Open)
	{
		if (CurrentRotationTime < TimeToRotate)
		{
			CurrentRotationTime += DeltaTime;
			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
			GetOwner()->SetActorRotation(CurrentRotation);
		}
	}
	else
	{
		if (CurrentRotationTime > 0)
		{
			CurrentRotationTime -= DeltaTime;
			const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
			const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
			const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
			GetOwner()->SetActorRotation(CurrentRotation);
		}
	}

	DebugDraw();
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