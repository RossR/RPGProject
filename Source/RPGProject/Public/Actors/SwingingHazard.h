// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingingHazard.generated.h"

class UStaticMeshComponent;
class UCableComponent;
class UPhysicsConstraintComponent;
class UCapsuleComponent;

UCLASS()
class RPGPROJECT_API ASwingingHazard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASwingingHazard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void StartSwingDelayFinished();

	void SwingClockwise();
	void SwingAnticlockwise();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HangingPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SwingingHazard;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCableComponent* Cable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* PhysicsConstraint;

	UPROPERTY(EditAnywhere, NoClear)
	UCapsuleComponent* TriggerCapsule;

	UPROPERTY(EditAnywhere)
	bool SwingingAnticlockwiseFirst;

	UPROPERTY(EditAnywhere)
	float StartSwingDelay;

	UPROPERTY(EditAnywhere, Category = "Swing Settings")
	FVector ImpulseVector;

	UPROPERTY(EditAnywhere, Category = "Swing Settings")
	float DirectionChangeTime;

	UPROPERTY(EditAnywhere, Category = "Swing Settings")
	float DirectionChangeAngle;

protected:

	UPROPERTY(VisibleAnywhere, Category = "Swing Settings")
	float TimeCounter;
	
	float DeltaSeconds;

	FTimerHandle StartSwingDelayTimerHandle;
	FTimerHandle SwingClockwiseTimerHandle;
	FTimerHandle SwingAnticlockwiseTimerHandle;

};
