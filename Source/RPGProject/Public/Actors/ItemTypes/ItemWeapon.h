// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/ItemEquipment.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundWave.h"
#include "ItemWeapon.generated.h"

class UStaticMeshComponent;
class UAnimSequence;


UCLASS()
class RPGPROJECT_API AItemWeapon : public AItemEquipment
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemWeapon();

	// UFUNCTION(BlueprintCallable)
	virtual UItemData* GetItemData() override { return ItemData; }

	UFUNCTION(BlueprintCallable)
	void PlayRandomAttackSound();

	UFUNCTION(BlueprintImplementableEvent)
	void WeaponTrace();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TraceBetweenTraces();

	UFUNCTION(BlueprintCallable)
	TArray<FHitResult> GetWeaponTraceHitResults() { return WeaponTraceHitResult; }

	UFUNCTION(BlueprintCallable)
	void ClearWeaponTraceHitResults() { WeaponTraceHitResult.Empty(); }

	UFUNCTION(BlueprintCallable)
	bool GetWeaponTraceHasHit() { return bWeaponTraceHasHit; }

	UFUNCTION(BlueprintCallable)
	void SetIsFirstTrace(bool bActive) { bIsFirstTrace = bActive; }

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceStart(USceneComponent* _WeaponTraceStart);

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceEnd(USceneComponent* _WeaponTraceEnd);

	UFUNCTION(BlueprintCallable)
	void SetTraceAlpha(int CurrentTraceSectionIndex);

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceSection();

	UFUNCTION(BlueprintCallable)
	void SetLastTraceSection(int Index);

	UFUNCTION(BlueprintCallable)
	void SetTraceOrientation(USceneComponent* _WeaponTraceStart);

	UFUNCTION(BlueprintCallable)
	void SetTraceResults(TArray<FHitResult> OutHits,bool TraceReturnValue);

	UFUNCTION(BlueprintCallable)
	void CheckIfFirstTrace();

	UFUNCTION(BlueprintCallable)
	void SetLastTraceVariables(int Index);

public:	


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	UItemWeaponData* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<USceneComponent*> WeaponTraceStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<USceneComponent*> WeaponTraceEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	int TraceIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	bool bWeaponTraceHasHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FHitResult> WeaponTraceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	int TotalTraceSections = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	float TraceAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	bool bIsFirstTrace = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceStart = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceEnd = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FVector> LastTraceStart;//= FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FVector> LastTraceEnd;// = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceSection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FVector LastTraceSection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FVector TraceBoxHalfSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	float TraceHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	FRotator TraceOrientation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<AActor*> IgnoredActorArray;
	
private:

	

};
