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
	
	// --- BIND ACTIONS --- //

	InputComponent->BindAction("Jump", IE_Pressed, this, &ARPGProjectPlayerController::RequestJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGProjectPlayerController::RequestStopJumping);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerController::RequestSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerController::RequestStopSprinting);

	InputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerController::RequestHoldCrouch);
	InputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerController::RequestStopCrouching);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleCrouch);

	InputComponent->BindAction("OffhandSkills", IE_Pressed, this, &ARPGProjectPlayerController::RequestAim);
	InputComponent->BindAction("OffhandSkills", IE_Released, this, &ARPGProjectPlayerController::RequestStopAiming);

	InputComponent->BindAction("ReadyWeapon", IE_Pressed, this, &ARPGProjectPlayerController::RequestReadyWeapon);

	InputComponent->BindAction("Walk", IE_Pressed, this, &ARPGProjectPlayerController::RequestWalkMode);
	InputComponent->BindAction("Walk", IE_Released, this, &ARPGProjectPlayerController::RequestStopWalkMode);

	InputComponent->BindAction("Interact/Dodge", IE_Pressed, this, &ARPGProjectPlayerController::RequestInteractOrDodge);
	InputComponent->BindAction("Interact", IE_Pressed, this, &ARPGProjectPlayerController::RequestInteraction);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARPGProjectPlayerController::RequestDodge);

	InputComponent->BindAction("LightAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestLightAttack);
	InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestHeavyAttack);

	InputComponent->BindAction(FName("InteractionStart"), IE_Pressed, this, &ARPGProjectPlayerController::StartInteraction);
	InputComponent->BindAction(FName("InteractionCancel"), IE_Pressed, this, &ARPGProjectPlayerController::StopInteraction);

	// --- BIND AXIS --- //

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::RequestMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::RequestMoveRight);

	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::RequestTurnRate);
	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::RequestAddControllerYawInput);

	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::RequestLookUpRate);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::RequestAddControllerPitchInput);
	
	// FInputActionBinding* Binding;

	// Prevent dodge & interacting from consuming the input
	//InputComponent->GetActionBinding(0).bConsumeInput = false; // 9 or 11 (Dodge)
	//InputComponent->GetActionBinding(14).bConsumeInput = false;

	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponent Set up successfully!"));
}

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetCharacter());

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
			RequestStopSprinting();
		}
	}

	CombatModeUpdate();
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
				RequestStopAiming();
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

void ARPGProjectPlayerController::RequestJump()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestJump();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Jump PlayerCharacter is nullptr."));
	}
}

void ARPGProjectPlayerController::RequestStopJumping()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopJumping();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopJumping PlayerCharacter is nullptr."));
	}
}

void ARPGProjectPlayerController::RequestSprint()
{
	//UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::Sprint called"));
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestSprint();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::Sprint GetCharacter() is nullptr."));
	}
}

void ARPGProjectPlayerController::RequestStopSprinting()
{
	//UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StopSprinting called"));
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopSprinting();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopSprinting PlayerCharacter is nullptr."));
	}

}

void ARPGProjectPlayerController::RequestHoldCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHoldCrouch();
	}
}

void ARPGProjectPlayerController::RequestStopCrouching()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopCrouching();
	}
}

void ARPGProjectPlayerController::RequestToggleCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestToggleCrouch();
	}
}

void ARPGProjectPlayerController::RequestAim()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestAim();
	}
}

void ARPGProjectPlayerController::RequestStopAiming()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopAiming();
	}
}

void ARPGProjectPlayerController::RequestReadyWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestReadyWeapon();
	}
}

void ARPGProjectPlayerController::RequestWalkMode()
{
	//UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::Walking called"));

	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Walking);
	}
}

void ARPGProjectPlayerController::RequestStopWalkMode()
{
	//UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::StopWalking called"));

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Walking)
		{
			PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Jogging);
		}
	}
}

void ARPGProjectPlayerController::RequestInteractOrDodge()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestInteractOrDodge();
	}
}

void ARPGProjectPlayerController::RequestInteraction()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestInteraction();
	}
}

void ARPGProjectPlayerController::RequestDodge()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestDodge();
	}
}

void ARPGProjectPlayerController::RequestLightAttack()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestLightAttack();
	}
}

void ARPGProjectPlayerController::RequestHeavyAttack()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHeavyAttack();
	}
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

//--------------------------------------------------------------
// Axis Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::RequestMoveForward(float Value)
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

void ARPGProjectPlayerController::RequestMoveRight(float Value)
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

void ARPGProjectPlayerController::RequestTurnRate(float Rate)
{
	if (bIsInMenu) { return; }

	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerYawInput(float Value)
{
	if (bIsInMenu) { return; }

	AddYawInput(Value);
}

void ARPGProjectPlayerController::RequestLookUpRate(float Rate)
{
	if (bIsInMenu) { return; }

	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerPitchInput(float Value)
{
	if (bIsInMenu) { return; }

	AddPitchInput(Value);
}