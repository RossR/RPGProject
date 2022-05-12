// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactables/InteractableActor_Base.h"
#include "InteractableActor_Door.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API AInteractableActor_Door : public AInteractableActor_Base
{
	GENERATED_BODY()

public:

	AInteractableActor_Door();

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor) override;

	void InteractionStart(AActor* InteractingActor) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableActivated() override;

	void InteractableDeactivated() override;

	bool GetIsInInteractableRange(AActor* InteractingActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	
	// --- VARIABLES --- //

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Door")
	FRotator DesiredRotation = FRotator::ZeroRotator;

	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator FinalRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Door")
	float TimeToRotate = 1.0f;

	float CurrentRotationTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Door")
	FRuntimeFloatCurve OpenCurve;

};
