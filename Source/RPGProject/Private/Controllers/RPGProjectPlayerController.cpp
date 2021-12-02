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
	
	CharacterTurnSpeed = 10.0f;
	bIsMovementStopped = false;

	// Variables for movement functions
	SprintSpeedMultiplier = 1.5f;
	CrouchSpeedMultiplier = 0.66f;
	CombatSpeedMultiplier = 0.5f;

	WalkingMaxAcceleration = 2048;
	SprintingMaxAcceleration = 8192;
	CharacterMinAnalogWalkSpeed = 0;

	MovementSpeed = 600;
	WalkMovementSpeed = 150;
	CrouchMovementSpeed = MovementSpeed * CrouchSpeedMultiplier;
	SprintMovementSpeed = MovementSpeed * SprintSpeedMultiplier;
	CombatMovementSpeed = MovementSpeed * CombatSpeedMultiplier;

	CapsuleCrouchHeight = 68.0f;
}

void ARPGProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Jump", IE_Pressed, this, &ARPGProjectPlayerController::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGProjectPlayerController::StopJumping);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerController::Sprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerController::StopSprinting);
	InputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerController::HoldCrouch);
	InputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerController::StopCrouching);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerController::ToggleCrouch);
	InputComponent->BindAction("Aim", IE_Pressed, this, &ARPGProjectPlayerController::Aim);
	InputComponent->BindAction("Aim", IE_Released, this, &ARPGProjectPlayerController::StopAiming);
	InputComponent->BindAction("Walk", IE_Pressed, this, &ARPGProjectPlayerController::Walking);
	InputComponent->BindAction("Walk", IE_Released, this, &ARPGProjectPlayerController::StopWalking);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARPGProjectPlayerController::Dodge);

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

	CrouchMovementSpeed = MovementSpeed * CrouchSpeedMultiplier;
	SprintMovementSpeed = MovementSpeed * SprintSpeedMultiplier;
	CombatMovementSpeed = MovementSpeed * CombatSpeedMultiplier;
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
	if (PlayerCharacter) { CharacterSpeed = UKismetMathLibrary::VSizeXY(PlayerCharacter->GetVelocity()); }

	CalculateDesiredActorRotation();

	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetCurrentStamina() <= 0)
		{
			StopSprinting();
		}
	}

	TickMovementUpdate();
		
}

void ARPGProjectPlayerController::TickMovementUpdate()
{
	if (PlayerCharacter)
	{
		switch (PlayerCharacter->GetPlayerCombatState())
		{
			case EPlayerCombatState::PCS_AtEase:
			{
				RelaxedMovementUpdate();
				break;
			}
			case EPlayerCombatState::PCS_CombatReady:
			{
				CombatMovementUpdate();
				StopAiming();
				break;
			}
		}
	}
}

void ARPGProjectPlayerController::RelaxedMovementUpdate()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->HasPlayerMoveStateChanged() || PlayerCharacter->HasPlayerCombatStateChanged())
		{

			switch (PlayerCharacter->GetPlayerMoveState())
			{
				case EPlayerMoveState::PMS_Idle:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Walking:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Jogging:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Sprinting:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Crouching:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}
			}

			StopPreviousStateEffects();

		}
	}
}

void ARPGProjectPlayerController::CombatMovementUpdate()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->HasPlayerMoveStateChanged() || PlayerCharacter->HasPlayerCombatStateChanged())
		{

			switch (PlayerCharacter->GetPlayerMoveState())
			{
				case EPlayerMoveState::PMS_Idle:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * CombatSpeedMultiplier;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Walking:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed * CombatSpeedMultiplier;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Jogging:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * CombatSpeedMultiplier;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Sprinting:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed * CombatSpeedMultiplier;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;
					break;
				}

				case EPlayerMoveState::PMS_Crouching:
				{
					PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed * CombatSpeedMultiplier;
					PlayerCharacter->GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
					PlayerCharacter->GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;
					break;
				}
			}

			StopPreviousStateEffects();
			
		}
	}
}

void ARPGProjectPlayerController::StopPreviousStateEffects()
{
	if (PlayerCharacter->HasPlayerMoveStateChanged())
	{
		switch (PlayerCharacter->LastPlayerMoveState)
		{
			case EPlayerMoveState::PMS_Idle:
			{

				break;
			}

			case EPlayerMoveState::PMS_Walking:
			{

				break;
			}

			case EPlayerMoveState::PMS_Jogging:
			{

				break;
			}

			case EPlayerMoveState::PMS_Sprinting:
			{
				StopSprinting();
				break;
			}

			case EPlayerMoveState::PMS_Crouching:
			{
				StopCrouching();
				break;
			}
		}

		PlayerCharacter->SetPlayerMoveState(PlayerCharacter->GetPlayerMoveState());
	}
}

void ARPGProjectPlayerController::SprintTimerFinished()
{
	// UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerController::SprintTimerFinished called"));
	if (PlayerCharacter)
	{
		float Damage = PlayerCharacter->StaminaDamagePerInterval;

		if (CharacterSpeed > 0  && !PlayerCharacter->IsCharacterFalling())	
		{ 
			PlayerCharacter->TakeStaminaDamage(Damage); 
		}
	}
}

void ARPGProjectPlayerController::CheckSpeedToSetMoveState()
{
	if (CharacterSpeed > 0)
	{
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Jogging);
	}
	else
	{
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Idle);
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
	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Sprinting);

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
		if (PlayerCharacter->GetPlayerMoveState() == EPlayerMoveState::PMS_Sprinting)
		{
			CheckSpeedToSetMoveState();
		}
		
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ARPGProjectPlayerController::StopSprinting PlayerCharacter is nullptr."));
	}

	GetWorld()->GetTimerManager().ClearTimer(SprintStaminaDrainTimerHandle);
}

void ARPGProjectPlayerController::HoldCrouch()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsCrouched(true);
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Crouching);
		PlayerCharacter->SetCapsuleHeight(CapsuleCrouchHeight);
		PlayerCharacter->GetMesh()->SetRelativeLocation({ 0.0f,0.0f,-(CapsuleCrouchHeight + 6.0f) });
	}

	/*
	PlayerMoveState = EPlayerMoveState::PMS_Crouching;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 48.0f);
	*/
}

void ARPGProjectPlayerController::StopCrouching()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetIsCrouched(false);
		PlayerCharacter->ResetCapsuleHeight();
		PlayerCharacter->GetMesh()->SetRelativeLocation({ 0.0f,0.0f,-95.0f });
		
		if (PlayerCharacter->GetPlayerMoveState() == EPlayerMoveState::PMS_Crouching)
		{
			CheckSpeedToSetMoveState();
		}
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

		if (PlayerCharacter->GetIsCrouched())
		{
			PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Crouching);
			PlayerCharacter->SetCapsuleHeight(CapsuleCrouchHeight);
			PlayerCharacter->GetMesh()->SetRelativeLocation({ 0.0f,0.0f,-(CapsuleCrouchHeight + 6.0f) });
		}
		else if (!PlayerCharacter->GetIsCrouched())
		{
			StopCrouching();
		}
	}
}

void ARPGProjectPlayerController::Aim()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerCombatState() == EPlayerCombatState::PCS_AtEase)
		{
			PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("RightShoulder")));
			PlayerCharacter->bUseControllerRotationYaw = true;
			IsAiming = true;
		}
	}
}

void ARPGProjectPlayerController::StopAiming()
{
	if (PlayerCharacter)
	{
		// UArrowComponent* ArrowComp = PlayerCharacter->ChaseArrow;
		PlayerCharacter->MoveCameraToArrowLocation(FName(TEXT("Chase")));
		PlayerCharacter->bUseControllerRotationYaw = false;
		IsAiming = false;
	}
}

void ARPGProjectPlayerController::Walking()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerMoveState(EPlayerMoveState::PMS_Walking);
	}
}

void ARPGProjectPlayerController::StopWalking()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerMoveState() == EPlayerMoveState::PMS_Walking)
		{
			CheckSpeedToSetMoveState(); 
		}
	}
}

void ARPGProjectPlayerController::Dodge()
{

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