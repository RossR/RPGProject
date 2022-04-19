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

UENUM(BlueprintType)
enum class ETraceType : uint8
{
	Multi_Line		UMETA(DisplayName = "Multi Line"),
	Multi_Box		UMETA(DisplayName = "Multi Box"),
	Multi_Sphere	UMETA(DisplayName = "Multi Sphere"),
	Unique_Trace	UMETA(DisplayName = "Unique Trace"),

	MAX				UMETA(Hidden)
};


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

	UFUNCTION()
	void WeaponTrace(ETraceType InTraceType, int SingleWeaponTraceStart = -1);

	UFUNCTION(BlueprintImplementableEvent)
	void UniqueWeaponTrace();

	UFUNCTION()
	void TraceBetweenTraces(int TraceIndex);

	UFUNCTION(BlueprintCallable)
	TArray<FHitResult> GetWeaponTraceHitResults() { return WeaponTraceHitResult; }

	UFUNCTION(BlueprintCallable)
	void ClearWeaponTraceHitResults() { WeaponTraceHitResult.Empty(); }

	UFUNCTION(BlueprintCallable)
	bool GetWeaponTraceHasHit() { return bWeaponTraceHasHit; }

	UFUNCTION(BlueprintCallable)
	void SetIsFirstTrace(bool bActive) { bIsFirstTrace = bActive; }

	UFUNCTION(BlueprintPure)
	ETraceType GetTraceType() const { return TraceType; }

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
	void CheckIfFirstTrace(int TraceIndex = 0);

	UFUNCTION(BlueprintCallable)
	void SetLastTraceVariables(int Index = 0);

public:	


protected:

	// --- VARIABLES --- //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	UItemWeaponData* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Mesh")
	UStaticMeshComponent* WeaponMesh;

	// - WEAPON TRACING - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	ETraceType TraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<USceneComponent*> WeaponTraceStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<USceneComponent*> WeaponTraceEnd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Tracing")
	bool bTraceComplex = false;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	bool bWeaponTraceHasHit = false;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FHitResult> WeaponTraceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	int TotalTraceSections = 10;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	float TraceAlpha;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	bool bIsFirstTrace = true;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceStart = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceEnd = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FVector> LastTraceStart;//= FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<FVector> LastTraceEnd;// = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	FVector CurrentTraceSection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Tracing")
	FVector LastTraceSection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	TArray<AActor*> IgnoredActorArray;

	// - WEAPON TRACING - BOX - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing - Box")
	FVector TraceBoxHalfSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing - Box")
	FRotator TraceOrientation = FRotator::ZeroRotator;

	// - WEAPON TRACING - SPHERE - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Tracing - Sphere")
	float TraceRadius;
	
	
private:

	

};
