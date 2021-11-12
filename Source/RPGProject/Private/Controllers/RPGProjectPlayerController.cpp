// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/RPGProjectPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ARPGProjectPlayerController::ARPGProjectPlayerController()
{
	
	PrimaryActorTick.bCanEverTick = true;

	// set our turn rates for input
	BaseTurnRate = 70.0f;
	BaseLookUpRate = 70.0f;
	
	// Variables for sprint function
	MovementSpeed = 600;
	SprintSpeedMultiplier = 1.5f;
	CrouchSpeedMultiplier = 0.66f;
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

	// FInputActionBinding* Binding;

	// Binding = &
	InputComponent->BindAction(FName("InteractionStart"), IE_Pressed, this, &ARPGProjectPlayerController::StartInteraction);
	// Binding = &
	InputComponent->BindAction(FName("InteractionCancel"), IE_Pressed, this, &ARPGProjectPlayerController::StopInteraction);

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponent Set up successfully!"));
}

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetCharacter());
}

void ARPGProjectPlayerController::StartInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StartInteraction called"));
	OnInteractionStart.Broadcast();
}

void ARPGProjectPlayerController::StopInteraction()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StopInteraction called"));
	OnInteractionCancel.Broadcast();
}

// Called every frame
void ARPGProjectPlayerController::Tick(float DeltaTime)
{
	DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetIsCrouched())
		{
			GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * CrouchSpeedMultiplier;
		}
		else if (!PlayerCharacter->GetIsCrouched() && PlayerCharacter->GetPlayerMoveState() != EPlayerMoveState::PMS_Sprinting)
		{
			GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
		}
	}

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetCurrentStamina() <= 0)
		{
			StopSprinting();
		}
	}
		
}

void ARPGProjectPlayerController::SprintTimerFinished()
{
	// UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::SprintTimerFinished called"));
	if (PlayerCharacter)
	{
		float Damage = PlayerCharacter->StaminaDamagePerInterval;
		float Speed = UKismetMathLibrary::VSizeXY(PlayerCharacter->GetVelocity());
		
		if (Speed > 0)
		{
			PlayerCharacter->TakeStaminaDamage(Damage);
		}
	}
}


//--------------------------------------------------------------
// Action Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::Jump()
{
	if (PlayerCharacter)
	{
		if (!PlayerCharacter->GetIsCrouched())
		{
			PlayerCharacter->Jump();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Jump GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::StopJumping()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->StopJumping();
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

	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Sprinting);
		PlayerCharacter->SetIsCrouched(false);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * SprintSpeedMultiplier;
		PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
		PlayerCharacter->GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;

		GetWorld()->GetTimerManager().SetTimer(SprintStaminaDrainTimerHandle, this, &ARPGProjectPlayerController::SprintTimerFinished, DeltaSeconds, true, -1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Sprint GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::StopSprinting()
{

	if (PlayerCharacter)
	{
		if (!PlayerCharacter->GetIsCrouched())
		{
			PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Walking);
		}
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
		PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
		PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopSprinting GetCharacter() is nullptr."));
	}

	GetWorld()->GetTimerManager().ClearTimer(SprintStaminaDrainTimerHandle);
}

void ARPGProjectPlayerController::HoldCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsCrouched(true);
	}

	/*
	PlayerMoveState = EPlayerMoveState::PMS_Crouching;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 48.0f);
	*/
}

void ARPGProjectPlayerController::StopHoldingCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsCrouched(false);
	}
	/*
	PlayerMoveState = EPlayerMoveState::PMS_Walking;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	*/
}

void ARPGProjectPlayerController::ToggleCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsCrouched(!PlayerCharacter->GetIsCrouched());
	}
}

void ARPGProjectPlayerController::Aim()
{
	if (PlayerCharacter != nullptr)
	{
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("RightShoulder")));
		PlayerCharacter->bUseControllerRotationYaw = true;
	}
}

void ARPGProjectPlayerController::StopAiming()
{
	if (PlayerCharacter != nullptr)
	{
		// UArrowComponent* ArrowComp = PlayerCharacter->ChaseArrow;
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("Chase")));
		PlayerCharacter->bUseControllerRotationYaw = false;
	}
}

//--------------------------------------------------------------
// Axis Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::MoveForward(float Value)
{
	if (Value != 0 && PlayerCharacter)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		PlayerCharacter->AddMovementInput(Direction, Value);

		// FString DisplayValue = "MoveForward: " + FString::SanitizeFloat(Value);
		// GEngine->AddOnScreenDebugMessage(1, 0.5, FColor::Emerald, DisplayValue);

	}
}

void ARPGProjectPlayerController::MoveRight(float Value)
{
	if (Value != 0 && PlayerCharacter)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;
		
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		PlayerCharacter->AddMovementInput(Direction, Value);

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