// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "InteractableDoor.generated.h"

class UDoorInteractionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDoorOpen);

UCLASS()
class RPGPROJECT_API AInteractableDoor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AInteractableDoor();

	virtual void BeginPlay() override;

public:
	float InteractionTime = 5.0f;

	UPROPERTY(BlueprintAssignable, Category = "Door Interaction")
	FOnDoorOpen OnDoorOpen;

protected:
	UFUNCTION()
	void OnInteractionSuccess();

protected:
	UPROPERTY(EditAnywhere, NoClear)
		UDoorInteractionComponent* DoorInteractionComponent;
};