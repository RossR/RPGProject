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

	UFUNCTION(BlueprintCallable)
	void SetIsInMenu(bool bActive) { bIsInMenu = bActive; }
	UFUNCTION(BlueprintCallable)
	bool GetIsInMenu() { return bIsInMenu; }

public:

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;

protected:

	virtual void BeginPlay() override;

	void CombatModeUpdate();

	// --- INPUT FUNCTIONS --- //

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

	void StartInteraction();
	void StopInteraction();

	void RequestMoveForward(float Value);
	void RequestMoveRight(float Value);
	void RequestTurnRate(float Rate);
	void RequestAddControllerYawInput(float Value);
	void RequestLookUpRate(float Rate);
	void RequestAddControllerPitchInput(float Value);

	// --- --- //

	void CalculateDesiredActorRotation();

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CapsuleCrouchHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator DesiredActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator PlayerInputRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float MaxCharacterTurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CharacterTurnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float ForwardValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float RightValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement")
	bool bIsMovementStopped;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Controls")
	bool bIsInMenu;

	ARPGProjectPlayerCharacter* PlayerCharacter;

	FTimerHandle SprintStaminaDrainTimerHandle;

	FTimerHandle StaminaRegenTimerHandle;

	float DeltaSeconds;
};
