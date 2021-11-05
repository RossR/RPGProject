// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "InteractionComponent.h"
#include "DoorInteractionComponent.generated.h"

class ATriggerBox;
class IConsoleVariable;

UENUM()
enum class EDoorState
{
	DS_Closed = 0 UMETA(DisplayName = "Closed"),
	DS_Opening = 1 UMETA(DisplayName = "Opening"),
	DS_Open = 2 UMETA(DisplayName = "Open"),
	DS_Closing = 3 UMETA(DisplayName = "Closing"),
	DS_Locked = 4 UMETA(DisplayName = "Locked"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UDoorInteractionComponent : public UInteractionComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDoorInteractionComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	static void OnDebugToggled(IConsoleVariable* Var);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Bound to interaction input from player
	void InteractionStart();

	// Request to open the door
	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	//  Called internally when the door has finished opening
	void OnDoorOpen();

	UFUNCTION(BlueprintCallable)
	bool IsOpen() { return DoorState == EDoorState::DS_Open; }

	void DebugDraw();

public:	
	DECLARE_EVENT(FDoorInteractionComponent, FOpened)
	FOpened& OnOpened() { return OpenedEvent; }

	FOpened OpenedEvent;

protected:

	UPROPERTY(EditAnywhere)
	FRotator DesiredRotation = FRotator::ZeroRotator;

	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere)
	float TimeToRotate = 1.0f;

	float CurrentRotationTime = 0.0f;

	UPROPERTY(EditAnywhere)
	ATriggerBox* TriggerBox;

	UPROPERTY(EditAnywhere)
	FRuntimeFloatCurve OpenCurve;

	UPROPERTY(BlueprintReadOnly)
	EDoorState DoorState;

};
