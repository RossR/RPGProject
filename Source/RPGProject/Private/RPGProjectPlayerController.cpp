// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGProjectPlayerController.h"

ARPGProjectPlayerController::ARPGProjectPlayerController()
{

	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 70.0f;
	BaseLookUpRate = 70.0f;
	
}

void ARPGProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", IE_Pressed, this, &ARPGProjectPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGProjectPlayerController::StopJumping);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerController::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerController::StopSprinting);
	InputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerController::HoldCrouch);
	InputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerController::StopHoldingCrouch);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerController::ToggleCrouch);

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::MoveRight);
	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::TurnRate);
	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::AddControllerYawInput);
	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::LookUpRate);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::AddControllerPitchInput);

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponet Set up successfully!"));
}

// Called every frame
void ARPGProjectPlayerController::Tick(float DeltaTime)
{

}


//--------------------------------------------------------------
// Action Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::Jump()
{
	
}

void ARPGProjectPlayerController::StopJumping()
{

}

void ARPGProjectPlayerController::Sprint()
{
	/*
	// PlayerMoveState = EPlayerMoveState::PMS_Sprinting;

	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * 1.5f;
	GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;

	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	*/
}

void ARPGProjectPlayerController::StopSprinting()
{
	/*
	PlayerMoveState = EPlayerMoveState::PMS_Walking;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0;
	GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	*/
}

void ARPGProjectPlayerController::HoldCrouch()
{
	/*
	PlayerMoveState = EPlayerMoveState::PMS_Crouching;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 48.0f);
	*/
}

void ARPGProjectPlayerController::StopHoldingCrouch()
{
	/*
	PlayerMoveState = EPlayerMoveState::PMS_Walking;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	*/
}

void ARPGProjectPlayerController::ToggleCrouch()
{

}

//--------------------------------------------------------------
// Axis Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::MoveForward(float Value)
{
	if (Value != 0 && GetPawn() != nullptr)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		GetPawn()->AddMovementInput(Direction, Value);

		// FString DisplayValue = "MoveForward: " + FString::SanitizeFloat(Value);
		// GEngine->AddOnScreenDebugMessage(1, 0.5, FColor::Emerald, DisplayValue);

	}
}

void ARPGProjectPlayerController::MoveRight(float Value)
{
	if (Value != 0 && GetPawn() != nullptr)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;
		
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		GetPawn()->AddMovementInput(Direction, Value);

		// FString DisplayValue = "MoveRight: " + FString::SanitizeFloat(Value);
		// GEngine->AddOnScreenDebugMessage(2, 0.5, FColor::Emerald, DisplayValue);

	}
}

void ARPGProjectPlayerController::TurnRate(float Rate)
{
	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::AddControllerYawInput(float Value)
{
	AddYawInput(Value);
}

void ARPGProjectPlayerController::LookUpRate(float Rate)
{
	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::AddControllerPitchInput(float Value)
{
	AddPitchInput(Value);
}