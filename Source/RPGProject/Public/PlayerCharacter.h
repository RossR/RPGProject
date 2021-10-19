// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


/*UENUM()
enum class EPlayerMoveState
{
	PMS_Walking = 0 UMETA(DisplayName = "Walking"),
	PMS_Sprinting = 1 UMETA(DisplayName = "Sprinting"),
	PMS_Crouching = 2 UMETA(DisplayName = "Crouching"),

};
*/

UENUM(BlueprintType)
namespace EPlayerMoveState
{
	enum State
	{
		PMS_Walking = 0 UMETA(DisplayName = "Walking"),
		PMS_Sprinting = 1 UMETA(DisplayName = "Sprinting"),
		PMS_Crouching = 2 UMETA(DisplayName = "Crouching"),

		PMS_Max UMETA(Hidden)
	};
}


UCLASS(BlueprintType)
class RPGPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enum)
	TEnumAsByte<EPlayerMoveState::State> PlayerMoveState;

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = CharacterMovement)
	int32 MovementSpeed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Sprint();
	void StopSprinting();
	void HoldCrouch();
	void StopHoldingCrouch();
	void ToggleCrouch();
	

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void LookUpRate(float Rate);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};