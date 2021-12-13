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

	/*UFUNCTION(BlueprintCallable)
	FName GetWeaponName() { return WeaponInfo.WeaponName; }

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() { return WeaponInfo.WeaponType; }

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetAttackMontage() { return WeaponInfo.AttackMontage; }

	UFUNCTION(BlueprintCallable)
	uint8 GetLightAttackComboLimit() { return WeaponInfo.LightAttackComboLimit; }

	UFUNCTION(BlueprintCallable)
	uint8 GetHeavyAttackComboLimit() { return WeaponInfo.HeavyAttackComboLimit; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponWeight() { return WeaponInfo.WeaponWeight; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponCondition() { return WeaponInfo.WeaponCondition; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponDamage() { return WeaponInfo.WeaponDamage; }*/

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing", Meta = (MakeEditWidget = true))
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	USceneComponent* WeaponTraceStart;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon Line Tracing")
	USceneComponent* WeaponTraceEnd;

	


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
};
