// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "DealDamageComponent.generated.h"

class UCapsuleComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UDealDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDealDamageComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UCapsuleComponent* GetTriggerCapsule() const { return TriggerCapsule; }

	bool IsActive() const { return bActive; }
	UFUNCTION(BlueprintCallable)
	void SetActiveState(bool IsActive) { bActive = IsActive; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	//Tracks wether the component is active or not
	bool bActive = true;

	UPROPERTY(EditAnywhere)
	// Total damage dealt to the overlapped actors
	float BaseDamage = 50.0f;

	UPROPERTY(EditAnywhere)
	// Intervals (in seconds) to apply damage to overlapped actors
	float TakeDamageInterval;

	UPROPERTY(EditAnywhere)
	// The duration that overlapped actors will take damage
	float DamageTotalTime = 2.0f;

	UPROPERTY(EditAnywhere, NoClear)
	UCapsuleComponent* TriggerCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<AActor*> OverlappedActorArray;

	// UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	// TArray<UPrimitiveComponent*> OverlappedComponentArray;

	ARPGProjectPlayerCharacter* PlayerCharacter;
	TSubclassOf<UDamageType> ValidDamageTypeClass;

	UPrimitiveComponent* OtherComponent;

	FTimerHandle DamageOverTime;
	
	bool bWasActiveLastTick;

};
