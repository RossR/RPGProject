// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGProjectPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

ARPGProjectPlayerController::ARPGProjectPlayerController()
{

	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 70.0f;
	BaseLookUpRate = 70.0f;
	
	// Variables for sprint function
	MovementSpeed = 600;
	SprintSpeedMultiplier = 1.5f;
	WalkingMaxAcceleration = 2048;
	SprintingMaxAcceleration = 8192;
	CharacterMinAnalogWalkSpeed = 0;

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
	InputComponent->BindAction("Aim", IE_Pressed, this, &ARPGProjectPlayerController::Aim);
	InputComponent->BindAction("Aim", IE_Released, this, &ARPGProjectPlayerController::StopAiming);

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::MoveRight);
	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::TurnRate);
	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::AddControllerYawInput);
	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::LookUpRate);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::AddControllerPitchInput);

	FInputActionBinding* Binding;

	Binding = &InputComponent->BindAction(FName("InteractionStart"), IE_Pressed, this, &ARPGProjectPlayerController::StartInteraction);
	Binding = &InputComponent->BindAction(FName("InteractionCancel"), IE_Pressed, this, &ARPGProjectPlayerController::StopInteraction);

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponent Set up successfully!"));
}

void ARPGProjectPlayerController::StartInteraction()
{
	OnInteractionStart.Broadcast();
}

void ARPGProjectPlayerController::StopInteraction()
{
	OnInteractionCancel.Broadcast();
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
	if (GetCharacter() != nullptr)
	{
		GetCharacter()->Jump();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Jump GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::StopJumping()
{
	if (GetCharacter() != nullptr)
	{
		GetCharacter()->StopJumping();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopJumping GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::Sprint()
{
	// PlayerMoveState = EPlayerMoveState::PMS_Sprinting;
	// Alternate way to cast to character -> ACharacter* PosCharacter = Cast<ACharacter>(GetPawn());

	if (GetCharacter() != nullptr)
	{
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * SprintSpeedMultiplier;
		GetCharacter()->GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacter()->GetCharacterMovement()->MaxWalkSpeed;
		GetCharacter()->GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Sprint GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::StopSprinting()
{

	if (GetCharacter() != nullptr)
	{
		GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
		GetCharacter()->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
		GetCharacter()->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopSprinting GetCharacter() is nullptr."));
	}
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

void ARPGProjectPlayerController::Aim()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter());

	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("RightShoulder")));
	}
}

void ARPGProjectPlayerController::StopAiming()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetCharacter());

	if (PlayerCharacter != nullptr)
	{
		UArrowComponent* ArrowComp = PlayerCharacter->ChaseArrow;
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("Chase")));
	}
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