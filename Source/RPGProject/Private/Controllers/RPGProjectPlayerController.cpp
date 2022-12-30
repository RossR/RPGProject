// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/RPGProjectPlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Actors/Components/CombatComponent.h"

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

	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::GPAM_MAX; i++)
	{
		InputButtonStateMap.Add((EGamePadActionMappings)i);
	}
	
}

void ARPGProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// --- BIND ACTIONS --- //

	UpdateGamepadInputActionBindingsMap();

	// Gamepad

	InputComponent->BindAction("Gamepad_LightAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestLightAttack);
	InputComponent->BindAction("Gamepad_LightAttack", IE_Released, this, &ARPGProjectPlayerController::RequestStopLightAttack);

	InputComponent->BindAction("Gamepad_HeavyAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestHeavyAttack);
	InputComponent->BindAction("Gamepad_HeavyAttack", IE_Released, this, &ARPGProjectPlayerController::RequestStopHeavyAttack);

	InputComponent->BindAction("Gamepad_ContextAction", IE_Pressed, this, &ARPGProjectPlayerController::RequestContextAction);
	InputComponent->BindAction("Gamepad_ContextAction", IE_Released, this, &ARPGProjectPlayerController::RequestStopContextAction);

	InputComponent->BindAction("Gamepad_JumpAndCrouch", IE_Pressed, this, &ARPGProjectPlayerController::RequestJumpOrCrouch);
	InputComponent->BindAction("Gamepad_JumpAndCrouch", IE_Released, this, &ARPGProjectPlayerController::RequestStopJumpOrCrouch);

	InputComponent->BindAction("Gamepad_OffhandStance", IE_Pressed, this, &ARPGProjectPlayerController::RequestOffhandStance);
	InputComponent->BindAction("Gamepad_OffhandStance", IE_Released, this, &ARPGProjectPlayerController::RequestStopOffhandStance);

	InputComponent->BindAction("Gamepad_MainhandStance", IE_Pressed, this, &ARPGProjectPlayerController::RequestMainhandStance);
	InputComponent->BindAction("Gamepad_MainhandStance", IE_Released, this, &ARPGProjectPlayerController::RequestStopMainhandStance);

	InputComponent->BindAction("Gamepad_SheatheAndUnsheathe", IE_Pressed, this, &ARPGProjectPlayerController::RequestSheatheUnsheatheWeapon);

	InputComponent->BindAction("Gamepad_CombatAction", IE_Pressed, this, &ARPGProjectPlayerController::RequestCombatAction);
	InputComponent->BindAction("Gamepad_CombatAction", IE_Released, this, &ARPGProjectPlayerController::RequestStopCombatAction);

	InputComponent->BindAction("Gamepad_SwitchWeaponLoadout", IE_Pressed, this, &ARPGProjectPlayerController::RequestSwapWeaponLoadout);

	//InputComponent->BindAction("Gamepad_PadLeft", IE_Pressed, this, &ARPGProjectPlayerController::);

	//InputComponent->BindAction("Gamepad_PadRight", IE_Pressed, this, &ARPGProjectPlayerController::);

	//InputComponent->BindAction("Gamepad_PadDown", IE_Pressed, this, &ARPGProjectPlayerController::);

	InputComponent->BindAction("Gamepad_Sprint", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleSprint);

	InputComponent->BindAction("Gamepad_LockOn", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleLockOn);

	//InputComponent->BindAction("Gamepad_Menu", IE_Pressed, this, &ARPGProjectPlayerController::);

	//InputComponent->BindAction("Gamepad_SpecialLeft", IE_Pressed, this, &ARPGProjectPlayerController::);


	// Keyboard

	InputComponent->BindAction("Jump", IE_Pressed, this, &ARPGProjectPlayerController::RequestJump);
	InputComponent->BindAction("Jump", IE_Released, this, &ARPGProjectPlayerController::RequestStopJumping);

	InputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerController::RequestSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerController::RequestStopSprinting);

	InputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerController::RequestHoldCrouch);
	InputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerController::RequestStopCrouching);
	InputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleCrouch);

	InputComponent->BindAction("MainhandStance", IE_Pressed, this, &ARPGProjectPlayerController::RequestMainhandStance);
	InputComponent->BindAction("MainhandStance", IE_Released, this, &ARPGProjectPlayerController::RequestStopMainhandStance);

	InputComponent->BindAction("OffhandStance", IE_Pressed, this, &ARPGProjectPlayerController::RequestOffhandStance);
	InputComponent->BindAction("OffhandStance", IE_Released, this, &ARPGProjectPlayerController::RequestStopOffhandStance);

	InputComponent->BindAction("Sheathe/Unsheathe", IE_Pressed, this, &ARPGProjectPlayerController::RequestSheatheUnsheatheWeapon);

	InputComponent->BindAction("Walk", IE_Pressed, this, &ARPGProjectPlayerController::RequestWalkMode);
	InputComponent->BindAction("Walk", IE_Released, this, &ARPGProjectPlayerController::RequestStopWalkMode);

	InputComponent->BindAction("ContextAction", IE_Pressed, this, &ARPGProjectPlayerController::RequestContextAction);
	InputComponent->BindAction("ContextAction", IE_Released, this, &ARPGProjectPlayerController::RequestStopContextAction);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ARPGProjectPlayerController::RequestInteraction);
	InputComponent->BindAction("Dodge", IE_Pressed, this, &ARPGProjectPlayerController::RequestDodge);

	InputComponent->BindAction("LightAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestLightAttack);
	InputComponent->BindAction("LightAttack", IE_Released, this, &ARPGProjectPlayerController::RequestStopLightAttack);

	InputComponent->BindAction("HeavyAttack", IE_Pressed, this, &ARPGProjectPlayerController::RequestHeavyAttack);
	InputComponent->BindAction("HeavyAttack", IE_Released, this, &ARPGProjectPlayerController::RequestStopHeavyAttack);

	InputComponent->BindAction("CombatAction", IE_Pressed, this, &ARPGProjectPlayerController::RequestCombatAction);
	InputComponent->BindAction("CombatAction", IE_Released, this, &ARPGProjectPlayerController::RequestStopCombatAction);

	InputComponent->BindAction("RadialMenu", IE_Pressed, this, &ARPGProjectPlayerController::RequestRadialMenu);
	InputComponent->BindAction("RadialMenu", IE_Released, this, &ARPGProjectPlayerController::RequestStopRadialMenu);

	InputComponent->BindAction("SwapWeaponLoadout", IE_Pressed, this, &ARPGProjectPlayerController::RequestSwapWeaponLoadout);

	InputComponent->BindAction("ToggleLockOn", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleLockOn);

	InputComponent->BindAction(FName("InteractionStart"), IE_Pressed, this, &ARPGProjectPlayerController::StartInteraction);
	InputComponent->BindAction(FName("InteractionCancel"), IE_Pressed, this, &ARPGProjectPlayerController::StopInteraction);

	// --- BIND AXIS --- //

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::RequestMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::RequestMoveRight);

	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::RequestTurnRate);
	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::RequestAddControllerYawInput);

	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::RequestLookUpRate);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::RequestAddControllerPitchInput);

	// Prevent dodge & interacting from consuming the input
	//InputComponent->GetActionBinding(0).bConsumeInput = false; // 9 or 11 (Dodge)
	//InputComponent->GetActionBinding(14).bConsumeInput = false;

	// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("InputComponent Set up successfully!"));
}

ARPGPlayerCameraManager* ARPGProjectPlayerController::GetRPGPlayerCameraManager()
{
	if (ARPGPlayerCameraManager* RPGPlayerCameraManager = Cast<ARPGPlayerCameraManager>(PlayerCameraManager))
	{
		return RPGPlayerCameraManager;
	}
	return nullptr;
}

void ARPGProjectPlayerController::UpdateGamepadInputActionBindingsMap()
{
	TArray<struct FInputActionKeyMapping> ActionMappingsArray = PlayerInput.Get()->ActionMappings;
	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::GPAM_MAX; i++)
	{
		if (ActionMappingsArray.IsValidIndex(i - 1))
		{
			TEnumAsByte<EGamePadActionMappings> GamePadActionMappings = (EGamePadActionMappings)i;
			if (GamepadInputActionBindingsMap.Contains(GamePadActionMappings))
			{
				GamepadInputActionBindingsMap[GamePadActionMappings] = ActionMappingsArray[i - 1];
			}
			else
			{
				GamepadInputActionBindingsMap.Emplace((EGamePadActionMappings)i, ActionMappingsArray[i - 1]);
			}
		}
	}
}

EInputButtonState ARPGProjectPlayerController::GetInputButtonState(TEnumAsByte<EGamePadActionMappings> ActionMapping)
{
	if (InputButtonStateMap.Contains(ActionMapping))
	{
		return InputButtonStateMap[ActionMapping];
	}
	return EInputButtonState::IBD_None;
}

void ARPGProjectPlayerController::SetInputButtonState(TEnumAsByte<EGamePadActionMappings> ActionMapping, EInputButtonState NewInputState)
{
	if (InputButtonStateMap.Contains(ActionMapping))
	{
		InputButtonStateMap[ActionMapping] = NewInputState;
	}
}

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetCharacter());

	if (PlayerCharacter)
	{
		CombatComponentRef = Cast<UCombatComponent>(PlayerCharacter->GetComponentByClass(UCombatComponent::StaticClass()));
	}

	RPGPlayerCameraManagerRef = Cast<ARPGPlayerCameraManager>(PlayerCameraManager);

}

// Called every frame
void ARPGProjectPlayerController::Tick(float DeltaTime)
{
	DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	CalculateDesiredActorRotation();

	if (PlayerCharacter)
	{
		if (!PlayerCharacter->IsAlive())
		{
			bIsMovementStopped = true;
		}

		if (PlayerCharacter->GetCurrentStamina() <= 0)
		{
			RequestStopSprinting();
		}
	}

	CheckInputToSwapLockOnTarget();

	CheckInputToJumpOrCrouch(DeltaTime);
	CheckInputForAttackFinisher(DeltaTime);

	InputUIUpdate();

	OverrideActorRotationUpdate();
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
				//RequestStopAiming();
				break;
			}
		}
	}
}

void ARPGProjectPlayerController::InputUIUpdate()
{
	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::GPAM_MAX; i++)
	{
		EGamePadActionMappings CurrentGamePadActionMapping = (EGamePadActionMappings)i;

		switch (CurrentGamePadActionMapping)
		{
		case EGamePadActionMappings::GPAM_None:
			break;

		case EGamePadActionMappings::GPAM_LightAttack:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CS_CombatReady)
				{
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::CWS_None:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_LightAttack;
						break;

					case ECombatWeaponStance::CWS_Mainhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_MainhandSkillOne;
						break;

					case ECombatWeaponStance::CWS_Offhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_OffhandSkillOne;
						break;
					}
				}
				else
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_HeavyAttack:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CS_CombatReady)
				{
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::CWS_None:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_HeavyAttack;
						break;

					case ECombatWeaponStance::CWS_Mainhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_MainhandSkillTwo;
						break;

					case ECombatWeaponStance::CWS_Offhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_OffhandSkillTwo;
						break;
					}
				}
				else
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_ContextAction:
			if (PlayerCharacter)
			{
				if (PlayerCharacter->GetVelocity().Length() > 151.f)
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Dodge;
				}
				else if (PlayerCharacter->GetIsInteractionAvailable())
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Interact;
				}
				else
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Dodge;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_JumpAndCrouch:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CS_CombatReady)
				{
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Jump;
				}
				else
				{
					if (PlayerCharacter)
					{
						if (PlayerCharacter->GetVelocity().Length() < 5.f)
						{
							if (!PlayerCharacter->GetCharacterMovement()->bWantsToCrouch)
							{
								InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Crouch;
							}
							else
							{
								InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Uncrouch;
							}
						}
						else
						{
							InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Jump;
						}
					}
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_OffhandStance:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatWeaponStance())
				{
				case ECombatWeaponStance::CWS_None:
					switch (CombatComponentRef->GetCombatState())
					{
					case ECombatState::CS_AtEase:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
						break;

					case ECombatState::CS_CombatReady:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_OffhandStance;
						break;
					}
					break;

				case ECombatWeaponStance::CWS_Mainhand:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
					break;

				case ECombatWeaponStance::CWS_Offhand:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
					break;

				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_MainhandStance:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatWeaponStance())
				{
				case ECombatWeaponStance::CWS_None:
					switch (CombatComponentRef->GetCombatState())
					{
					case ECombatState::CS_AtEase:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
						break;

					case ECombatState::CS_CombatReady:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_MainhandStance;
						break;
					}
					break;

				case ECombatWeaponStance::CWS_Mainhand:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
					break;

				case ECombatWeaponStance::CWS_Offhand:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
					break;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_SheatheAndUnsheathe:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatState())
				{
				case ECombatState::CS_AtEase:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Unsheathe;
					break;

				case ECombatState::CS_CombatReady:
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::CWS_None:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Sheathe;
						break;

					case ECombatWeaponStance::CWS_Mainhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_SecondaryCombatAction;
						break;

					case ECombatWeaponStance::CWS_Offhand:
						InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_SecondaryCombatAction;
						break;
					}
					break;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_CombatAction:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatState())
				{
				case ECombatState::CS_AtEase:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
					break;
				case ECombatState::CS_CombatReady:
					InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_CombatAction;
					break;
				}
			}
			else
			{
				InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			}
			break;

		case EGamePadActionMappings::GPAM_SwitchWeaponLoadout:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_SwitchWeaponLoadout;
			break;

		case EGamePadActionMappings::GPAM_PadLeft:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		case EGamePadActionMappings::GPAM_PadRight:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		case EGamePadActionMappings::GPAM_PadDown:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		case EGamePadActionMappings::GPAM_Sprint:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_Sprint;
			break;

		case EGamePadActionMappings::GPAM_LockOn:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		case EGamePadActionMappings::GPAM_Menu:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		case EGamePadActionMappings::GPAM_SpecialLeft:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;

		default:
			InputButtonStateMap[CurrentGamePadActionMapping] = EInputButtonState::IBD_None;
			break;
		}
	}
}

void ARPGProjectPlayerController::OverrideActorRotationUpdate()
{
	if (!RPGPlayerCameraManagerRef) { return; }
	
	if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
	{
		SetOverrideWithLockOnActorRotation(true);
	}
	else
	{
		SetOverrideWithLockOnActorRotation(false);
	}

	if (!PlayerCharacter) { return; }

	if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Sprinting)
	{
		SetOverrideWithLockOnActorRotation(false);
	}

	if (PlayerCharacter->GetPlayerActionState() == EPlayerActionState::PAS_Dodging)
	{
		SetOverrideActorRotation(true);
		SetOverrideWithLockOnActorRotation(false);
	}
	else
	{
		SetOverrideActorRotation(false);
	}


	if (!CombatComponentRef) { return; }

	if (CombatComponentRef->GetCombatState() == ECombatState::CS_AtEase)
	{
		SetOverrideWithLockOnActorRotation(false);
	}

	if (CombatComponentRef->GetIsInAttackSequence())
	{
		SetOverrideWithLockOnActorRotation(false);
	}

	if (CombatComponentRef->GetCurrentWeaponStanceType() == EWeaponStanceType::ST_Guard && bOverrideWithLockOnActorRotation)
	{
		SetOverrideActorRotation(true);
	}
}

void ARPGProjectPlayerController::CheckInputToSwapLockOnTarget()
{
	if (!RPGPlayerCameraManagerRef) { return; }

	if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
	{
		if ((FMath::Abs(YawValue) + FMath::Abs(PitchValue)) >= 5.f)
		{
			if (RPGPlayerCameraManagerRef->GetCanSwapTarget())
			{
				RPGPlayerCameraManagerRef->SwapLockOnTarget(YawValue, PitchValue);
			}
		}
		else if ((FMath::Abs(YawRate) + FMath::Abs(PitchRate)) >= .4f)
		{
			if (RPGPlayerCameraManagerRef->GetCanSwapTarget())
			{
				RPGPlayerCameraManagerRef->SwapLockOnTarget(YawRate, PitchRate);
			}
		}
		else
		{
			RPGPlayerCameraManagerRef->SetCanSwapTarget(true);
		}
	}
}

void ARPGProjectPlayerController::CheckInputToJumpOrCrouch(float DeltaTime)
{
	if (bJumpOrCrouchPressed && JumpOrCrouchHoldTime < JumpOrCrouchHoldTimeThreshold)
	{
		JumpOrCrouchHoldTime += DeltaTime;

		if (JumpOrCrouchHoldTime >= JumpOrCrouchHoldTimeThreshold)
		{
			RequestHoldJumpOrCrouch();
		}
	}
}

void ARPGProjectPlayerController::CheckInputForAttackFinisher(float DeltaTime)
{
	if (bLightAttackIsPressed && LightAttackHoldTime < LightAttackHoldTimeThreshold)
	{
		LightAttackHoldTime += DeltaTime;

		if (LightAttackHoldTime >= LightAttackHoldTimeThreshold)
		{
			RequestLightAttackFinisher();
		}
	}

	if (bHeavyAttackIsPressed && HeavyAttackHoldTime < HeavyAttackHoldTimeThreshold)
	{
		HeavyAttackHoldTime += DeltaTime;

		if (HeavyAttackHoldTime >= HeavyAttackHoldTimeThreshold)
		{
			RequestHeavyAttackFinisher();
		}
	}
}

void ARPGProjectPlayerController::CalculateDesiredActorRotation()
{
	if (bDisableRotation && !bOverrideActorRotation) { return; }

	PlayerInputRotation = { 0, (GetDesiredRotation().Yaw + UKismetMathLibrary::MakeRotFromX({ForwardValue, RightValue, 0}).Yaw), 0 };

	if (PlayerCharacter)
	{
		float TurnSpeedModifier = 0.f;
		if (CombatComponentRef)
		{
			TurnSpeedModifier = 1 - CombatComponentRef->GetRotationSpeedReductionScaleCurve();
			//CharacterTurnSpeed = CombatComponentRef->GetRotationSpeedReductionScaleCurve() < 0.09f ? MaxCharacterTurnSpeed : MaxCharacterTurnSpeed * CombatComponentRef->GetRotationSpeedReductionScaleCurve();
			//CharacterTurnSpeed = TurnSpeedModifier >= 1.f ? MaxCharacterTurnSpeed * -1 : MaxCharacterTurnSpeed * TurnSpeedModifier;
		}
		else
		{
			TurnSpeedModifier = 1.f;
		}

		if (TurnSpeedModifier > 0.f)
		{
			/*if (RPGPlayerCameraManagerRef)
			{
				if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
				{
					DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, PlayerCharacter->GetActorRotation().Yaw, 0 }, { 0, GetControlRotation().Yaw, 0 }, DeltaSeconds, (CharacterTurnSpeed * TurnSpeedModifier));
					PlayerCharacter->SetActorRotation(DesiredActorRotation);

					return;
				}
			}*/

			if (bOverrideWithLockOnActorRotation)
			{
				DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, PlayerCharacter->GetActorRotation().Yaw, 0 }, { 0, GetControlRotation().Yaw, 0 }, DeltaSeconds, (CharacterTurnSpeed * TurnSpeedModifier));
				PlayerCharacter->SetActorRotation(DesiredActorRotation);
			}
			else
			{
				DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, PlayerCharacter->GetActorRotation().Yaw, 0 }, PlayerInputRotation, DeltaSeconds, (CharacterTurnSpeed * TurnSpeedModifier));
			}
		}
	}
}

//--------------------------------------------------------------
// Action Mappings
//--------------------------------------------------------------

void ARPGProjectPlayerController::RequestJumpOrCrouch()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetVelocity().Length() < 5.f)
		{
			bJumpOrCrouchPressed = true;
		}
		else
		{
			bJumpOrCrouchPressed = false;
			PlayerCharacter->RequestJump();
		}
	}
}

void ARPGProjectPlayerController::RequestHoldJumpOrCrouch()
{
	RequestToggleCrouch();
}

void ARPGProjectPlayerController::RequestStopJumpOrCrouch()
{
	if (!bJumpOrCrouchPressed) { return; }

	if (JumpOrCrouchHoldTime < (JumpOrCrouchHoldTimeThreshold * .5f))
	{
		if (PlayerCharacter)
		{
			PlayerCharacter->RequestJump();
		}
	}
	if (bJumpOrCrouchPressed)
	{
		JumpOrCrouchHoldTime = 0.f;
		bJumpOrCrouchPressed = false;
	}
}

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

void ARPGProjectPlayerController::RequestToggleSprint()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Sprinting)
		{
			RequestStopSprinting();
		}
		else
		{
			RequestSprint();
		}
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

void ARPGProjectPlayerController::RequestMainhandStance()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestMainhandStance();
	}
}

void ARPGProjectPlayerController::RequestStopMainhandStance()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopMainhandStance();
	}
}

void ARPGProjectPlayerController::RequestOffhandStance()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestOffhandStance();
	}
}

void ARPGProjectPlayerController::RequestStopOffhandStance()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopOffhandStance();
	}
}

void ARPGProjectPlayerController::RequestSheatheUnsheatheWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestSheatheUnsheatheWeapon();
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

void ARPGProjectPlayerController::RequestContextAction()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		if (abs(ForwardValue) + abs(RightValue) > 0.33f) { PlayerCharacter->RequestDodge(); }
		else { PlayerCharacter->RequestInteraction(); }
		//PlayerCharacter->RequestContextAction();
	}
	bContextActionIsPressed = true;
}

void ARPGProjectPlayerController::RequestHoldContextAction()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHoldContextAction();
	}
}

void ARPGProjectPlayerController::RequestStopContextAction()
{
	if (bIsInMenu) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopContextAction(ContextActionHoldTime >= ContextActionSprintThreshold);
	}
	ContextActionHoldTime = 0.f;
	bContextActionIsPressed = false;
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
	bLightAttackIsPressed = true;

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestLightAttack();
	}
}

void ARPGProjectPlayerController::RequestStopLightAttack()
{
	bLightAttackIsPressed = false;

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopLightAttack();
	}

	LightAttackHoldTime = 0.f;
}

void ARPGProjectPlayerController::RequestHeavyAttack()
{
	bHeavyAttackIsPressed = true;

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHeavyAttack();
	}
}

void ARPGProjectPlayerController::RequestStopHeavyAttack()
{
	bHeavyAttackIsPressed = false;

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopHeavyAttack();
	}

	HeavyAttackHoldTime = 0.f;
}

void ARPGProjectPlayerController::RequestLightAttackFinisher()
{
	RequestStopLightAttack();
	RequestStopHeavyAttack();

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestLightAttackFinisher();
	}
}

void ARPGProjectPlayerController::RequestHeavyAttackFinisher()
{
	/*bLightAttackIsPressed = false;
	LightAttackHoldTime = 0.f;
	bHeavyAttackIsPressed = false;
	HeavyAttackHoldTime = 0.f;*/
	RequestStopLightAttack();
	RequestStopHeavyAttack();

	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHeavyAttackFinisher();
	}
}

void ARPGProjectPlayerController::RequestCombatAction()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestCombatAction();
	}
}

void ARPGProjectPlayerController::RequestStopCombatAction()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopCombatAction();
	}
}

void ARPGProjectPlayerController::RequestRadialMenu()
{

}

void ARPGProjectPlayerController::RequestStopRadialMenu()
{

}

void ARPGProjectPlayerController::RequestSwapWeaponLoadout()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestSwapWeaponLoadout();
	}
}

void ARPGProjectPlayerController::RequestToggleLockOn()
{
	if (ARPGPlayerCameraManager* RPGPlayerCameraManager = Cast<ARPGPlayerCameraManager>(PlayerCameraManager))
	{
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::CV_LockOn)
		{
			RPGPlayerCameraManager->DisableLockOn();
		}
		else
		{
			RPGPlayerCameraManager->EnableLockOn();
		}
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
	else if ((ForwardValue + RightValue) == 0.f)
	{
		RequestStopSprinting();
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

	YawRate = Rate;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
		{
			return; 
		}
	}


	AddYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerYawInput(float Value)
{
	if (bIsInMenu) { return; }

	YawValue = Value;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
		{
			return;
		}
	}

	AddYawInput(Value);
}

void ARPGProjectPlayerController::RequestLookUpRate(float Rate)
{
	if (bIsInMenu) { return; }

	PitchRate = Rate;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
		{
			return;
		}
	}

	AddPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerPitchInput(float Value)
{
	if (bIsInMenu) { return; }

	PitchValue = Value;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::CV_LockOn)
		{
			return;
		}
	}

	AddPitchInput(Value);
}