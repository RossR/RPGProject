// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RPGProjectPlayerController.generated.h"


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

protected:

	void Jump();
	void StopJumping();
	void Sprint();
	void StopSprinting();
	void HoldCrouch();
	void StopHoldingCrouch();
	void ToggleCrouch();


	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void AddControllerYawInput(float Value);
	void LookUpRate(float Rate);
	void AddControllerPitchInput(float Value);

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

};