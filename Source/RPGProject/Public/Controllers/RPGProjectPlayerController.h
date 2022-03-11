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

	UFUNCTION(BlueprintImplementableEvent)
	void InventoryUpdate();

public:

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;

protected:

	virtual void BeginPlay() override;

	void CombatModeUpdate();

	void Jump();
	void StopJumping();
	void Sprint();
	void StopSprinting();
	void HoldCrouch();
	void StopCrouching();
	void ToggleCrouch();
	void Aim();
	void StopAiming();
	void RequestReadyWeapon();
	void Walking();
	void StopWalking();
	void Dodge();
	void RequestLightAttack();
	void RequestHeavyAttack();

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void AddControllerYawInput(float Value);
	void LookUpRate(float Rate);
	void AddControllerPitchInput(float Value);

	void StartInteraction();
	void StopInteraction();

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

	bool bIsAiming = false;
	bool bIsPressingSprint = false;
};
