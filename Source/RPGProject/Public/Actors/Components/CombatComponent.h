// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "DataAssets/ItemData.h"
#include "PlayerCameraManagers/RPGPlayerCameraManager.h"
#include "Enums/CombatEnums.h"
#include "Structs/CombatStructs.h"
#include "CombatComponent.generated.h"

class ACharacter;
class URPGProjectAnimInstance;
class ARPGProjectPlayerController;
class ARPGPlayerCameraManager;
class UEquipmentComponent;
class UHealthComponent;
class UStaminaComponent;
class AItemWeapon;
class AProjectileActor;
class UHitFXData;
class UNiagaraSystem;
class USoundAttenuation;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	// --- FUNCTIONS --- //

	UFUNCTION(BlueprintCallable)
	// Set the combat state
	void SetCombatState(ECombatState NewState) { CombatState = NewState; }

	UFUNCTION(BlueprintPure)
	// Get the combat state
	const ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable)
	void SetAttackType(EAttackType NewAttackType) { CurrentAttackType = NewAttackType; }
	UFUNCTION(BlueprintPure)
	const EAttackType GetAttackType() { return CurrentAttackType; }

	UFUNCTION(BlueprintPure)
	bool GetIsInAttackSequence() { return bIsInAttackSequence; }

	UFUNCTION(BlueprintPure)
	float GetAttackCount() { return AttackCount; }

	UFUNCTION(BlueprintCallable)
	void CharacterAttack(EAttackType CharacterAttackType);

	UFUNCTION(BlueprintCallable)
	void StartAttackSequence(EAttackType SequenceAttackType);

	UFUNCTION(BlueprintCallable)
	void StartNeutralCombatAction();
	UFUNCTION(BlueprintCallable)
	void StopNeutralCombatAction();

	UFUNCTION(BlueprintCallable)
	void StartStanceCombatAction();
	UFUNCTION(BlueprintCallable)
	void StopStanceCombatAction();

	UFUNCTION(BlueprintCallable)
	void ReloadWeapon(EWeaponToUse WeaponToReload);
	
	UFUNCTION(BlueprintCallable)
	void ToggleCombatState();

	UFUNCTION(BlueprintCallable)
	bool UnsheatheWeapon();
	
	UFUNCTION(BlueprintCallable)
	bool SheatheWeapon();

	UFUNCTION(BlueprintCallable)
	bool CombatDodge();

	UFUNCTION(BlueprintCallable)
	bool StaminaExhausted(EStaminaExhaustionType StaminaExhaustionType = EStaminaExhaustionType::Default);

	UFUNCTION(BlueprintCallable)
	void SwapWeaponLoadout();

	UFUNCTION(BlueprintCallable)
	void SetCanAttack(bool bActive) { bCanAttack = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetCanAttack() { return bCanAttack; }

	UFUNCTION(BlueprintPure)
	float GetRotationSpeedReductionScaleCurve() { return RotationSpeedReductionScaleCurve; }

	UFUNCTION(BlueprintPure)
	float GetQueueAttackWindowCurve() { return QueueAttackWindowCurve; }

	UFUNCTION(BlueprintPure)
	float GetEnableHitCurve() { return EnableHitCurve; }

	UFUNCTION(BlueprintPure)
	bool GetIsInAttackRecovery() { return bIsInActionRecovery; };

	UFUNCTION(BlueprintCallable)
	void SetCombatWeaponStance(ECombatWeaponStance NewWeaponStance) { CombatWeaponStance = NewWeaponStance; }
	UFUNCTION(BlueprintPure)
	const ECombatWeaponStance GetCombatWeaponStance() { return CombatWeaponStance; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentWeaponStanceType(EWeaponStanceType NewWeaponStanceType) { CurrentWeaponStanceType = NewWeaponStanceType; }
	UFUNCTION(BlueprintPure)
	EWeaponStanceType GetCurrentWeaponStanceType() { return CurrentWeaponStanceType; }

	UFUNCTION(BlueprintCallable)
	void SetIsDodging(bool bActive) { bIsDodging = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetIsDodging() { return bIsDodging; }
	
	UFUNCTION(BlueprintCallable)
	void SetIsGuarding(bool bActive) { bIsGuarding = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetIsGuarding() { return bIsGuarding; }

	UFUNCTION(BlueprintCallable)
	void SetAimAtCrosshair(bool bActive) { bAimAtCrosshair = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetAimAtCrosshair() { return bAimAtCrosshair; }
	
	UFUNCTION(BlueprintCallable)
	void EvaluateHitResult(FHitResult InHitResult, AItemWeapon* InItemWeapon = nullptr, AProjectileActor* InProjectileActor = nullptr);

	UFUNCTION(BlueprintCallable)
	bool IsAttackFromBlockedAngle(AActor* AttackingActor);

	UFUNCTION(BlueprintCallable)
	void AttackBlocked(FWeaponAttackInfo& BlockedAttackInfo, AItemWeapon* InItemWeapon = nullptr, EAttackType BlockedAttackType = EAttackType::AT_None, AProjectileActor* InProjectileActor = nullptr);

	UFUNCTION(BlueprintCallable)
	void SetCombatActionIsPressed(bool bActive) { bCombatActionIsPressed = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetCombatActionIsPressed() { return bCombatActionIsPressed; }

	UFUNCTION(BlueprintCallable)
	FWeaponAttackInfo GetCurrentWeaponAttackInfo(AItemWeapon* AttackingWeapon);

	UFUNCTION(BlueprintCallable)
	void PlayHitFX(FHitResult InHitResult, AActor* AttackingActor = nullptr, bool bOverrideHitSurface = false, TEnumAsByte<EPhysicalSurface> HitSurfaceOverride = EPhysicalSurface::SurfaceType_Default);

	// --- VARIABLES --- // 

protected:

	// --- FUNCTIONS --- //

	UFUNCTION()
	void OnSheathingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnSheathingMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnSheatheWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload, EWeaponToUse WeaponToSheathe);

	UFUNCTION()
	void SheatheMainhand();
	UFUNCTION()
	void SheatheOffhand();

	UFUNCTION()
	void OnUnsheatheMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnUnsheatheMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnUnsheatheWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload, EWeaponToUse WeaponToUnsheathe);

	UFUNCTION()
	void UnsheatheMainhand();
	UFUNCTION()
	void UnsheatheOffhand();

	UFUNCTION()
	void OnPlayAttackSFXNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnReloadWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);


	UFUNCTION()
	void OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnAttackNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnCombatActionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnCombatActionEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnReloadMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDodgeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnStaminaExhaustedMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnStaminaExhaustedMontageEnded(UAnimMontage* Montage, bool bInterrupted);


	UFUNCTION()
	void UnbindSheatheMontage();
	UFUNCTION()
	void UnbindUnsheatheMontage();
	UFUNCTION()
	void UnbindAttackMontage();
	UFUNCTION()
	void UnbindCombatActionMontage();
	UFUNCTION()
	void UnbindReloadMontage();
	UFUNCTION()
	void UnbindDodgeMontage();
	UFUNCTION()
	void UnbindStaminaExhaustedMontage();

	UFUNCTION(BlueprintCallable)
	bool DoesAttackNeedAmmunition(EWeaponToUse AttackingWeapon, EAttackType InAttackType);

	// --- VARIABLES --- // 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* SheatheMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* UnsheatheMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* WeaponAttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* WeaponReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Component | Animations")
	UAnimMontage* StaminaExhaustedMontage;

	FOnMontageBlendingOutStarted WeaponAttackBlendingOutDelegate;
	FOnMontageEnded WeaponAttackMontageEndedDelegate;

	FOnMontageBlendingOutStarted CombatActionBlendingOutDelegate;
	FOnMontageEnded CombatActionMontageEndedDelegate;

	FOnMontageBlendingOutStarted UnSheatheBlendingOutDelegate;
	FOnMontageEnded UnSheatheMontageEndedDelegate;

	FOnMontageBlendingOutStarted SheatheBlendingOutDelegate;
	FOnMontageEnded SheatheMontageEndedDelegate;

	FOnMontageBlendingOutStarted ReloadMontageBlendingOutDelegate;
	FOnMontageEnded ReloadMontageEndedDelegate;

	FOnMontageBlendingOutStarted DodgeMontageBlendingOutDelegate;
	FOnMontageEnded DodgeMontageEndedDelegate;

	FOnMontageBlendingOutStarted  StaminaExhaustedMontageBlendingOutDelegate;
	FOnMontageEnded  StaminaExhaustedMontageEndedDelegate;

	// Store values of the curves used in the attack montages
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float RotationSpeedReductionScaleCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float QueueAttackWindowCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float EnableHitCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float EnableWeaponVFXCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float EnableActionRecoveryCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float EnableDodgeCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat Component | Curves")
	float EnableGuardCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat Component | Hit FX" , meta = (DisplayName = "Default HitFX Data"))
	UHitFXData* DefaultHitFXData;

private:

	// --- FUNCTIONS --- //

	void UpdateCurveFloats();
	void UpdateCombatBooleans();
	void UpdateGuardState();

	void SetupNextAttack(); // Formerly EndAttackWindow
	void EndAttackSequence();
	void AttackTracing();
	UItemWeaponData* GetEquippedWeaponData();

	void StaminaExhaustionUpdate();

	// --- VARIABLES --- // 


	// Reference to owning character
	ACharacter* CharacterRef;

	ARPGProjectPlayerCharacter* RPGProjectCharacterRef;

	ARPGProjectPlayerController* PC;

	ARPGPlayerCameraManager* RPGPlayerCameraManagerRef;

	// Reference to the owning character's equipment component (if one exists)
	UEquipmentComponent* EquipmentComponentRef;

	UHealthComponent* HealthComponentRef;

	UStaminaComponent* StaminaComponentRef;

	// Reference to the owning character's mesh
	USkeletalMeshComponent* MeshComponentRef;

	UPROPERTY(VisibleAnywhere, Category = "Equipment References")
	AItemWeapon* MainhandWeaponRef;

	UPROPERTY(VisibleAnywhere, Category = "Equipment References")
	AItemWeapon* OffhandWeaponRef;

	// Reference to the owning character's animinstance
	URPGProjectAnimInstance* CharacterAnimInstance;

	// Tracks how many attacks the character has done in the current attack sequence
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Attack Count")
	uint8 AttackCount;

	// Stores what attack type the current attack is
	EAttackType CurrentAttackType;

	EAttackType NextAttackType;

	// Stores what attack type the current finisher is
	EAttackType FinisherType;

	// Stores what type of dodge the character can currently perform
	EDodgeType DodgeType;

	// Stores current combat state
	ECombatState CombatState;

	// Stores current combat weapon stance
	UPROPERTY(EditAnywhere, Category = "Combat Component | Weapon Stance")
	ECombatWeaponStance CombatWeaponStance;

	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Weapon Stance")
	EWeaponStanceType CurrentWeaponStanceType = EWeaponStanceType::ST_None;

	// Stores all actors hit when attacking
	TArray<TWeakObjectPtr<class AActor>> HitActorArray;

	// Stores name of the current montage section being played in the attack sequence
	FName CurrentSectionInMontage;

	// Stores name of the next montage section to play in the attack sequence
	FName NextSectionInMontage;

	//// Stores montage section for sheathing the currently equipped weapon
	//FName EquippedWeaponSheatheSection;
	//// Stores montage section for unsheathing the currently equipped weapon
	//FName EquippedWeaponUnsheatheSection;

	// Booleans used in swapping between weapon loadouts
	bool bMainHandSheathed = true;
	bool bOffHandSheathed = true;
	bool bSwapWeaponLoadout = false;

	// Combat booleans

	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bCanAttack = true;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsInCombat = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsAttackQueued = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsFinisherQueued = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsInAttackSequence = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsInAttackWindow = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsInAttackWindUp = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans")
	bool bIsInActionRecovery = false;

	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans | Actions")
	bool bAimAtCrosshair = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans | Actions")
	bool bIsDodging = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans | Actions")
	bool bIsGuarding = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans | Actions")
	bool bCombatActionIsPressed = false;
	UPROPERTY(VisibleAnywhere, Category = "Combat Component | Combat Booleans | Actions")
	bool bCombatActionNeedsToReload = false;

	EWeaponToUse BlockingWeapon = EWeaponToUse::None;
	float CurrentWeaponBlockAngle = 0.f;

};
