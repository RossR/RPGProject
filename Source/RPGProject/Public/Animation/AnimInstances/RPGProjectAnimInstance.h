// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enums/CombatEnums.h"
#include "RPGProjectAnimInstance.generated.h"


/** Delegate called by 'SheatheMainHand'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSheatheWeaponAnimNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload, EWeaponToUse, WeaponToSheathe);

/** Delegate called by 'UnsheatheMainHand'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FUnsheatheWeaponAnimNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload, EWeaponToUse, WeaponToUnsheathe);

/** Delegate called by 'PlayAttackSFX'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayAttackSFXNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

/** Delegate called by 'FireProjectile'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFireProjectileNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

/** Delegate called by 'ReloadWeapon'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReloadWeaponNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

/** Delegate called by 'GrabAmmunition'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGrabAmmunitionNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

/** Delegate called by 'ReleaseAmmunition'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReleaseAmmunitionNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

/** Delegate called by 'ReleaseAmmunition'**/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamageStaminaNotifyDelegate, FName, NotifyName, const FBranchingPointNotifyPayload&, BranchingPointPayload);

class ARPGProjectPlayerCharacter;
class ARPGProjectPlayerController;
class AAIController;

UCLASS()
class RPGPROJECT_API URPGProjectAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:		// --- FUNCTIONS --- \\

	URPGProjectAnimInstance();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetReactToBlock(bool bActive) { bReactToBlock = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetReactToBlock() { return bReactToBlock; }

public:		// --- VARIABLES --- \\

	FSheatheWeaponAnimNotifyDelegate OnSheatheWeapon;
	FUnsheatheWeaponAnimNotifyDelegate OnUnsheatheWeapon;
	
	FPlayAttackSFXNotifyDelegate OnPlayAttackSFX;

	FFireProjectileNotifyDelegate OnFireProjectile;
	FReloadWeaponNotifyDelegate OnReloadWeapon;

	FGrabAmmunitionNotifyDelegate OnGrabAmmunition;
	FReleaseAmmunitionNotifyDelegate OnReleaseAmmunition;

	FDamageStaminaNotifyDelegate OnDamageStamina;

protected:	// --- FUNCTIONS --- \\

	void UpdateCurves();

protected:	// --- VARIABLES --- \\

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	ARPGProjectPlayerCharacter* PlayerCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	AAIController* AIPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Curve Values")
	bool bDisableAngleBodyToTarget = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Curve Values")
	bool DisableAngleHeadToTarget = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "React Booleans")
	bool bReactToBlock = false;

private:	// --- FUNCTIONS --- \\

	

private:	// --- VARIABLES --- \\



};
