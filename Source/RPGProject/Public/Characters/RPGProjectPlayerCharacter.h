// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Actors/ItemTypes/ItemWeapon.h"
#include "Actors/Components/CombatComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "RPGProjectPlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerVerticalMobility : uint8
{
	PVM_Standing	UMETA(DisplayName = "Standing"),
	PVM_Crouching	UMETA(DisplayName = "Crouching"),
	PVM_Crawling	UMETA(DisplayName = "Crawling"),
	PVM_Jumping		UMETA(DisplayName = "Jumping"),
	PVM_Falling		UMETA(DisplayName = "Falling"),

	/*
	PMS_Idle UMETA(DisplayName = "Idle"),
	PMS_Walking UMETA(DisplayName = "Walking"),
	PMS_Jogging UMETA(DisplayName = "Jogging"),
	PMS_Sprinting UMETA(DisplayName = "Sprinting"),
	PMS_Crouching UMETA(DisplayName = "Crouching"),
	*/

	PVM_Max			UMETA(Hidden)
};
	
UENUM(BlueprintType)
enum class EPlayerHorizontalMobility : uint8
{
	PHM_Idle		UMETA(DisplayName = "Idle"),
	PHM_Walking		UMETA(DisplayName = "Walking"),
	PHM_Jogging		UMETA(DisplayName = "Jogging"),
	PHM_Sprinting	UMETA(DisplayName = "Sprinting"),

	PHM_Max			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
	PAS_Idle		UMETA(DisplayName = "Idle"),
	PAS_Dodging		UMETA(DisplayName = "Dodging"),
	PAS_Interacting UMETA(DisplayName = "Interacting"),
	PAS_Guarding	UMETA(DisplayName = "Guarding"),
	PAS_Aiming		UMETA(DisplayName = "Aiming"),
	PAS_Casting		UMETA(DisplayName = "Casting"),

	PAS_MAX			UMETA(Hidden)
};

class ARPGProjectPlayerController;

class UCameraShakeBase;
class UParticleSystemComponent;
class UChildActorComponent;

class UCharacterStatisticComponent;
class UHealthComponent;
class UStaminaComponent;
class UDamageHandlerComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UCombatComponent;

UCLASS(BlueprintType)
class RPGPROJECT_API ARPGProjectPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGProjectPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the actor falls out the world (KillZ)
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void TakeStaminaDamage(float Damage);

	void SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval);

	UFUNCTION(BlueprintCallable)
	void HandleItemCollected();

	UFUNCTION(BlueprintImplementableEvent)
	void ItemCollected();

	UFUNCTION(BlueprintImplementableEvent)
	void DoorOpenInteractionStarted(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable)
	void MoveCameraToArrowLocation(FName ArrowName);


	UFUNCTION(BlueprintCallable)
	const bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	const float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	const float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable)
	bool IsStaminaFull();

	UFUNCTION(BlueprintCallable)
	bool IsCharacterExhausted() { return bIsExhausted; }

	UFUNCTION(BlueprintCallable)
	bool IsCharacterFalling() { return bIsFalling; }

	UFUNCTION(BlueprintCallable)
	void SetIsCrouched(bool IsActive) { bIsCrouched = IsActive; }

	UFUNCTION(BlueprintCallable)
	// Returns true if the character is crouching
	bool GetIsCrouched() { return bIsCrouched; }

	UFUNCTION(BlueprintCallable)
	void SetCapsuleHeight(float NewCapsuleHeight);

	UFUNCTION(BlueprintCallable)
	void ResetCapsuleHeight();

	UFUNCTION(BlueprintCallable)
	void SetIsRagdollDeath(bool IsActive) { bIsRagdollDeath = IsActive; }
	UFUNCTION(BlueprintCallable)
	bool GetIsRagdollDeath() { return bIsRagdollDeath; }

	UFUNCTION(BlueprintCallable)
	void ActivateRagdollCamera();

	UFUNCTION(BlueprintCallable)
	void SetEquippedWeaponType(EWeaponType NewType) { EquippedWeaponType = NewType; }
	UFUNCTION(BlueprintCallable)
	EWeaponType GetEquippedWeaponType() { return EquippedWeaponType; }

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSocket(FName SocketName);

	UFUNCTION(BlueprintCallable)
	void InteractionTrace();

	UFUNCTION(BlueprintCallable)
	bool IsInUninterruptableAction() { return bIsInUninterruptableAction; }



	//--------------------------------------------------------------
	// State Machine Functions
	//--------------------------------------------------------------

	//-------------------------------------
	// EPlayerVerticalMobility functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	// Set the character's vertical mobility state
	void SetPlayerVerticalMobilityState(EPlayerVerticalMobility NewState);

	UFUNCTION(BlueprintCallable)
	// Get the character's vertical mobility state
	EPlayerVerticalMobility GetPlayerVerticalMobilityState() { return PlayerVerticalMobilityState; }

	UFUNCTION(BlueprintCallable)
	// Returns true if the Player Move State has changed this/last frame (not sure which)
	bool HasPlayerVerticalMobilityStateChanged() { return PlayerVerticalMobilityState != LastPlayerVerticalMobilityState; }

	// Clear the changes made by the previous vertical mobility state
	void ClearLastPlayerVerticalMobilityStateChanges();

	//-------------------------------------
	// EPlayerHorizontalMobility functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	// Set the character's combat state
	void SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility NewState);

	UFUNCTION(BlueprintCallable)
	// Get the character's horizontal mobility state
	EPlayerHorizontalMobility GetPlayerHorizontalMobilityState() { return PlayerHorizontalMobilityState; }

	UFUNCTION(BlueprintCallable)
	// Returns true if the Player Move State has changed this/last frame (not sure which)
	bool HasPlayerHorizontalMobilityStateChanged() { return PlayerHorizontalMobilityState != LastPlayerHorizontalMobilityState; }

	// Clear the changes made by the previous horizontal mobility state
	void ClearLastPlayerHorizontalMobilityStateChanges();

	UFUNCTION(BlueprintCallable)
	// Get the character's combat state
	ECombatState GetPlayerCombatState() { return CombatComponent->GetCombatState(); }

	//-------------------------------------
	// EPlayerActionState functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	// Set the character's action state
	void SetPlayerActionState(EPlayerActionState NewState);

	UFUNCTION(BlueprintCallable)
	// Get the character's action state
	EPlayerActionState GetPlayerActionState() { return PlayerActionState; }

	UFUNCTION(BlueprintCallable)
	// Returns true if the Player Combat State has changed this/last frame (not sure which)
	bool HasPlayerActionStateChanged() { return PlayerActionState != LastPlayerActionState; }

	// Clear the changes made by the previous action state
	void ClearLastPlayerActionStateChanges();

	//-------------------------------------
	// EAttackType functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	// Set the attack type
	void SetAttackType(EAttackType NewState) { AttackType = NewState; }

	UFUNCTION(BlueprintCallable)
	// Get the current attack type
	EAttackType GetAttackType() { return AttackType; }

	void RequestJump();
	void RequestStopJumping();

	void RequestSprint();
	void RequestStopSprinting();

	void RequestHoldCrouch();
	void RequestStopCrouching();
	void RequestToggleCrouch();

	void RequestAim();
	void RequestStopAiming();

	void RequestReadyWeapon();

	void RequestWalkMode();
	void RequestStopWalkMode();

	void RequestInteractOrDodge();
	void RequestInteraction();
	void RequestDodge();

	void RequestLightAttack();
	void RequestHeavyAttack();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDeath(bool IsFellOut);

	UFUNCTION()
	void OnDeathTimerFinished();

	// CharacterStatComponent functions

	void CharacterStatisticsUpdate();

	// State machine check functions

	void CheckPlayerVerticalMobility();
	void CheckPlayerHorizontalMobility();
	void CheckPlayerActionState();

	// State machine update functions

	void PlayerVerticalMobilityUpdate();
	void PlayerHorizontalMobilityUpdate();
	void PlayerActionStateUpdate();

	void CheckCharacterExhaustion();

	

	bool PlayDodgeMontage();
	void UnbindDodgeMontage();

	UFUNCTION()
	void OnDodgeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void DodgeMontageOnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void DodgeMontageOnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ChaseArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* RightShoulderArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particle System")
	UParticleSystemComponent* ParticleSystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaDamagePerInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemsCollected = 0;

protected:

	// Player state variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerVerticalMobility PlayerVerticalMobilityState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerVerticalMobility LastPlayerVerticalMobilityState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerHorizontalMobility PlayerHorizontalMobilityState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerHorizontalMobility LastPlayerHorizontalMobilityState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerActionState PlayerActionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerActionState LastPlayerActionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EAttackType AttackType;

	// Movement variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float CurrentCharacterXYVelocity;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float CombatMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float WalkMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float CrouchMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float SprintMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float CombatSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float CrouchSprintSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 NormalMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 SprintingMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 CharacterMinAnalogWalkSpeed;

	//-----

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Controller")
	ARPGProjectPlayerController* PC;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CamShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCharacterStatisticComponent* CharacterStatisticComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaminaComponent* StaminaComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDamageHandlerComponent* DamageHandlerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatComponent* CombatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "On Death")
	float TimeRestartAfterDeath = 5.0f;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipped Weapon")
	//UChildActorComponent* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipped Weapon")
	EWeaponType EquippedWeaponType;

	FTimerHandle RestartLevelTimerHandle;

	bool bIsCrouched;
	bool bIsRagdollDeath;
	bool bIsExhausted;
	bool bIsFalling;
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bCanAttack;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Actions")
	bool bIsInUninterruptableAction;

	

	// Force Feedback values
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackIntensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackDuration = 1.0f;

	float DeltaSeconds;

	UPROPERTY(EditAnywhere, Category = "Interaction Trace Settings")
	TEnumAsByte<ETraceTypeQuery> SeeInteractableTraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category = "Interaction Trace Settings")
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractionObjectTypeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace Settings")
	float TraceDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace Settings")
	float SphereCastRadius = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace")
	AActor* LookedAtActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace")
	TArray<FHitResult> HitResultArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace")
	TArray<AActor*> HitActorArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Trace")
	bool bTraceWasBlocked;

	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* DodgeMontage = nullptr;

	FOnMontageBlendingOutStarted DodgeMontageBlendingOutDelegate;
	FOnMontageEnded DodgeMontageEndedDelegate;
};