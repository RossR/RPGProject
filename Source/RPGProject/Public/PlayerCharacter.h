// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"


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

class UHealthComponent;

UCLASS(BlueprintType)
class RPGPROJECT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* ChaseArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* RightShoulderArrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Enum)
	TEnumAsByte<EPlayerMoveState::State> PlayerMoveState;

	UFUNCTION(BlueprintCallable)
	void MoveCameraToArrowLocation(FName ArrowName);

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

	/*
	void Sprint();
	void StopSprinting();
	void HoldCrouch();
	void StopHoldingCrouch();
	void ToggleCrouch();
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnRate(float Rate);
	void LookUpRate(float Rate);
	*/

	void OnDeath(bool IsFellOut);

	UPROPERTY(EditAnywhere)
	UHealthComponent* HealthComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called when the actor falls out the world (KillZ)
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	 
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};