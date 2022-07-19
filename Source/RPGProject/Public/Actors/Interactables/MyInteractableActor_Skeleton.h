// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactables/InteractableActor_Base.h"
#include "MyInteractableActor_Skeleton.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API AMyInteractableActor_Skeleton : public AInteractableActor_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMyInteractableActor_Skeleton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor = nullptr) override;

	void InteractionStart(AActor* InteractingActor = nullptr) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableActivated() override;

	void InteractableDeactivated() override;

	bool GetIsInInteractableRange(AActor* InteractingActor) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	USkeletalMeshComponent* InteractableSkeletalMesh;
};
