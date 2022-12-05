// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileSpawnerComponent.generated.h"

class USceneComponent;
class UArrowComponent;
class AItemWeaponRanged;
class AProjectileActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UProjectileSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:		// --- FUNCTIONS --- \\

	// Sets default values for this component's properties
	UProjectileSpawnerComponent();

	USceneComponent* GetProjectileSpawnTransform() { return ProjectileSpawnTransform; }

	void SetProjectileFiringAngle(FRotator NewAngle) { ProjectileFiringAngle = NewAngle; }

protected:	// --- FUNCTIONS --- \\

	// Called when the game starts
	virtual void BeginPlay() override;

public:		// --- FUNCTIONS --- \\

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:		// --- FUNCTIONS --- \\

	UFUNCTION(BlueprintCallable)
	bool SpawnProjectile(TSubclassOf<AProjectileActor> ProjectileInUse, bool bAimAtCrosshair = false, float ProjectileStrength = 1.f);


protected:	// --- FUNCTIONS --- \\



private:	// --- FUNCTIONS --- \\
	


protected:	// --- VARIABLES --- \\

	AItemWeaponRanged* RangedWeaponRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Transform")
	USceneComponent* ProjectileSpawnTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Transform")
	UArrowComponent* ProjectileSpawnArrow;

	FActorSpawnParameters ProjectileSpawnParameters;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Projectiles")
	//TArray<AProjectileActor*> SpawnedProjectileArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workarounds")
	FRotator ProjectileFiringAngle = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Workarounds")
	FRotator FiringAngleVariance = FRotator::ZeroRotator;

private:	// --- VARIABLES --- \\



};
