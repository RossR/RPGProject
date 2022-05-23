// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactables/InteractableActor_Base.h"
#include "InteractableActor_Lever.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API AInteractableActor_Lever : public AInteractableActor_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableActor_Lever();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// --- FUNCTIONS --- //

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor = nullptr) override;

	void InteractionStart(AActor* InteractingActor = nullptr) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableActivated() override;

	void InteractableDeactivated() override;

protected:

	// --- VARIABLES --- //

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	UStaticMeshComponent* LeverHandleMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Lever")
	FRotator InactiveRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Lever")
	FRotator ActiveRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Lever")
	float TimeToRotate = 1.0f;

	float CurrentRotationTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Lever")
	FRuntimeFloatCurve LeverRotationCurve;

};
