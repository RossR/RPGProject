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

	void CalculateDesiredActorRotation();
	void CheckSpeedToSetMoveState();

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float CharacterSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 MovementSpeed;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 CombatMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 WalkMovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	int32 CrouchMovementSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	int32 SprintMovementSpeed;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float CombatSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float SprintSpeedMultiplier;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "Character Movement")
	float CrouchSpeedMultiplier;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 WalkingMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 SprintingMaxAcceleration;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "Character Movement")
	int32 CharacterMinAnalogWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CapsuleCrouchHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator DesiredActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator PlayerInputRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CharacterTurnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float ForwardValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float RightValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement")
	bool bIsMovementStopped;
	

	ARPGProjectPlayerCharacter* PlayerCharacter;

	FTimerHandle SprintStaminaDrainTimerHandle;

	FTimerHandle StaminaRegenTimerHandle;

	float DeltaSeconds;

	bool IsAiming = false;
};
