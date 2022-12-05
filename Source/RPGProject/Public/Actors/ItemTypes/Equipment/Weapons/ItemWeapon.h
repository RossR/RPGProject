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
	TMap<uint8, FWeaponAttackInfo> GetLightAttackInfo() const { return LightAttackInfo; }

	UFUNCTION(BlueprintPure)
	FWeaponAttackInfo GetLightAttackInfoAtIndex(uint8 Index) { return LightAttackInfo.Find(Index) ? LightAttackInfo[Index] : FWeaponAttackInfo(); }

	UFUNCTION(BlueprintPure)
	TMap<uint8, FWeaponAttackInfo> GetHeavyAttackInfo() const { return HeavyAttackInfo; }

	UFUNCTION(BlueprintPure)
	FWeaponAttackInfo GetHeavyAttackInfoAtIndex(uint8 Index) { return HeavyAttackInfo.Find(Index) ? HeavyAttackInfo[Index] : FWeaponAttackInfo(); }

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Mesh")
	UStaticMeshComponent* WeaponMesh;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attack Blocking Volume")
	TSubclassOf<UShapeComponent> AttackBlockerVolumeClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Attack Blocking Volume")
	UCapsuleComponent* AttackBlockerVolume = nullptr;*/

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Weapon | Weapon Attack Blocking Volume")
	EAttackBlockerType AttackBlockerType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Weapon Attack Blocking Volume")
	UBoxComponent* AttackBlockerBoxVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Weapon Attack Blocking Volume")
	UCapsuleComponent* AttackBlockerCapsuleVolume;


	// - VFX - //

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item | Weapon | VFX", DisplayName = "Auto Set Melee Trail VFX Width")
	bool bAutoSetMeleeTrailVFXWidth = false;

	// - WEAPON TRACING - //

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Tracing")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace = EDrawDebugTrace::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	EAttackType CurrentAttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TMap<uint8, FWeaponAttackInfo> LightAttackInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TMap<uint8, FWeaponAttackInfo> HeavyAttackInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	ETraceType CurrentTraceType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Trace Start Vectors")
	TMap<int, USceneComponent*> WeaponTraceStartMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Trace End Vectors")
	TMap<int, USceneComponent*> WeaponTraceEndMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Item | Weapon | VFX")
	TMap<int, UNiagaraComponent*> WeaponVFXMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray = { EObjectTypeQuery::ObjectTypeQuery4, EObjectTypeQuery::ObjectTypeQuery6, EObjectTypeQuery::ObjectTypeQuery11 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	bool bTraceComplex = false;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	bool bWeaponTraceHasHit = false;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TArray<FHitResult> WeaponTraceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	int BPTraceIndex = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	int TotalTraceSections = 10;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	float TraceAlpha;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	bool bIsFirstTrace = true;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	FVector CurrentTraceStart = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	FVector CurrentTraceEnd = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TArray<FVector> LastTraceStart;//= FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TArray<FVector> LastTraceEnd;// = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	FVector CurrentTraceSection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	FVector LastTraceSection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing")
	TArray<AActor*> IgnoredActorArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Tracing")
	uint8 AttackComboCount;

	// - WEAPON TRACING - BOX - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing | Box")
	FVector TraceBoxHalfSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing | Box")
	FRotator TraceOrientation = FRotator::ZeroRotator;

	// - WEAPON TRACING - SPHERE - //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Tracing | Sphere")
	float TraceRadius;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponData* WeaponData;*/

private:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponData* WeaponData;
	

};
