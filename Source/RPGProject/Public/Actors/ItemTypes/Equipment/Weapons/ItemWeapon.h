// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/Equipment/ItemEquipment.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/Components/CombatComponent.h"
#include "Enums/CombatEnums.h"
#include "Structs/CombatStructs.h"
#include "ItemWeapon.generated.h"

class UStaticMeshComponent;
class UAnimSequence;
class UNiagaraComponent;
class UShapeComponent;
class UBoxComponent;
class UCapsuleComponent;







UCLASS()
class RPGPROJECT_API AItemWeapon : public AItemEquipment
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemWeapon();

	virtual void PostInitProperties() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetItemData(UItemData* NewItemData) override;

	// UFUNCTION(BlueprintCallable)
	virtual UItemData* GetItemData() override { return WeaponData; }

	UFUNCTION(BlueprintPure)
	UItemWeaponData* GetItemWeaponData() { return WeaponData; }

	virtual void ItemPickupEvent() override { OnItemPickup.ExecuteIfBound(WeaponData); }

	UFUNCTION(BlueprintCallable)
	void PlayAttackSFX();

	UFUNCTION()
	void WeaponTrace(int SingleWeaponTraceStart = -1);

	UFUNCTION(BlueprintImplementableEvent)
	void UniqueWeaponTrace();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UniqueTraceBetweenTraces(int TraceIndex);

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
	TMap<uint8, FWeaponAttackInfo> GetLightAttackInfo() const { if (!WeaponData->LightAttackInfo.IsEmpty()) { return WeaponData->LightAttackInfo; } else { return TMap<uint8, FWeaponAttackInfo>(); } }

	UFUNCTION(BlueprintPure)
	FWeaponAttackInfo GetLightAttackInfoAtIndex(uint8 Index) { return  WeaponData->LightAttackInfo.Find(Index) ? WeaponData->LightAttackInfo[Index] : FWeaponAttackInfo(); }

	UFUNCTION(BlueprintPure)
	TMap<uint8, FWeaponAttackInfo> GetHeavyAttackInfo() const { if (!WeaponData->HeavyAttackInfo.IsEmpty()) { return WeaponData->HeavyAttackInfo; } else { return TMap<uint8, FWeaponAttackInfo>(); } }

	UFUNCTION(BlueprintPure)
	FWeaponAttackInfo GetHeavyAttackInfoAtIndex(uint8 Index) { return WeaponData->HeavyAttackInfo.Find(Index) ? WeaponData->HeavyAttackInfo[Index] : FWeaponAttackInfo(); }

	UFUNCTION(BlueprintCallable)
	void EnableWeaponVFX(int Index = -1);

	UFUNCTION(BlueprintCallable)
	void DisableWeaponVFX(int Index = -1);

	UFUNCTION(BlueprintCallable)
	void SetAttackComboCount(uint8 NewAttackComboCount) { AttackComboCount = AttackComboCount; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentAttackType(EAttackType NewAttackType) { CurrentAttackType = NewAttackType; }

	UFUNCTION(BlueprintPure)
	UShapeComponent* GetAttackBlockerVolume();

	UFUNCTION(BlueprintCallable)
	void SetAttackBlockerCollisionEnabled(ECollisionEnabled::Type NewType);

	UFUNCTION(BlueprintCallable)
	void AttackBlocked(FWeaponAttackInfo& BlockedAttackInfo, AItemWeapon* InItemWeapon = nullptr, EAttackType BlockedAttackType = EAttackType::AT_None, AProjectileActor* InProjectileActor = nullptr);

protected:

	virtual void UpdateCurves() override;

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceStart(USceneComponent* _WeaponTraceStart) { CurrentTraceStart = _WeaponTraceStart->GetComponentLocation(); }

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceEnd(USceneComponent* _WeaponTraceEnd) { CurrentTraceEnd = _WeaponTraceEnd->GetComponentLocation(); }

	UFUNCTION(BlueprintCallable)
	void SetTraceAlpha(int CurrentTraceSectionIndex) { TraceAlpha = (float)CurrentTraceSectionIndex / (float)TotalTraceSections; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentTraceSection() { CurrentTraceSection = (CurrentTraceStart * TraceAlpha) + (CurrentTraceEnd * (1.0f - TraceAlpha)); }

	UFUNCTION(BlueprintCallable)
	void SetLastTraceSection(int Index) { LastTraceSection = (LastTraceStart[Index] * TraceAlpha) + (LastTraceEnd[Index] * (1 - TraceAlpha)); }

	UFUNCTION(BlueprintCallable)
	void SetTraceOrientation(USceneComponent* _WeaponTraceStart) { TraceOrientation = _WeaponTraceStart->GetRelativeRotation() + ItemMesh->GetComponentRotation(); }

	UFUNCTION(BlueprintCallable)
	void AddToWeaponTraceHitResults(TArray<FHitResult> OutHits);

	UFUNCTION(BlueprintCallable)
	void CheckIfFirstTrace(int TraceIndex = 0) { if (!bIsFirstTrace) { TraceBetweenTraces(TraceIndex); } }

	UFUNCTION(BlueprintCallable)
	void SetLastTraceVariables(int LastTraceStartArraySize, int LastTraceEndArraySize, int Index = 0);

	UFUNCTION(BlueprintCallable)
	void SetMeleeTrailVFXWidth();

public:	



protected:

	// --- VARIABLES --- //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Mesh")
	UStaticMeshComponent* WeaponMesh;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attack Blocking Volume")
	TSubclassOf<UShapeComponent> AttackBlockerVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attack Blocking Volume")
	UCapsuleComponent* AttackBlockerVolume = nullptr;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Actor | Weapon Attack Blocking Volume")
	EAttackBlockerType AttackBlockerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Weapon Attack Blocking Volume")
	UBoxComponent* AttackBlockerBoxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Weapon Attack Blocking Volume")
	UCapsuleComponent* AttackBlockerCapsuleVolume;


	// - VFX - //

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Actor | VFX", DisplayName = "Auto Set Melee Trail VFX Width")
	bool bAutoSetMeleeTrailVFXWidth = false;

	// - WEAPON TRACING - //

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Actor | Tracing")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace = EDrawDebugTrace::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	EAttackType CurrentAttackType;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TMap<uint8, FWeaponAttackInfo> LightAttackInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TMap<uint8, FWeaponAttackInfo> HeavyAttackInfo;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	ETraceType CurrentTraceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Trace Start Vectors")
	TMap<int, USceneComponent*> WeaponTraceStartMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Trace End Vectors")
	TMap<int, USceneComponent*> WeaponTraceEndMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Weapon Actor | VFX")
	TMap<int, UNiagaraComponent*> WeaponVFXMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray = { EObjectTypeQuery::ObjectTypeQuery4, EObjectTypeQuery::ObjectTypeQuery6, EObjectTypeQuery::ObjectTypeQuery11 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	bool bTraceComplex = false;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	bool bWeaponTraceHasHit = false;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TArray<FHitResult> WeaponTraceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	int BPTraceIndex = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	int TotalTraceSections = 10;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	float TraceAlpha;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	bool bIsFirstTrace = true;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	FVector CurrentTraceStart = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	FVector CurrentTraceEnd = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TArray<FVector> LastTraceStart;//= FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TArray<FVector> LastTraceEnd;// = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	FVector CurrentTraceSection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	FVector LastTraceSection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing")
	TArray<AActor*> IgnoredActorArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Actor | Tracing")
	uint8 AttackComboCount;

	// - WEAPON TRACING - BOX - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing | Box")
	FVector TraceBoxHalfSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing | Box")
	FRotator TraceOrientation = FRotator::ZeroRotator;

	// - WEAPON TRACING - SPHERE - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor | Tracing | Sphere")
	float TraceRadius;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponData* WeaponData;*/

private:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponData* WeaponData;
	

};
