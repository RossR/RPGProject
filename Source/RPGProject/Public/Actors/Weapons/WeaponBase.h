// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundWave.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;
class UAnimSequence;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None				UMETA(DisplayName="None"),
	WT_Unarmed			UMETA(DisplayName = "Unarmed"),
	WT_Item				UMETA(DisplayName = "Item"),
	WT_1HandDagger		UMETA(DisplayName = "One-handed Dagger"),
	WT_1HandMace		UMETA(DisplayName = "One-handed Mace"),
	WT_1HandPistol		UMETA(DisplayName = "One-handed Pistol"),
	WT_1HandSpear		UMETA(DisplayName = "One-handed Spear"),
	WT_1HandSword		UMETA(DisplayName = "One-handed Sword"),
	WT_2HandAxe			UMETA(DisplayName = "Two-handed Axe"),
	WT_2HandBow			UMETA(DisplayName = "Two-handed Bow"),
	WT_2HandCrossbow	UMETA(DisplayName = "Two-handed Crossbow"),
	WT_2HandShooter		UMETA(DisplayName = "Two-handed Shooter"),
	WT_2HandSpear		UMETA(DisplayName = "Two-handed Spear"),
	WT_2HandStaff		UMETA(DisplayName = "Two-handed Staff"),
	WT_2HandSword		UMETA(DisplayName = "Two-handed Sword"),
	WT_1HandShield		UMETA(DisplayName = "One-handed Shield"),

	WT_Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName WeaponName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	uint8 LightAttackComboLimit;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	uint8 HeavyAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UAnimSequence* CombatIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	TArray<USoundWave*> RandomAttackSoundArray;

};

UCLASS()
class RPGPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UFUNCTION(BlueprintCallable)
	FWeaponInfo GetWeaponInfo() { return WeaponInfo; }

	UFUNCTION(BlueprintCallable)
	void PlayRandomAttackSound();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void WeaponTrace();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void TraceBetweenTraces();

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
	void SetIsFirstTrace(bool bActive) { bIsFirstTrace = bActive; }

	UFUNCTION(BlueprintCallable)
	void SetLastTraceVariables(int Index);

public:	


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FWeaponInfo WeaponInfo
	{
		// Weapon Name
		"WeaponBase",
		// Weapon Type
		EWeaponType::WT_None, 
		// Attack Montage
		nullptr,
		// Light Attack Combo Limit
		4,
		// Heavy Attack Combo Limit
		2,
		// Combat Idle Animation
		nullptr,
		// Weapon Weight
		0.0f, 
		// Weapon Condition
		100.0f,
		// Weapon Damage
		50.0f,
		// Attack Sounds Array
		//{ nullptr },
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName SheathSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName EquippedSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<USceneComponent*> WeaponTraceStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<USceneComponent*> WeaponTraceEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Tracing")
	int TraceIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	bool bWeaponTraceHasHit = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<FHitResult> WeaponTraceHitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	int TotalTraceSections = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	float TraceAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	bool bIsFirstTrace = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FVector CurrentTraceStart = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FVector CurrentTraceEnd = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<FVector> LastTraceStart;//= FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<FVector> LastTraceEnd;// = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FVector CurrentTraceSection = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FVector LastTraceSection = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FVector TraceBoxHalfSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	float TraceRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	float TraceHalfHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	FRotator TraceOrientation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	TArray<AActor*> IgnoredActorArray;

};
