// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactables/InteractableActor_Base.h"
#include "InteractableActor_Button.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API AInteractableActor_Button : public AInteractableActor_Base
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractableActor_Button();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	// --- FUNCTIONS --- //

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor = nullptr) override;

	void StartInteraction(AActor* InteractingActor = nullptr) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableHasActivated() override;

	void InteractableHasDeactivated() override;
	
protected:

	// --- VARIABLES --- //

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	UStaticMeshComponent* ButtonMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (MakeEditWidget = true), Category = "InteractableActor - Button")
	FVector StartingVector = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (MakeEditWidget = true), Category = "InteractableActor - Button")
	FVector PressedVector = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Button")
	float TranslationTime = 1.0f;

	float CurrentTranslationTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "InteractableActor - Button")
	FRuntimeFloatCurve ButtonPressCurve;

};
