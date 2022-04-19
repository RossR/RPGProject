// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "InteractableActorBase.generated.h"

class UCapsuleComponent;

UCLASS()
class RPGPROJECT_API AInteractableActorBase : public AActor, public IInteractionInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// --- FUNCTIONS --- //

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void EnableHighlight(bool bActive, int Colour = 0) override;

	virtual void InteractionStart(AActor* InteractingActor) override;

protected:

	// --- VARIABLES --- //

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interactable Mesh")
	UStaticMeshComponent* InteractableMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Range")
	UCapsuleComponent* InteractionRangeVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Interaction Range")
	TArray<AActor*> ActorsInInteractionRange;
};
