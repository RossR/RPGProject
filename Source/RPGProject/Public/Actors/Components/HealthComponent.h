// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

USTRUCT(BlueprintType)
struct FBodyPartInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	TArray<FName> BoneArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	// 0 = Don't modify BodyPartMaxHP
	// Will change the value of BodyPartMaxHP if value is larger than 0
	float MaxHPPercentage = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	float BodyPartMaxHP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	float BodyPartCurrentHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	float DamageModifier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	bool bIsVitalBodyPart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Body Part Info")
	bool bCanBreak = false;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Damage, FName InBodyPartName = "All", bool bUseDamageModifier = true); //{ CurrentHealthPoints -= Damage; }
	UFUNCTION(BlueprintCallable)
	void HealDamage(float HealAmount, FName InBodyPartName = "All"); //{ CurrentHealthPoints += HealAmount; }

	UFUNCTION(BlueprintCallable)
	bool IsDead() { return CurrentHealthPoints <= FLT_EPSILON; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealthPoints() { return MaxHealthPoints; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealthPoints() { return CurrentHealthPoints; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentHealthPoints(float NewHealth) { CurrentHealthPoints = NewHealth; }
	UFUNCTION(BlueprintCallable)
	void SetMaxHealthPoints(float NewMaxHealth) { MaxHealthPoints = NewMaxHealth; }

	UFUNCTION(BlueprintPure)
	TMap<FName, FBodyPartInfo> GetBodyPartMap();

	UFUNCTION(BlueprintCallable)
	bool AddToBodyPartMap(FName BoneName, FBodyPartInfo BodyPartInfo);

	UFUNCTION(BlueprintCallable)
	bool RemoveFromBodyPartMap(FName BoneName);

	UFUNCTION(BlueprintCallable)
	FName GetBodyPartFromBone(FName BoneName);

protected:

	void BodyPartUpdate();

	void CurrentHealthPointsUpdate();

protected:

	UPROPERTY(EditAnywhere, Category = "Health Variables")
	float MaxHealthPoints = 1000;

	UPROPERTY(VisibleAnywhere, Category = "Health Variables")
	float CurrentHealthPoints = 1000.f;
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hitboxes")
	// FName is the name of the body part, FBodyPartInfo stores information about that body part
	TMap<FName, FBodyPartInfo> BodyPartMap;

};
