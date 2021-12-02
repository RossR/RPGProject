// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Actors/Weapons/WeaponBase.h"
#include "RPGProjectPlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerMoveState : uint8
{
	PMS_Idle UMETA(DisplayName = "Idle"),
	PMS_Walking UMETA(DisplayName = "Walking"),
	PMS_Jogging UMETA(DisplayName = "Jogging"),
	PMS_Sprinting UMETA(DisplayName = "Sprinting"),
	PMS_Crouching UMETA(DisplayName = "Crouching"),

	PMS_Max UMETA(Hidden)
};
	
UENUM(BlueprintType)
enum class EPlayerCombatState : uint8
{
	PCS_AtEase UMETA(DisplayName = "At Ease"),
	PCS_CombatReady UMETA(DisplayName = "Combat Ready"),

	PCS_Max UMETA(Hidden)
};

class ARPGProjectPlayerController;

class UCameraShakeBase;
class UParticleSystemComponent;
class UChildActorComponent;

class UHealthComponent;
class UStaminaComponent;
class UDamageHandlerComponent;


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
	void SetPlayerMoveState(EPlayerMoveState NewState);
	UFUNCTION(BlueprintCallable)
	EPlayerMoveState GetPlayerMoveState() { return PlayerMoveState; }
	UFUNCTION(BlueprintCallable)
	// Returns true if the Player Move State has changed this/last frame (not sure which)
	bool HasPlayerMoveStateChanged() { return PlayerMoveState != LastPlayerMoveState; }

	UFUNCTION(BlueprintCallable)
	void SetPlayerCombatState(EPlayerCombatState NewState);
	UFUNCTION(BlueprintCallable)
	EPlayerCombatState GetPlayerCombatState() { return PlayerCombatState; }
	UFUNCTION(BlueprintCallable)
	// Returns true if the Player Combat State has changed this/last frame (not sure which)
	bool HasPlayerCombatStateChanged() { return PlayerCombatState != LastPlayerCombatState; }

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void OnDeath(bool IsFellOut);

	UFUNCTION()
	void OnDeathTimerFinished();

	void CheckCharacterExhaustion();



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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerMoveState PlayerMoveState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerMoveState LastPlayerMoveState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerCombatState PlayerCombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player States")
	EPlayerCombatState LastPlayerCombatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stamina)
	float StaminaDamagePerInterval = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ItemsCollected = 0;

protected:

	ARPGProjectPlayerController* PC;

	UPROPERTY(EditAnywhere, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CamShake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterMovement)
	int32 MovementSpeed;

	UPROPERTY(EditAnywhere)
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere)
	UStaminaComponent* StaminaComponent;

	UPROPERTY(EditAnywhere)
	UDamageHandlerComponent* DamageHandlerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "On Death")
	float TimeRestartAfterDeath = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipped Weapon")
	UChildActorComponent* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipped Weapon")
	EWeaponType EquippedWeaponType;

	FTimerHandle RestartLevelTimerHandle;

	bool bIsCrouched;
	bool bIsRagdollDeath;
	bool bIsExhausted;
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	bool bIsAttacking;

	// Force Feedback values
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackIntensity = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Force Feedback")
	float ForceFeedbackDuration = 1.0f;

};