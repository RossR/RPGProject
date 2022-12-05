// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/HitFXInterface.h"
#include "ProjectileActor.generated.h"

class UShapeComponent;
class USphereComponent;
class UAudioComponent;
class USoundAttenuation;
//class UProjectileMovementComponent;

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	PT_Default	UMETA(Hidden, DisplayName = "Default"),
	PT_Arrow	UMETA(DisplayName = "Arrow"),
	PT_Bullet	UMETA(DisplayName = "Bullet"),

	PT_MAX		UMETA(Hidden)
};


UCLASS()
class RPGPROJECT_API AProjectileActor : public AActor, public IHitFXInterface
{
	GENERATED_BODY()
	
public:		// --- FUNCTIONS --- \\
	// Sets default values for this actor's properties
	AProjectileActor();

protected:	// --- FUNCTIONS --- \\
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		// --- FUNCTIONS --- \\
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:		// --- FUNCTIONS --- \\

	virtual UHitFXData* GetHitFXData() override { return ProjectileHitFXData; }

	UFUNCTION()
	void OnProjectileCollisionVolumeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnProjectileMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintPure)
	USphereComponent* GetProjectileCollisionVolume() { return ProjectileCollisionVolume; }

	UFUNCTION(BlueprintPure)
	UStaticMeshComponent* GetProjectileMesh() { return ProjectileMesh; }

	UFUNCTION(BlueprintCallable)
	void SetProjectileOwner(AActor* NewProjectileOwner) { ProjectileOwner = NewProjectileOwner; }

	UFUNCTION(BlueprintPure)
	AActor* GetProjectileOwner() { return ProjectileOwner; }

	UFUNCTION(BlueprintCallable)
	void SetProjectileDamage(float NewValue) { ProjectileDamage = NewValue; }

	UFUNCTION(BlueprintPure)
	float GetProjectileDamage() { return ProjectileDamage; }

	UFUNCTION(BlueprintCallable)
	void SetProjectileImpactImpulse(float NewValue) { ProjectileImpactImpulse = NewValue; }

	UFUNCTION(BlueprintPure)
	float GetProjectileImpactImpulse() { return ProjectileImpactImpulse; }

	UFUNCTION(BlueprintPure)
	float GetProjectileStrengthMultiplier() { return ProjectileStrengthMultiplier; }

	UFUNCTION(BlueprintCallable)
	void SetProjectileStrengthMultiplier(float NewValue) { ProjectileStrengthMultiplier = NewValue; }

	UFUNCTION(BlueprintPure)
	UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

	UFUNCTION(BlueprintCallable)
	void SetEnableProjectile(bool bActive) { bEnableProjectile = bActive; }

	UFUNCTION(BlueprintPure)
	bool GetEnableProjectile() { return bEnableProjectile; }

	UFUNCTION(BlueprintPure)
	FName GetProjectileStartSocket() { return ProjectileStartSocket; }

	UFUNCTION(BlueprintPure)
	FName GetProjectileEndSocket() { return ProjectileEndSocket; };

	UFUNCTION(BlueprintPure)
	FName GetProjectileImpalePointSmallSocket() { return ProjectileImpalePointSmallSocket; }

	UFUNCTION(BlueprintPure)
	FName GetProjectileImpalePointMediumSocket() { return ProjectileImpalePointMediumSocket; }

	UFUNCTION(BlueprintPure)
	FName GetProjectileImpalePointLargeSocket() { return ProjectileImpalePointLargeSocket; }

	UFUNCTION(BlueprintCallable)
	void EnableProjectilePhysics(bool bActive);

	UFUNCTION(BlueprintCallable)
	void PlayProjectileSFX();

protected:	// --- FUNCTIONS --- \\

	void SpinProjectile();

	bool CheckIfCloseToObject();

private:	// --- FUNCTIONS --- \\

	void PlayProjectileHitFX(FHitResult InHitResult);

public:		// --- VARIABLES --- \\



protected:	// --- VARIABLES --- \\

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bEnableProjectile = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Collision")
	USphereComponent* ProjectileCollisionVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Mesh")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Movement")
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Projectile VFX")
	UNiagaraComponent* ProjectileTrailVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Projectile VFX")
	UNiagaraComponent* ProjectileSweetSpotVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Projectile Audio")
	UAudioComponent* ProjectileAudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Damage")
	float ProjectileDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Damage")
	bool bIgnoreMassForImpulse = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Damage")
	float ProjectileImpactImpulse = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Damage")
	float ProjectileStrengthMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Lifespam")
	float ProjectileLifeSpawn = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projecile Socket Names")
	FName ProjectileStartSocket = "ProjectileStart";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projecile Socket Names")
	FName ProjectileEndSocket = "ProjectileEnd";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projecile Socket Names")
	FName ProjectileImpalePointSmallSocket = "ImpalePoint_Small";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projecile Socket Names")
	FName ProjectileImpalePointMediumSocket = "ImpalePoint_Medium";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projecile Socket Names")
	FName ProjectileImpalePointLargeSocket = "ImpalePoint_Large";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Deflection Multiplier")
	float ProjectileVelocityDeflectionMultiplier = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Type")
	EProjectileType ProjectileType = EProjectileType::PT_Default;

	FTimerHandle SpinProjectileTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Spin Speed")
	float ProjectileSpinSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile Ignored Actor Array")
	TArray<AActor*> IgnoredActorArray;

private:	// --- VARIABLES --- \\

	// Actor that created the projectile
	AActor* ProjectileOwner;

	UPROPERTY(EditAnywhere, Category = "Projecile HitFXData")
	UHitFXData* ProjectileHitFXData;
};
