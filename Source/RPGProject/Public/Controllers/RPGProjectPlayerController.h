// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGProjectPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInteractionStart);
DECLARE_MULTICAST_DELEGATE(FOnInteractionCancel);

class ARPGProjectPlayerCharacter;

UCLASS()
class RPGPROJECT_API ARPGProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ARPGProjectPlayerController();

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;

protected:

	virtual void BeginPlay() override;

	void TickMovementUpdate();
	void RelaxedMovementUpdate();
	void CombatMovementUpdate();
	void StopPreviousStateEffects();

	void Jump();
	void StopJumping();
	void Sprint();
	void StopSprinting();
	void HoldCrouch();
	void StopCrouching();
	void ToggleCrouch();
	void Aim();
	void StopAiming();
	void Walking();
	void StopWalking();
	void Dodge();


	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void AddControllerYawInput(float Value);
	void LookUpRate(float Rate);
	void AddControllerPitchInput(float Value);

	void StartInteraction();
	void StopInteraction();

	void SprintTimerFinished();

	void CheckSpeedToSetMoveState();

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterMovement)
	float CharacterSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 MovementSpeed;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 CombatMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 WalkMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterMovement)
	int32 CrouchMovementSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CharacterMovement)
	int32 SprintMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	float CombatSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	float CrouchSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 WalkingMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 SprintingMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = CharacterMovement)
	int32 CharacterMinAnalogWalkSpeed;

	ARPGProjectPlayerCharacter* PlayerCharacter;

	FTimerHandle SprintStaminaDrainTimerHandle;

	FTimerHandle StaminaRegenTimerHandle;

	float DeltaSeconds;

	bool IsAiming = false;
};
