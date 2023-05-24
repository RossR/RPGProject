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

public:		

	// Sets default values for this component's properties
	UProjectileSpawnerComponent();

protected:	

	// Called when the game starts
	virtual void BeginPlay() override;

public:		

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:		// --- FUNCTIONS --- \\

	UFUNCTION(BlueprintPure, Category = "Projectile Spawner")
	USceneComponent* GetProjectileSpawnTransform() const { return ProjectileSpawnTransform; }

	/*
	* Spawns the specified projectile actor
	* @param ProjectileInUse The class of the projectile actor being spawned
	* @param bAimAtCrosshair If true, the projectile will be spawned facing towards the centre of the player's viewport (if the projectile is being fired from a weapon)
	* @param ProjectileStrengthScale Determines the speed and accuracy of the fired projectile
	*/
	UFUNCTION(BlueprintCallable)
	bool SpawnProjectile(TSubclassOf<AProjectileActor> ProjectileInUse, bool bAimAtCrosshair = false, float ProjectileStrengthScale = 1.f);

public:		// --- VARIABLES --- \\



protected:	// --- FUNCTIONS --- \\



protected:	// --- VARIABLES --- \\

	FActorSpawnParameters ProjectileSpawnParameters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Spawner")
	FRotator FiringAngleVariance = FRotator::ZeroRotator;

	//------------------
	// References
	//------------------

	UPROPERTY(BlueprintReadOnly, Category = "Projectile Spawner|References")
	AItemWeaponRanged* RangedWeaponRef;

	//------------------
	// Spawn Transform
	//------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Spawner|Spawn Transform")
	UArrowComponent* ProjectileSpawnArrow;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Spawner|Spawn Transform")
	USceneComponent* ProjectileSpawnTransform;

private:	// --- FUNCTIONS --- \\
	


private:	// --- VARIABLES --- \\



};
