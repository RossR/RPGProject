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
#include "Actors/Components/StaminaComponent.h"
#include "Actors/Components/HealthComponent.h"

ARPGProjectPlayerController::ARPGProjectPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	bIsMovementStopped = false;
	bIsPlayerInMenu = false;

	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::MAX; i++)
	{
		MappedActionStates.Add((EGamePadActionMappings)i);
	}

	InputAxisProperties.Add(EKeys::Mouse2D.GetFName());
	InputAxisProperties.Add(EKeys::Gamepad_RightX.GetFName());
	InputAxisProperties.Add(EKeys::Gamepad_RightY.GetFName());

	PlayerCameraManagerClass = ARPGPlayerCameraManager::StaticClass();
}

void ARPGProjectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UpdateInputActionGamepadMappings();

	//------------------
	// General Bindings
	//------------------

	InputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerController::RequestMoveForward);
	InputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerController::RequestMoveRight);

	//------------------
	// Gamepad Bindings
	//------------------

	// Axis
	//------------------

	InputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerController::RequestTurnRate);
	InputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerController::RequestLookUpRate);

	// Action
	//------------------

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

	//------------------
	// Keyboard Bindings
	//------------------

	// Axis
	//------------------

	InputComponent->BindAxis("Turn", this, &ARPGProjectPlayerController::RequestAddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &ARPGProjectPlayerController::RequestAddControllerPitchInput);

	// Action
	//------------------

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

	//InputComponent->BindAction("RadialMenu", IE_Pressed, this, &ARPGProjectPlayerController::RequestRadialMenu);
	//InputComponent->BindAction("RadialMenu", IE_Released, this, &ARPGProjectPlayerController::RequestStopRadialMenu);

	InputComponent->BindAction("SwapWeaponLoadout", IE_Pressed, this, &ARPGProjectPlayerController::RequestSwapWeaponLoadout);

	InputComponent->BindAction("ToggleLockOn", IE_Pressed, this, &ARPGProjectPlayerController::RequestToggleLockOn);
}

void ARPGProjectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetCharacter());

	CombatComponentRef = Cast<UCombatComponent>(GetPawn()->GetComponentByClass(UCombatComponent::StaticClass()));

	RPGPlayerCameraManagerRef = Cast<ARPGPlayerCameraManager>(PlayerCameraManager);

}

// Called every frame
void ARPGProjectPlayerController::Tick(float DeltaTime)
{

	CalculateDesiredPawnRotation();

	if (PlayerCharacter)
	{
		if (UHealthComponent* CharacterHealthComponentRef = Cast<UHealthComponent>(PlayerCharacter->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (CharacterHealthComponentRef->IsDead()) { bIsMovementStopped = true; }
		}

		if (UStaminaComponent* CharacterStaminaComponentRef = Cast<UStaminaComponent>(PlayerCharacter->GetComponentByClass(UStaminaComponent::StaticClass())))
		{
			if (CharacterStaminaComponentRef->IsStaminaExhausted()) { RequestStopSprinting(); }
		}
	}

	CheckInputToSwapLockOnTarget();

	CheckInputToJumpOrCrouch(DeltaTime);
	CheckInputForAttackFinisher(DeltaTime);

	InputButtonStateUpdate();

	UpdatePawnRotationBooleans();
}

ARPGPlayerCameraManager* ARPGProjectPlayerController::GetRPGPlayerCameraManager()
{
	if (ARPGPlayerCameraManager* RPGPlayerCameraManager = Cast<ARPGPlayerCameraManager>(PlayerCameraManager))
	{
		return RPGPlayerCameraManager;
	}
	return nullptr;
}

void ARPGProjectPlayerController::UpdateInputActionGamepadMappings()
{
	// Update InputActionGamepadMappings
	TArray<struct FInputActionKeyMapping> ActionMappingsArray = PlayerInput.Get()->ActionMappings;
	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::MAX; i++)
	{
		if (ActionMappingsArray.IsValidIndex(i - 1))
		{
			TEnumAsByte<EGamePadActionMappings> GamePadActionMappings = (EGamePadActionMappings)i;
			if (InputActionGamepadMappings.Contains(GamePadActionMappings))
			{
				InputActionGamepadMappings[GamePadActionMappings] = ActionMappingsArray[i - 1];
			}
			else
			{
				InputActionGamepadMappings.Emplace((EGamePadActionMappings)i, ActionMappingsArray[i - 1]);
			}
		}
	}

	// Update InputAxisProperties
	TArray<FName> InputAxisPropertiesKeyArray;
	InputAxisProperties.GenerateKeyArray(InputAxisPropertiesKeyArray);
	for (size_t i = 0; i < InputAxisPropertiesKeyArray.Num(); i++)
	{
		PlayerInput->GetAxisProperties(InputAxisPropertiesKeyArray[i], InputAxisProperties[InputAxisPropertiesKeyArray[i]]);
	}
}

EInputButtonState ARPGProjectPlayerController::GetMappedActionState(TEnumAsByte<EGamePadActionMappings> ActionMapping)
{
	if (MappedActionStates.Contains(ActionMapping))
	{
		return MappedActionStates[ActionMapping];
	}
	return EInputButtonState::None;
}

void ARPGProjectPlayerController::SetMappedActionState(TEnumAsByte<EGamePadActionMappings> ActionMapping, EInputButtonState NewInputState)
{
	if (MappedActionStates.Contains(ActionMapping))
	{
		MappedActionStates[ActionMapping] = NewInputState;
	}
}

void ARPGProjectPlayerController::InputButtonStateUpdate()
{
	for (uint8 i = 1; i < (uint8)EGamePadActionMappings::MAX; i++)
	{
		if (!MappedActionStates.Contains((EGamePadActionMappings)i)) { continue; }

		EGamePadActionMappings CurrentGamePadActionMapping = (EGamePadActionMappings)i;

		switch (CurrentGamePadActionMapping)
		{
		case EGamePadActionMappings::None:
			break;

		case EGamePadActionMappings::LightAttack:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CombatReady)
				{
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::None:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::LightAttack;
						break;

					case ECombatWeaponStance::Mainhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::MainhandSkillOne;
						break;

					case ECombatWeaponStance::Offhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::OffhandSkillOne;
						break;
					}
				}
				else
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::HeavyAttack:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CombatReady)
				{
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::None:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::HeavyAttack;
						break;

					case ECombatWeaponStance::Mainhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::MainhandSkillTwo;
						break;

					case ECombatWeaponStance::Offhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::OffhandSkillTwo;
						break;
					}
				}
				else
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::ContextAction:
			if (PlayerCharacter)
			{
				if (PlayerCharacter->GetVelocity().Length() > 151.f)
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Dodge;
				}
				else if (PlayerCharacter->GetIsInteractionAvailable())
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Interact;
				}
				else
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Dodge;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::JumpAndCrouch:
			if (CombatComponentRef)
			{
				if (CombatComponentRef->GetCombatState() == ECombatState::CombatReady)
				{
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Jump;
				}
				else
				{
					if (PlayerCharacter)
					{
						if (PlayerCharacter->GetVelocity().Length() < 5.f)
						{
							if (!PlayerCharacter->GetCharacterMovement()->bWantsToCrouch)
							{
								MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Crouch;
							}
							else
							{
								MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Uncrouch;
							}
						}
						else
						{
							MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Jump;
						}
					}
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::OffhandStance:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatWeaponStance())
				{
				case ECombatWeaponStance::None:
					switch (CombatComponentRef->GetCombatState())
					{
					case ECombatState::AtEase:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
						break;

					case ECombatState::CombatReady:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::OffhandStance;
						break;
					}
					break;

				case ECombatWeaponStance::Mainhand:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
					break;

				case ECombatWeaponStance::Offhand:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
					break;

				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::MainhandStance:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatWeaponStance())
				{
				case ECombatWeaponStance::None:
					switch (CombatComponentRef->GetCombatState())
					{
					case ECombatState::AtEase:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
						break;

					case ECombatState::CombatReady:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::MainhandStance;
						break;
					}
					break;

				case ECombatWeaponStance::Mainhand:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
					break;

				case ECombatWeaponStance::Offhand:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
					break;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::SheatheAndUnsheathe:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatState())
				{
				case ECombatState::AtEase:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Unsheathe;
					break;

				case ECombatState::CombatReady:
					switch (CombatComponentRef->GetCombatWeaponStance())
					{
					case ECombatWeaponStance::None:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Sheathe;
						break;

					case ECombatWeaponStance::Mainhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::SecondaryCombatAction;
						break;

					case ECombatWeaponStance::Offhand:
						MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::SecondaryCombatAction;
						break;
					}
					break;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

		case EGamePadActionMappings::CombatAction:
			if (CombatComponentRef)
			{
				switch (CombatComponentRef->GetCombatState())
				{
				case ECombatState::AtEase:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
					break;
				case ECombatState::CombatReady:
					MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::CombatAction;
					break;
				}
			}
			else
			{
				MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			}
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::SwitchWeaponLoadout:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::SwitchWeaponLoadout;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::PadLeft:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::PadRight:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::PadDown:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::Sprint:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::Sprint;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::LockOn:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::Menu:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

			// Currently not used in the UI
		case EGamePadActionMappings::SpecialLeft:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;

		default:
			MappedActionStates[CurrentGamePadActionMapping] = EInputButtonState::None;
			break;
		}
	}
}

void ARPGProjectPlayerController::UpdatePawnRotationBooleans()
{
	if (!RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
		{
			OverridePawnRotationForLockOn(true);
		}
		else { OverridePawnRotationForLockOn(false); }
	}

	if (!PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::Sprinting)
		{
			OverridePawnRotationForLockOn(false);
		}

		if (PlayerCharacter->GetPlayerActionState() == EPlayerActionState::Dodging)
		{
			OverridePawnRotation(true);
			OverridePawnRotationForLockOn(false);
		}
		else { OverridePawnRotation(false); }
	}

	if (!CombatComponentRef)
	{
		if (CombatComponentRef->GetCombatState() == ECombatState::AtEase ||
			CombatComponentRef->GetIsInAttackSequence())
		{
			OverridePawnRotationForLockOn(false);
		}

		if (CombatComponentRef->GetCurrentWeaponStanceType() == EWeaponStanceType::ST_Guard && GetOverridePawnRotationForLockOn())
		{
			OverridePawnRotation(true);
		}
	}
}

void ARPGProjectPlayerController::CalculateDesiredPawnRotation()
{
	if (IsPawnRotationDisabled() && !GetOverridePawnRotation()) { return; }

	// Get the direction that the player wants the possessed pawn to move towards
	PlayerInputRotation = { 0, (GetDesiredRotation().Yaw + UKismetMathLibrary::MakeRotFromX({MoveForwardValue, MoveRightValue, 0}).Yaw), 0 };

	if (GetPawn())
	{
		// Get the possessed pawn's rotation speed (determined by the anim curve RotationSpeedReductionScale)

		float TurnSpeedModifier = 0.f;
		if (CombatComponentRef)
		{
			TurnSpeedModifier = 1 - CombatComponentRef->GetRotationSpeedReductionScaleCurve();
		}
		else
		{
			TurnSpeedModifier = 1.f;
		}

		// Do not rotate the possessed pawn if they are unable to rotate currently
		if (TurnSpeedModifier > 0.f)
		{
			// Make the possessed pawn look towards the lock-on target
			if (bOverridePawnRotationForLockOn)
			{
				DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, GetPawn()->GetActorRotation().Yaw, 0 }, { 0, GetControlRotation().Yaw, 0 }, GetWorld()->GetDeltaSeconds(), (PawnTurnSpeed * TurnSpeedModifier));
				GetPawn()->SetActorRotation(DesiredActorRotation);
			}
			// Make the possessed pawn look in the direction of the player's input
			else
			{
				DesiredActorRotation = UKismetMathLibrary::RInterpTo({ 0, GetPawn()->GetActorRotation().Yaw, 0 }, PlayerInputRotation, GetWorld()->GetDeltaSeconds(), (PawnTurnSpeed * TurnSpeedModifier));
			}
		}
	}
}

void ARPGProjectPlayerController::CheckInputToSwapLockOnTarget()
{
	if (!RPGPlayerCameraManagerRef) { return; }

	float SensitivityModifier = 1.f;

	// Get the sensitivity of the mouse input
	if (InputAxisProperties.Find(EKeys::Mouse2D.GetFName())) { SensitivityModifier = InputAxisProperties[EKeys::Mouse2D.GetFName()].Sensitivity; }

	if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
	{
		// Swap Lock-on target if the mouse input is greater than the mouse input threshold
		if ((FMath::Abs(MouseYawValue) + FMath::Abs(MousePitchValue)) >= (SwapLockOnTargetMouseThreshold / SensitivityModifier))
		{
			if (RPGPlayerCameraManagerRef->GetCanSwapTarget())
			{
				RPGPlayerCameraManagerRef->SwapLockOnTarget(MouseYawValue, MousePitchValue);
			}
		}
		// Swap Lock-on target if the gamepade input is greater than the gamepade input threshold
		else if ((FMath::Abs(NormalisedGamepadYawRate) + FMath::Abs(NormalisedGamepadPitchRate)) >= SwapLockOnTargetGamepadThreshold)
		{
			if (RPGPlayerCameraManagerRef->GetCanSwapTarget())
			{
				RPGPlayerCameraManagerRef->SwapLockOnTarget(NormalisedGamepadYawRate, NormalisedGamepadPitchRate);
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
			RequestToggleCrouch();
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

void ARPGProjectPlayerController::RequestMoveForward(float Value)
{
	MoveForwardValue = Value;

	if (Value != 0 && GetPawn() && !bIsMovementStopped)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		GetPawn()->AddMovementInput(Direction, Value);

		GetPawn()->SetActorRotation(DesiredActorRotation);
	}
	else if ((MoveForwardValue + MoveRightValue) == 0.f)
	{
		RequestStopSprinting();
	}
}

void ARPGProjectPlayerController::RequestMoveRight(float Value)
{
	MoveRightValue = Value;

	if (Value != 0 && GetPawn() && !bIsMovementStopped)
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		GetPawn()->AddMovementInput(Direction, Value);

		GetPawn()->SetActorRotation(DesiredActorRotation);
	}
}

void ARPGProjectPlayerController::RequestTurnRate(float Rate)
{
	if (IsPlayerInMenu()) { return; }

	GamepadYawRate = Rate;

	if (InputAxisProperties.Find(EKeys::Gamepad_RightX.GetFName()))
	{
		NormalisedGamepadYawRate = Rate / InputAxisProperties[EKeys::Gamepad_RightX.GetFName()].Sensitivity;;
	}
	else { NormalisedGamepadYawRate = Rate; }


	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
		{
			return;
		}
	}


	AddYawInput(Rate * GamepadBaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerYawInput(float Value)
{
	if (IsPlayerInMenu()) { return; }

	MouseYawValue = Value;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
		{
			return;
		}
	}

	AddYawInput(Value);
}

void ARPGProjectPlayerController::RequestLookUpRate(float Rate)
{
	if (IsPlayerInMenu()) { return; }

	GamepadPitchRate = Rate;

	if (InputAxisProperties.Find(EKeys::Gamepad_RightY.GetFName()))
	{
		NormalisedGamepadPitchRate = Rate / InputAxisProperties[EKeys::Gamepad_RightY.GetFName()].Sensitivity;
	}
	else { NormalisedGamepadPitchRate = Rate; }

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
		{
			return;
		}
	}

	AddPitchInput(Rate * GamepadBaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ARPGProjectPlayerController::RequestAddControllerPitchInput(float Value)
{
	if (IsPlayerInMenu()) { return; }

	MousePitchValue = Value;

	if (RPGPlayerCameraManagerRef)
	{
		if (RPGPlayerCameraManagerRef->GetCameraView() == ECameraView::LockOn)
		{
			return;
		}
	}

	AddPitchInput(Value);
}

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

void ARPGProjectPlayerController::RequestStopJumpOrCrouch()
{
	if (!bJumpOrCrouchPressed) { return; }

	if (PlayerCharacter)
	{
		if (!PlayerCharacter->GetCharacterMovement()->IsMovingOnGround())
		{
			PlayerCharacter->RequestStopJumping();
		}
	}

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
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::Sprinting)
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
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestSprint();
	}
}

void ARPGProjectPlayerController::RequestStopSprinting()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopSprinting();
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
	if (PlayerCharacter)
	{
		PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::Walking);
	}
}

void ARPGProjectPlayerController::RequestStopWalkMode()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::Walking)
		{
			PlayerCharacter->SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::Jogging);
		}
	}
}

void ARPGProjectPlayerController::RequestContextAction()
{
	if (IsPlayerInMenu()) { return; }
	if (PlayerCharacter)
	{
		// Request for the possessed character to dodge if the player's movement input goes above the dodge threshold
		if (abs(MoveForwardValue) + abs(MoveRightValue) > ContextActionDodgeThreshold) { PlayerCharacter->RequestDodge(); }
		// Otherwise request to interact
		else { PlayerCharacter->RequestInteraction(); }
	}
	bContextActionIsPressed = true;
}

void ARPGProjectPlayerController::RequestHoldContextAction()
{
	if (IsPlayerInMenu()) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestHoldContextAction();
	}
}

void ARPGProjectPlayerController::RequestStopContextAction()
{
	if (IsPlayerInMenu()) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestStopContextAction();
	}
	ContextActionHoldTime = 0.f;
	bContextActionIsPressed = false;
}

void ARPGProjectPlayerController::RequestInteraction()
{
	if (IsPlayerInMenu()) { return; }
	if (PlayerCharacter)
	{
		PlayerCharacter->RequestInteraction();
	}
}

void ARPGProjectPlayerController::RequestDodge()
{
	if (IsPlayerInMenu()) { return; }
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
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::LockOn)
		{
			RPGPlayerCameraManager->DisableLockOn();
		}
		else
		{
			RPGPlayerCameraManager->EnableLockOn();
		}
	}
}