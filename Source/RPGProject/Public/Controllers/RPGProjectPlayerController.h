// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/InputActionDelegateBinding.h"
#include "RPGProjectPlayerController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInteractionStart);
DECLARE_MULTICAST_DELEGATE(FOnInteractionCancel);

class ARPGProjectPlayerCharacter;
class ARPGPlayerCameraManager;
class UCombatComponent;

UENUM(BlueprintType)
enum class EGamePadActionMappings : uint8
{
	GPAM_None					UMETA(Hidden, DisplayName = "None"),
	GPAM_LightAttack			UMETA(DisplayName = "Light Attack"),
	GPAM_HeavyAttack			UMETA(DisplayName = "Heavy Attack"),
	GPAM_ContextAction			UMETA(DisplayName = "Context Action"),
	GPAM_JumpAndCrouch			UMETA(DisplayName = "Jump / Crouch"),
	GPAM_OffhandStance			UMETA(DisplayName = "Offhand Stance / Radial Menu"),
	GPAM_MainhandStance			UMETA(DisplayName = "Mainhand Stance"),
	GPAM_SheatheAndUnsheathe	UMETA(DisplayName = "Sheathe / Unsheathe / 2nd Combat Action"),
	GPAM_CombatAction			UMETA(DisplayName = "Combat Action"),
	GPAM_SwitchWeaponLoadout	UMETA(DisplayName = "Switch Weapon Layout"),
	GPAM_PadLeft				UMETA(DisplayName = "DPad Left"),
	GPAM_PadRight				UMETA(DisplayName = "DPad Right"),
	GPAM_PadDown				UMETA(DisplayName = "DPad Down"),
	GPAM_Sprint					UMETA(DisplayName = "Sprint"),
	GPAM_LockOn					UMETA(DisplayName = "Lock-On"),
	GPAM_Menu					UMETA(DisplayName = "Menu"),
	GPAM_SpecialLeft			UMETA(DisplayName = "Special Left"),

	GPAM_MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EInputButtonState : uint8
{
	IBD_None						UMETA(DisplayName = "None"),
	IBD_Unsheathe					UMETA(DisplayName = "Unsheathe"),
	IBD_Sheathe						UMETA(DisplayName = "Sheathe"),
	IBD_Interact					UMETA(DisplayName = "Interact"),
	IBD_Dodge						UMETA(DisplayName = "Dodge"),
	IBD_Jump						UMETA(DisplayName = "Jump"),
	IBD_Sprint						UMETA(DisplayName = "Sprint"),
	IBD_Crouch						UMETA(DisplayName = "Crouch"),
	IBD_Uncrouch					UMETA(DisplayName = "Uncrouch"),
	IBD_LightAttack					UMETA(DisplayName = "Light Attack"),
	IBD_HeavyAttack					UMETA(DisplayName = "Heavy Attack"),
	IBD_OffhandStance				UMETA(DisplayName = "Offhand Stance"),
	IBD_MainhandStance				UMETA(DisplayName = "Mainhand Stance"),
	IBD_OffhandSkillOne				UMETA(DisplayName = "Offhand Skill #1"),
	IBD_OffhandSkillTwo				UMETA(DisplayName = "Offhand Skill #2"),
	IBD_MainhandSkillOne			UMETA(DisplayName = "Mainhand Skill #1"),
	IBD_MainhandSkillTwo			UMETA(DisplayName = "Mainhand Skill #2"),
	IBD_CombatAction				UMETA(DisplayName = "Combat Action"),
	IBD_SecondaryCombatAction		UMETA(DisplayName = "Secondary Combat Action"),
	IBD_ClassAction					UMETA(DisplayName = "Class Action"),
	IBD_SwitchWeaponLoadout			UMETA(DisplayName = "Switch Weapon Loadout"),
	IBD_GamePadLeft					UMETA(DisplayName = ""),
	IBD_GamePadRight				UMETA(DisplayName = ""),
	IBD_GamePadDown					UMETA(DisplayName = ""),

	IBD_MAX							UMETA(Hidden)
};


UCLASS()
class RPGPROJECT_API ARPGProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ARPGProjectPlayerController();

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetIsInMenu(bool bActive) { bIsInMenu = bActive; }
	UFUNCTION(BlueprintCallable)
	bool GetIsInMenu() { return bIsInMenu; }

	UFUNCTION(BlueprintImplementableEvent)
	void BPCreateNotification(UItemData* ItemData);

	UFUNCTION(BlueprintCallable)
	ARPGPlayerCameraManager* GetRPGPlayerCameraManager();

	UFUNCTION(BlueprintPure)
	TMap<EGamePadActionMappings, FInputActionKeyMapping> GetGamepadInputActionBindingsMap() { return GamepadInputActionBindingsMap; }
	UFUNCTION(BlueprintCallable)
	void UpdateGamepadInputActionBindingsMap();

	UFUNCTION(BlueprintPure)
	TMap<EGamePadActionMappings, EInputButtonState> GetInputButtonStateMap() { return InputButtonStateMap; }
	UFUNCTION(BlueprintCallable)
	void SetInputButtonState(TEnumAsByte<EGamePadActionMappings> ActionMapping, EInputButtonState NewInputState);
	UFUNCTION(BlueprintPure)
	EInputButtonState GetInputButtonState(TEnumAsByte<EGamePadActionMappings> ActionMapping);

	UFUNCTION(BlueprintPure)
	float GetJumpOrCrouchHoldTime() { return JumpOrCrouchHoldTime; }
	UFUNCTION(BlueprintPure)
	float GetJumpOrCrouchHoldTimeThreshold() { return JumpOrCrouchHoldTimeThreshold; }

	UFUNCTION(BlueprintCallable)
	void DisableCharacterRotation(bool bActive) { bDisableRotation = bActive; }

	
	UFUNCTION(BlueprintPure)
	bool GetOverrideActorRotation() { return bOverrideActorRotation; }
	UFUNCTION(BlueprintCallable)
	void SetOverrideActorRotation(bool bActive) { bOverrideActorRotation = bActive; }

	UFUNCTION(BlueprintPure)
	bool GetOverrideWithLockOnActorRotation() { return bOverrideWithLockOnActorRotation; }
	UFUNCTION(BlueprintCallable)
	void SetOverrideWithLockOnActorRotation(bool bActive) { bOverrideWithLockOnActorRotation = bActive; }


public:

	FOnInteractionStart OnInteractionStart;
	FOnInteractionCancel OnInteractionCancel;

protected:

	virtual void BeginPlay() override;

	void CombatModeUpdate();
	void InputUIUpdate();
	void OverrideActorRotationUpdate();
	void CheckInputToSwapLockOnTarget();
	void CheckInputToJumpOrCrouch(float DeltaTime);
	void CheckInputForAttackFinisher(float DeltaTime);

	// --- INPUT FUNCTIONS --- //

	void RequestJumpOrCrouch();
	void RequestHoldJumpOrCrouch();
	void RequestStopJumpOrCrouch();

	void RequestJump();
	void RequestStopJumping();

	void RequestToggleSprint();
	void RequestSprint();
	void RequestStopSprinting();

	void RequestHoldCrouch();
	void RequestStopCrouching();
	void RequestToggleCrouch();

	void RequestMainhandStance();
	void RequestStopMainhandStance();

	void RequestOffhandStance();
	void RequestStopOffhandStance();

	void RequestSheatheUnsheatheWeapon();

	void RequestWalkMode();
	void RequestStopWalkMode();

	void RequestContextAction();
	void RequestHoldContextAction();
	void RequestStopContextAction();

	void RequestInteraction();
	void RequestDodge();

	void RequestLightAttack();
	void RequestStopLightAttack();

	void RequestHeavyAttack();
	void RequestStopHeavyAttack();

	void RequestLightAttackFinisher();
	void RequestHeavyAttackFinisher();

	void RequestCombatAction();
	void RequestStopCombatAction();

	void RequestRadialMenu();
	void RequestStopRadialMenu();

	void RequestSwapWeaponLoadout();

	void RequestToggleLockOn();

	void StartInteraction();
	void StopInteraction();

	void RequestMoveForward(float Value);
	void RequestMoveRight(float Value);
	void RequestTurnRate(float Rate);
	void RequestAddControllerYawInput(float Value);
	void RequestLookUpRate(float Rate);
	void RequestAddControllerPitchInput(float Value);

	// --- --- //

	void CalculateDesiredActorRotation();

protected:

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CapsuleCrouchHeight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator DesiredActorRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	FRotator PlayerInputRotation = FRotator::ZeroRotator;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float MaxCharacterTurnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement")
	float CharacterTurnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float ForwardValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement")
	float RightValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement")
	float PitchValue = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement")
	float PitchRate = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement")
	float YawValue = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Movement")
	float YawRate = 0.f;

	bool bJumpOrCrouchPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Tracking - Jump / Crouch")
	float JumpOrCrouchHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Tracking - Jump / Crouch")
	float JumpOrCrouchHoldTimeThreshold = .5f;

	bool bLightAttackIsPressed = false;
	bool bHeavyAttackIsPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Tracking - Attacking")
	float LightAttackHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Tracking - Attacking")
	float LightAttackHoldTimeThreshold = .5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Tracking - Attacking")
	float HeavyAttackHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Tracking - Attacking")
	float HeavyAttackHoldTimeThreshold = .5f;

	bool bContextActionIsPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Tracking - Context Action")
	float ContextActionHoldTime = .0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input Tracking - Context Action")
	float ContextActionSprintThreshold= .25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CharacterMovement")
	bool bIsMovementStopped;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "UI Controls")
	bool bIsInMenu;

	ARPGProjectPlayerCharacter* PlayerCharacter;

	UCombatComponent* CombatComponentRef;

	ARPGPlayerCameraManager* RPGPlayerCameraManagerRef;

	FTimerHandle SprintStaminaDrainTimerHandle;

	FTimerHandle StaminaRegenTimerHandle;

	float DeltaSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Button States")
	TMap<EGamePadActionMappings, FInputActionKeyMapping> GamepadInputActionBindingsMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input Button States")
	TMap<EGamePadActionMappings, EInputButtonState> InputButtonStateMap;

	bool bDisableRotation = false;
	bool bOverrideActorRotation = false;
	bool bOverrideWithLockOnActorRotation = false;

};