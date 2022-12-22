// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "Actors/Components/CombatComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/HitFXInterface.h"
#include "DataAssets/HitFXData.h"
#include "PlayerCameraManagers/RPGPlayerCameraManager.h"
#include "RPGProjectPlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerVerticalMobility : uint8
{
	PVM_Standing	UMETA(DisplayName = "Standing"),
	PVM_Crouching	UMETA(DisplayName = "Crouching"),
	PVM_Crawling	UMETA(DisplayName = "Crawling"),
	PVM_Jumping		UMETA(DisplayName = "Jumping"),
	PVM_Falling		UMETA(DisplayName = "Falling"),

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
	PAS_Idle			UMETA(DisplayName = "Idle"),
	PAS_Dodging			UMETA(DisplayName = "Dodging"),
	PAS_Interacting		UMETA(DisplayName = "Interacting"),
	PAS_Guarding		UMETA(DisplayName = "Guarding"),
	PAS_Aiming			UMETA(DisplayName = "Aiming"),
	PAS_Casting			UMETA(DisplayName = "Casting"),
	PAS_CombatAction	UMETA(DisplayName = "Combat Action"),
	PAS_Incapacitated	UMETA(DisplayName = "Incapacitated"),

	PAS_MAX			UMETA(Hidden)
};

class ARPGProjectPlayerController;
class ARPGPlayerCameraManager;

class AAIController;
class UCameraShakeBase;
class UParticleSystemComponent;
class UChildActorComponent;
class UShapeComponent;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;

class UCharacterStatisticComponent;
class UHealthComponent;
class UStaminaComponent;
class UDamageHandlerComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UCombatComponent;

UCLASS(BlueprintType)
class RPGPROJECT_API ARPGProjectPlayerCharacter : public ACharacter, public IHitFXInterface
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
	void ReduceCurrentStamina(float Damage);

	void SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval);

	UFUNCTION(BlueprintCallable)
	void HandleItemCollected();

	UFUNCTION(BlueprintImplementableEvent)
	void ItemCollected();

	UFUNCTION(BlueprintImplementableEvent)
	void DoorOpenInteractionStarted(AActor* InteractableActor);

	UFUNCTION(BlueprintImplementableEvent)
	void InteractionStarted(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable)
	void MoveCameraToArrowLocation(FName ArrowName);

	UFUNCTION(BlueprintCallable)
	const bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
	const float GetCurrentHealthPoints() const;

	UFUNCTION(BlueprintCallable)
	const float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable)
	bool IsStaminaFull();

	/*UFUNCTION(BlueprintCallable)
	bool IsCharacterExhausted() { return bIsExhausted; }*/

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
	void InteractionTrace();

	UFUNCTION(BlueprintCallable)
	void SetIsInUninterruptableAction(bool bActive) { bIsInUninterruptableAction = bActive; }
	UFUNCTION(BlueprintPure)
	bool GetIsInUninterruptableAction() { return bIsInUninterruptableAction; }

	UFUNCTION(BlueprintCallable)
	ARPGPlayerCameraManager* GetRPGPlayerCameraManager() { return RPGPlayerCameraManager; }

	UFUNCTION(BlueprintCallable)
	UCameraComponent* GetPlayerCamera() { return HasActiveCameraComponent() ? PlayerCamera : nullptr; }

	UHitFXData* GetHitFXData() override { return HitFXOverride; };

	UFUNCTION(BlueprintCallable)
	void SetIsInteractionAvailable(bool IsActive) { bInteractionAvailable = IsActive; }
	UFUNCTION(BlueprintPure)
	bool GetIsInteractionAvailable() { return bInteractionAvailable; }

	void SetWeaponStance(ECombatWeaponStance CombatWeaponStanceType, UItemWeaponData* StanceWeaponData);
	void ResetWeaponStance();

	UFUNCTION(BlueprintCallable)
	void EnableDodgeCollision(bool bActive);

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
	// Get the character's last action state
	EPlayerActionState GetLastPlayerActionState() { return LastPlayerActionState; }

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
	void SetAttackType(EAttackType NewState) { CurrentAttackType = NewState; }

	UFUNCTION(BlueprintCallable)
	// Get the current attack type
	EAttackType GetAttackType() { return CurrentAttackType; }

	UFUNCTION(BlueprintCallable)
	void SetBodyPartHit(FName NewBodyPartHit) { BodyPartHit = NewBodyPartHit; }

	UFUNCTION(BlueprintCallable)
	FName GetBodyPartHit() { return BodyPartHit; }

	void RequestJump();
	void RequestStopJumping();

	void RequestSprint();
	void RequestStopSprinting();

	void RequestHoldCrouch();
	void RequestStopCrouching();
	void RequestToggleCrouch();

	UFUNCTION(BlueprintCallable)
	void RequestMainhandStance();
	UFUNCTION(BlueprintCallable)
	void RequestStopMainhandStance();

	UFUNCTION(BlueprintCallable)
	void RequestOffhandStance();
	UFUNCTION(BlueprintCallable)
	void RequestStopOffhandStance();

	void RequestSheatheUnsheatheWeapon();

	void RequestWalkMode();
	void RequestStopWalkMode();

	void RequestContextAction();
	void RequestHoldContextAction();
	void RequestStopContextAction(bool bWasButtonHeld = false);

	void RequestInteraction();
	void RequestDodge();

	void RequestLightAttack();
	void RequestStopLightAttack();

	void RequestHeavyAttack();
	void RequestStopHeavyAttack();

	void RequestLightAttackFinisher();
	void RequestHeavyAttackFinisher();

	void RequestCombatAction();
	void RequestStopCombatAction();
	
	void RequestSwapWeaponLoadout();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDeath(bool IsFellOut);

	UFUNCTION()
	void OnDeathTimerFinished();

	void UpdateCurves();

	// State machine check functions

	void CheckPlayerVerticalMobility();
	void CheckPlayerHorizontalMobility();
	void CheckPlayerActionState();

	// State machine update functions

	void PlayerVerticalMobilityUpdate();
	void PlayerHorizontalMobilityUpdate();
	void PlayerActionStateUpdate();

	void CombatStanceUpdate();

	void CheckCharacterExhaustion();

private:
	
	void PopulateHealthComponentHitboxMap();

	void PopulateCameraSpringArmMap();
	void PopulateCameraArrowMap();

	

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Particle System")
	UParticleSystemComponent* ParticleSystemComponent;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cameras - Spring Arms")
	TMap<ECameraView, USpringArmComponent*> CameraSpringArmMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cameras - Arrow")
	TMap<ECameraView, UArrowComponent*> CameraArrowMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* ChaseArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UArrowComponent* RightShoulderArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "[Character Movement] Stamina Damage")
	float StaminaDamagePerSecond = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemsCollected = 0;

	// Player state variables
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Action")
	EPlayerActionState PlayerActionState = EPlayerActionState::PAS_Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Action")
	EPlayerActionState LastPlayerActionState = PlayerActionState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Combat |")
	EAttackType CurrentAttackType = EAttackType::AT_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Combat | Weapon Stance")
	ECombatWeaponStance RequestedCombatWeaponStance = ECombatWeaponStance::CWS_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Mobility | Horizonal ")
	EPlayerHorizontalMobility PlayerHorizontalMobilityState = EPlayerHorizontalMobility::PHM_Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Mobility | Horizonal ")
	EPlayerHorizontalMobility LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Mobility | Vertical")
	EPlayerVerticalMobility PlayerVerticalMobilityState = EPlayerVerticalMobility::PVM_Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | States | Mobility | Vertical")
	EPlayerVerticalMobility LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;

	// Movement variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | Movement")
	float CurrentCharacterXYVelocity = 0.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float MovementSpeed = 400.f;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float CombatMovementSpeed = 0.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float WalkMovementSpeed = 150.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | Movement")
	float CrouchMovementSpeed = 265.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | Movement")
	float SprintMovementSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | Movement")
	float TotalSpeedModifier = 1.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float CombatSpeedModifier = 1.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float SprintSpeedModifier = 1.6875f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float CrouchSprintSpeedModifier = 1.25f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	float StaminaExhaustedSpeedModifier = 0.5f;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	int32 NormalMaxAcceleration = 2048;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	int32 SprintingMaxAcceleration = 8192;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Settings | Movement")
	int32 CharacterMinAnalogWalkSpeed = 0;

	//-----

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Controller")
	ARPGProjectPlayerController* PC;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI Controller")
	AAIController* AIPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Camera Manager")
	ARPGPlayerCameraManager* RPGPlayerCameraManager;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CamShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate = 70.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate = 70.f;

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
	EWeaponType EquippedWeaponType = EWeaponType::WT_None;

	FTimerHandle RestartLevelTimerHandle;

	bool bIsCrouched = false;
	bool bIsRagdollDeath = false;
	bool bIsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Settings | Combat")
	bool bCanAttack = true;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	//bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Actions")
	bool bIsInUninterruptableAction = false;

	

	// Force Feedback values
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackIntensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Character Settings | Interaction Trace")
	TEnumAsByte<ETraceTypeQuery> SeeInteractableTraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category = "Character Settings | Interaction Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractionObjectTypeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	float TraceDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	float SphereCastRadius = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	AActor* LookedAtActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	TArray<FHitResult> HitResultArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	TArray<AActor*> HitActorArray;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Settings | Interaction Trace")
	bool bTraceWasBlocked = false;

	FName BodyPartHit = "";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock-On Camera")
	TArray<AActor*> LockOnActorArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta = (DisplayName = "Hit FX Override"), Category = "Hit FX Override")
	UHitFXData* HitFXOverride;

	bool bInteractionAvailable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiver")
	UStaticMeshComponent* QuiverMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quiver - Arrow Mesh Array")
	TMap<int, UStaticMeshComponent*> ArrowMeshMap;


	// -- Curves --
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Settings | Curves")
	float MovementSpeedReductionScaleCurve = 1.f;

	// ------------

};