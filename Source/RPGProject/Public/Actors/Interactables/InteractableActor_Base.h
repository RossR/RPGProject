// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "Components/AudioComponent.h"
#include "InteractableActor_Base.generated.h"

UENUM(BlueprintType)
enum class EInteractionType : uint8
{
	Toggle			UMETA(DisplayName = "Toggle"),
	Button			UMETA(DisplayName = "Button"),
	Timer			UMETA(DisplayName = "Timer"),
	SingleUse		UMETA(DisplayName = "SingleUse"),
	MAX				UMETA(Hidden)
};

class UCapsuleComponent;

UCLASS()
class RPGPROJECT_API AInteractableActor_Base : public AActor, public IInteractionInterface 
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableActor_Base();

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

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor = nullptr) override;

	void StartInteraction(AActor* InteractingActor = nullptr) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableHasActivated() override;

	void InteractableHasDeactivated() override;

	bool GetIsInInteractableRange(AActor* InteractingActor) const override;

	UFUNCTION(BlueprintPure)
	EInteractableState GetInteractableState() const override { return InteractableState; };

	bool CanActorBeInteractedWith() const override { return bCanInteract; }


protected:

	// --- FUNCTIONS --- //

	UFUNCTION(BlueprintImplementableEvent)
	void BPActivateInteractable();

	UFUNCTION(BlueprintImplementableEvent)
	void BPDeactivateInteractable();

	void InteractTimer();

	// --- VARIABLES --- //

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	/*TSubclassOf<UMeshComponent>*/UMeshComponent* InteractableMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	UStaticMeshComponent* InteractableStaticMeshRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Mesh")
	USkeletalMeshComponent* InteractableSkeletalMeshRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Interaction Range")
	UCapsuleComponent* InteractionRangeVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InteractableActor - Interaction Range")
	TArray<AActor*> ActorsInInteractionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	bool bStartActive = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	bool bCanInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	bool bCanHighlight = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	bool bIsLocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	bool bActivateSelf = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	float TimerDuration = 3.f;

	// ToDo - Create ActivatableActor class
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Settings")
	TArray<AActor*> ActivatableActorsArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InteractableActor - Interactable State")
	EInteractableState InteractableState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableActor - Interaction Type")
	EInteractionType InteractionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableActor - Audio")
	UAudioComponent* AudioComponent;

	FTimerHandle InteractTimerHandle;

	// Store animation for interaction? Or store it on interacting actor?

};
