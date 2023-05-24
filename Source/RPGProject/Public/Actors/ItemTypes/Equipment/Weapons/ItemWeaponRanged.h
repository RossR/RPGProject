// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "ItemWeaponRanged.generated.h"

class URPGProjectAnimInstance;
class UProjectileSpawnerComponent;
class UCableComponent;

UCLASS()
class RPGPROJECT_API AItemWeaponRanged : public AItemWeapon
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	AItemWeaponRanged();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	

public:		// --- FUNCTIONS --- \\

	virtual void SetItemData(UItemData* NewItemData) override;

	virtual UItemData* GetItemData() override { return WeaponRangedData; }

	UFUNCTION(BlueprintPure)
	UItemWeaponRangedData* GetItemWeaponRangedData () { return WeaponRangedData; }

	virtual UItemWeaponData* GetItemWeaponData() override { return Cast<UItemWeaponData>(WeaponRangedData); }

	virtual void ItemPickupEvent() override { OnItemPickup.ExecuteIfBound(WeaponRangedData); }

	UFUNCTION(BlueprintCallable)
	void FireProjectile(bool bAimAtCrosshair = false);

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon(EReloadType InReloadType);

	UFUNCTION(BlueprintCallable)
	void UnloadWeapon();

	UFUNCTION(BlueprintPure)
	bool GetCanFire() { return bCanFire; }
	UFUNCTION(BlueprintCallable)
	void SetCanFire(bool bActive = true) { bCanFire = bActive; }

	UFUNCTION(BlueprintPure)
	AProjectileActor* GetProjectileChildActor() { return Cast<AProjectileActor>(ProjectileChildActorComponent->GetChildActor()); }

	void AttachAmmunitionToHand(USkeletalMeshComponent* MeshComp);
	void AttachAmmunitionToWeapon();

	UFUNCTION(BlueprintCallable)
	void SetProjectileStrengthScaleCurve(float NewValue) { ProjectileStrengthScaleCurve = NewValue; }

	UFUNCTION(BlueprintCallable)
	void SetEnableDrawSweetSpotCurve(bool bActive) { bEnableDrawSweetSpotCurve = bActive; }


protected:	// --- FUNCTIONS --- \\

	virtual void UpdateCurves() override;

	void FireProjectileCooldown();
	
private:	// --- FUNCTIONS --- \\
	
	void UpdateProjectileRotation();
	void UpdateBowstringPosition();

protected:	// --- VARIABLES --- \\

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	USceneComponent* BowstringOriginPosition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	USceneComponent* BowstringPullPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	UCableComponent* BowstringBottom;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	UCableComponent* BowstringTop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	float BowstringInterpSpeed = 1122.077026f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Bowstring")
	float BowstringResetTime = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Projectile")
	USceneComponent* ProjectileAimPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Projectile")
	UChildActorComponent* ProjectileChildActorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Projectile")
	AProjectileActor* ProjectileChildActorRef;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Projectile")
	UProjectileSpawnerComponent* ProjectileSpawnerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Ranged | Projectile")
	float SweetSpotDamageMultiplier = 1.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Ranged | General")
	bool bCanFire = true;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Weapon | Ranged | General")
	FTimerHandle FireProjectileCooldownTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Ranged | Curves")
	float EnableFireCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Ranged | Curves")
	float HoldBowstringCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Ranged | Curves")
	float ProjectileStrengthScaleCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Weapon | Ranged | Curves")
	bool bEnableDrawSweetSpotCurve = false;

	

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponRangedData* WeaponRangedData;*/

private:	// --- VARIABLES --- \\

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemWeaponRangedData* WeaponRangedData;

};
