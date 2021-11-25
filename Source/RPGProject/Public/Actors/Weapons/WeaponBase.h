// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None UMETA(DisplayName="None"),
	WT_Unarmed UMETA(DisplayName="Unarmed"),
	WT_Sword UMETA(DisplayName="Sword"),

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
	float WeaponWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponDamage;

};

UCLASS()
class RPGPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FName GetWeaponName() { return WeaponInfo.WeaponName; }

	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() { return WeaponInfo.WeaponType; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponWeight() { return WeaponInfo.WeaponWeight; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponCondition() { return WeaponInfo.WeaponCondition; }

	UFUNCTION(BlueprintCallable)
	float GetWeaponDamage() { return WeaponInfo.WeaponDamage; }


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FWeaponInfo WeaponInfo
	{
		"WeaponBase", 
		EWeaponType::WT_Unarmed, 
		0.0f, 
		100.0f,
		50.0f,
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UStaticMeshComponent* WeaponMesh;
};
