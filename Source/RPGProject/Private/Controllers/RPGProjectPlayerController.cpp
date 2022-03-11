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
	
	MaxCharacterTurnSpeed = 10.0f;
	CharacterTurnSpeed = MaxCharacterTurnSpeed;
	bIsMovementStopped = false;
	bIsInMenu = false;

	CapsuleCrouchHeight = 68.0f;
}

void ARPGProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Binding = &
	InputComponent->BindAction(FName("InteractionStart"), IE_Pressed, this, &ARPGProjectPlayerController::StartInteraction);
	// Binding = &
	InputComponent->BindAction(FName("InteractionCancel"), IE_Pressed, this, &ARPGProjectPlayerController::StopInteraction);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ARPGProjectPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGProjectPlayerController::StopJumping);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerController::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerController::StopSprinting);
	InputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerController::HoldCrouch);
	InputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerController::StopCrouching);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerController::ToggleCrouch);
	InputComponent->BindAction("OffhandSkills", IE_Pressed, this, &ARPGProjectPlayerController::Aim);
	InputComponent->BindAction("OffhandSkills", IE_Released, this, &ARPGProjectPlayerController::StopAiming);
	InputComponent->BindAction("ReadyWeapon", IE_Pressed, this, &ARPGProjectPlayerController::RequestReadyWeapon);
	InputComponent->BindAction("Walk", IE_Pressed, this, &ARPGProjectPlayerController::Walking);
	InputComponent->BindAction("Walk", IE_Released, this, &ARPGProjectPlayerController::StopWalking);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARPGProjectPlayerController::Dodge);
	InputComponent->BindAction("LightAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestLightAttack);
	InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestHeavyAttack);

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::MoveRight);
	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::TurnRate);
	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::AddControllerYawInput);
	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::LookUpRate);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::AddControllerPitchInput);
	
	// FInputActionBinding* Binding;

	

	// Prevent dodge & interacting from consuming the input
	InputComponent->GetActionBinding(0).bConsumeInput = false; // 9 or 11 (Dodge)
	InputComponent->GetActionBinding(14).bConsumeInput = false;

	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponent Set up successfully!"));
}

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetCharacter());

}

void ARPGProjectPlayerController::StartInteraction()
{
	//if (PlayerCharacter)
	//{
	//	PlayerCharacter->SetPlayerActionState(EPlayerActionState::PAS_Interacting);
	//}
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

	CalculateDesiredActorRotation();

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetCurrentStamina() <= 0)
		{
			StopSprinting();
		}
	}

	CombatModeUpdate();
		
	InventoryUpdate();
}

void ARPGProjectPlayerController::CombatModeUpdate()
{
	if (PlayerCharacter)
	{
		switch (PlayerCharacter->GetPlayerCombatState())
		{
		case ECombatState::CS_AtEase:
			{
				//RelaxedMovementUpdate();
				break;
			}
			case ECombatState::CS_CombatReady:
			{
				//CombatMovementUpdate();
				StopAiming();
				break;
			}
		}
	}
}

void ARPGProjectPlayerController::CalculateDesiredActorRotation()
{
	
	PlayerInputRotation = { 0, (GetDesiredRotation().Yaw + UKismetMathLibrary::MakeRotFromX({ForwardValue, RightValue, 0}).Yaw), 0 };

	DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, PlayerCharacter->GetActorRotation().Yaw, 0 }, PlayerInputRotation, DeltaSeconds, CharacterTurnSpeed);
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
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Jump PlayerCharacter is nullptr."));
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
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopJumping PlayerCharacter is nullptr."));
	}
}

void ARPGProjectPlayerController::Sprint()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::Sprint called"));
	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Sprinting);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Sprint GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::StopSprinting()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StopSprinting called"));
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Sprinting)
		{
			// CheckSpeedToSetMoveState();
			PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Jogging);
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopSprinting PlayerCharacter is nullptr."));
	}

}

void ARPGProjectPlayerController::HoldCrouch()
{
	if (!PlayerCharacter->GetCharacterMovement()->IsMovingOnGround()) { return; }

	if (PlayerCharacter)
	{
		PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = true;
		PlayerCharacter->SetIsCrouched(true);
		PlayerCharacter->SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Crouching);
	}
}

void ARPGProjectPlayerController::StopCrouching()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCharacterMovement()->bWantsToCrouch = false;
		PlayerCharacter->SetIsCrouched(false);
		
		if (PlayerCharacter->GetPlayerVerticalMobilityState() == EPlayerVerticalMobility::PVM_Crouching)
		{
			PlayerCharacter->SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Standing);
		}
	}
}

void ARPGProjectPlayerController::ToggleCrouch()
{
	if (PlayerCharacter)
	{
		if (!PlayerCharacter->GetCharacterMovement()->IsCrouching())
		{
			HoldCrouch();
		}
		else
		{
			StopCrouching();
		}
	}
}

void ARPGProjectPlayerController::Aim()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerCombatState() == ECombatState::CS_AtEase)
		{
			PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("RightShoulder")));
			PlayerCharacter->bUseControllerRotationYaw = true;
			bIsAiming = true;
		}
	}
}

void ARPGProjectPlayerController::StopAiming()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("Chase")));
		PlayerCharacter->bUseControllerRotationYaw = false;
		bIsAiming = false;
	}
}

void ARPGProjectPlayerController::RequestReadyWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->ReadyWeapon();
	}
}

void ARPGProjectPlayerController::Walking()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::Walking called"));

	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Walking);
	}
}

void ARPGProjectPlayerController::StopWalking()
{
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StopWalking called"));

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Walking)
		{
			PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Jogging);
		}
	}
}

void ARPGProjectPlayerController::Dodge()
{
	
}

void ARPGProjectPlayerController::RequestLightAttack()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->LightAttack();
	}
}

void ARPGProjectPlayerController::RequestHeavyAttack()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->HeavyAttack();
	}
}


//--------------------------------------------------------------
// Axis Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::MoveForward(float Value)
{
	ForwardValue = Value;

	if (Value != 0 && PlayerCharacter && !bIsMovementStopped)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;

		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		PlayerCharacter->AddMovementInput(Direction, Value);

		// FRotator TargetRotation = UKismetMathLibrary::RInterpTo({ 0, PlayerCharacter->GetActorRotation().Yaw, 0}, { 0, GetDesiredRotation().Yaw, 0 }, DeltaSeconds, 5.0f);

		PlayerCharacter->SetActorRotation(DesiredActorRotation);

		//FString DString = "MoveForward: " + Direction.ToString();
		//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Yellow, DString);
		//FString DisplayValue = "MoveForward: " + FString::SanitizeFloat(Value);
		//GEngine->AddOnScreenDebugMessage(1, 0.5, FColor::Emerald, DisplayValue);

	}
}

void ARPGProjectPlayerController::MoveRight(float Value)
{
	RightValue = Value;

	if (Value != 0 && PlayerCharacter && !bIsMovementStopped)
	{
		// PlayerMoveState = EPlayerMoveState::PMS_Walking;
		
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		PlayerCharacter->AddMovementInput(Direction, Value);

		PlayerCharacter->SetActorRotation(DesiredActorRotation);

		//FString DString = "MoveRight: " + Direction.ToString();
		//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Yellow, DString);
		//FString DisplayValue = "MoveRight: " + FString::SanitizeFloat(Value);
		//GEngine->AddOnScreenDebugMessage(2, 0.5, FColor::Emerald, DisplayValue);

	}
}

void ARPGProjectPlayerController::TurnRate(float Rate)
{
	if (bIsInMenu) { return; }

	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::AddControllerYawInput(float Value)
{
	if (bIsInMenu) { return; }

	AddYawInput(Value);
}

void ARPGProjectPlayerController::LookUpRate(float Rate)
{
	if (bIsInMenu) { return; }

	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::AddControllerPitchInput(float Value)
{
	if (bIsInMenu) { return; }

	AddPitchInput(Value);
}