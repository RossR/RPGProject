// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/InputActionDelegateBinding.h"
#include "RPGProjectPlayerController.generated.h"

class ARPGProjectPlayerCharacter;
class ARPGPlayerCameraManager;
class UCombatComponent;


UENUM(BlueprintType)
// The input actions used by a gamepad
enum class EGamePadActionMappings : uint8
{
	None						UMETA(Hidden, DisplayName = "None"),
	LightAttack					UMETA(DisplayName = "Light Attack"),
	HeavyAttack					UMETA(DisplayName = "Heavy Attack"),
	ContextAction				UMETA(DisplayName = "Context Action"),
	JumpAndCrouch				UMETA(DisplayName = "Jump / Crouch"),
	OffhandStance				UMETA(DisplayName = "Offhand Stance / Radial Menu"),
	MainhandStance				UMETA(DisplayName = "Mainhand Stance"),
	SheatheAndUnsheathe			UMETA(DisplayName = "Sheathe / Unsheathe / 2nd Combat Action"),
	CombatAction				UMETA(DisplayName = "Combat Action"),
	SwitchWeaponLoadout			UMETA(DisplayName = "Switch Weapon Layout"),
	PadLeft						UMETA(DisplayName = "DPad Left"),
	PadRight					UMETA(DisplayName = "DPad Right"),
	PadDown						UMETA(DisplayName = "DPad Down"),
	Sprint						UMETA(DisplayName = "Sprint"),
	LockOn						UMETA(DisplayName = "Lock-On"),
	Menu						UMETA(DisplayName = "Menu"),
	SpecialLeft					UMETA(DisplayName = "Special Left"),

	MAX							UMETA(Hidden)
};

UENUM(BlueprintType)
// A list of states for input actions
enum class EInputButtonState : uint8
{
	None						UMETA(DisplayName = "None"),
	Unsheathe					UMETA(DisplayName = "Unsheathe"),
	Sheathe						UMETA(DisplayName = "Sheathe"),
	Interact					UMETA(DisplayName = "Interact"),
	Dodge						UMETA(DisplayName = "Dodge"),
	Jump						UMETA(DisplayName = "Jump"),
	Sprint						UMETA(DisplayName = "Sprint"),
	Crouch						UMETA(DisplayName = "Crouch"),
	Uncrouch					UMETA(DisplayName = "Uncrouch"),
	LightAttack					UMETA(DisplayName = "Light Attack"),
	HeavyAttack					UMETA(DisplayName = "Heavy Attack"),
	OffhandStance				UMETA(DisplayName = "Offhand Stance"),
	MainhandStance				UMETA(DisplayName = "Mainhand Stance"),
	OffhandSkillOne				UMETA(DisplayName = "Offhand Skill #1"),
	OffhandSkillTwo				UMETA(DisplayName = "Offhand Skill #2"),
	MainhandSkillOne			UMETA(DisplayName = "Mainhand Skill #1"),
	MainhandSkillTwo			UMETA(DisplayName = "Mainhand Skill #2"),
	CombatAction				UMETA(DisplayName = "Combat Action"),
	SecondaryCombatAction		UMETA(DisplayName = "Secondary Combat Action"),
	ClassAction					UMETA(DisplayName = "Class Action"),
	SwitchWeaponLoadout			UMETA(DisplayName = "Switch Weapon Loadout"),
	GamePadLeft					UMETA(DisplayName = "Game Pad Left"),
	GamePadRight				UMETA(DisplayName = "Game Pad Right"),
	GamePadDown					UMETA(DisplayName = "Game Pad Down"),

	MAX							UMETA(Hidden)
};


UCLASS()
class RPGPROJECT_API ARPGProjectPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ARPGProjectPlayerController();

	// Called to bind functionality to input
	virtual void SetupInputComponent() override;

protected:

	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:		// --- FUNCTIONS --- \\

	//<><><><><><><><><><><><><><><><><><>
	// Events
	//<><><><><><><><><><><><><><><><><><>

	// Creates a notification on the player's screen when the possessed pawn picks up an item
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG Player Controller|Events|UI")
	void BPCreateNotification(UItemData* ItemData);


	//<><><><><><><><><><><><><><><><><><>
	// Functions
	//<><><><><><><><><><><><><><><><><><>

	//------------------
	// Camera Manager
	//------------------
	
	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Camera Manager")
	ARPGPlayerCameraManager* GetRPGPlayerCameraManager() const;

	//------------------
	// Input
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Input")
	TMap<EGamePadActionMappings, FInputActionKeyMapping> GetInputActionGamepadMappings() { return InputActionGamepadMappings; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Input")
	void UpdateInputActionGamepadMappings();

	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Input")
	TMap<EGamePadActionMappings, EInputButtonState> GetMappedActionStates() const { return MappedActionStates; }
	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Input")
	EInputButtonState GetMappedActionState(TEnumAsByte<EGamePadActionMappings> ActionMapping) const;
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Input")
	void SetMappedActionState(TEnumAsByte<EGamePadActionMappings> ActionMapping, EInputButtonState NewInputState);

	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Input")
	float GetJumpOrCrouchHoldTime() const { return JumpOrCrouchHoldTime; }
	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Input")
	float GetJumpOrCrouchHoldTimeThreshold() const { return JumpOrCrouchHoldTimeThreshold; }

	//------------------
	// Pawn
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Pawn")
	bool IsPawnRotationDisabled() { return bDisablePawnRotation; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Pawn")
	void DisablePawnRotation(bool bActive) { bDisablePawnRotation = bActive; }

	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Pawn")
	bool GetOverridePawnRotation() const { return bOverridePawnRotation; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Pawn")
	void OverridePawnRotation(bool bActive) { bOverridePawnRotation = bActive; }

	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|Pawn")
	bool GetOverridePawnRotationForLockOn() const { return bOverridePawnRotationForLockOn; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|Pawn")
	void OverridePawnRotationForLockOn(bool bActive) { bOverridePawnRotationForLockOn = bActive; }

	//------------------
	// UI
	//------------------

	// Returns true if the player is interacting with any menu UI
	UFUNCTION(BlueprintPure, Category = "RPG Player Controller|Functions|UI")
	bool IsPlayerInMenu() const { return bIsPlayerInMenu; }
	// Sets bIsPlayerInMenu
	UFUNCTION(BlueprintCallable, Category = "RPG Player Controller|Functions|UI")
	void SetIsPlayerInMenu(bool bActive) { bIsPlayerInMenu = bActive; }



public:		// --- VARIABLES --- \\



protected:	// --- FUNCTIONS --- \\

	
	// Updates the state of each mapped gamepad action
	void InputButtonStateUpdate();

	// Updates the values of bOverridePawnRotation & bOverridePawnRotationForLockOn
	void UpdatePawnRotationBooleans();

	// Determines the desired rotation of the possessed pawn and interpolates towards it  
	void CalculateDesiredPawnRotation();

	// Checks the mouse & gamepad input when called, attempts to swap the lock-on target if the input passes a certain threshold
	void CheckInputToSwapLockOnTarget();

	// Checks the mouse & gamepad input when called, requests for the possessed character to crouch/uncrouch if the jump/crouch input has been held for long enough
	void CheckInputToJumpOrCrouch(float DeltaTime);

	// Checks the mouse & gamepad input when called, request for the possessed character to execute an attack finisher if the attack input has been held for long enough
	void CheckInputForAttackFinisher(float DeltaTime);

	//------------------
	// Input
	//------------------

	void RequestMoveForward(float Value);
	void RequestMoveRight(float Value);

	void RequestTurnRate(float Rate);
	void RequestAddControllerYawInput(float Value);
	
	void RequestLookUpRate(float Rate);
	void RequestAddControllerPitchInput(float Value);

	// Request for the possessed character to jump if they are moving fast enough, or to crouch if the possessed character is moving slowly and the input is being held down
	void RequestJumpOrCrouch();
	// Request for the possessed character to stop jumping if they are not touching the ground, or to jump if the input was held down for a short period of time 
	void RequestStopJumpOrCrouch();

	void RequestJump();
	void RequestStopJumping();

	void RequestSprint();
	void RequestStopSprinting();
	void RequestToggleSprint();

	void RequestHoldCrouch();
	void RequestStopCrouching();
	// Request for the possessed character to crouch if they are standing or to stand up if they are crouching
	void RequestToggleCrouch();

	void RequestMainhandStance();
	void RequestStopMainhandStance();

	void RequestOffhandStance();
	void RequestStopOffhandStance();

	void RequestSheatheUnsheatheWeapon();

	void RequestWalkMode();
	void RequestStopWalkMode();

	void RequestContextAction();
	// Currently not in use but may be used in future combat/class updates
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

	void RequestSwapWeaponLoadout();

	void RequestToggleLockOn();

protected: 	// --- VARIABLES --- \\

	//------------------
	// Camera
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera")
	ARPGPlayerCameraManager* RPGPlayerCameraManagerRef;

	// Mouse
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Mouse")
	float MousePitchValue = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Mouse")
	float MouseYawValue = 0.f;

	// Gamepad
	//------------------

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float GamepadBaseLookUpRate = 70.f;
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float GamepadBaseTurnRate = 70.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float GamepadPitchRate = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float GamepadYawRate = 0.f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float NormalisedGamepadPitchRate = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Gamepad")
	float NormalisedGamepadYawRate = 0.f;

	// Lock-On
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Lock-On")
	float SwapLockOnTargetGamepadThreshold = .4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Camera|Lock-On")
	float SwapLockOnTargetMouseThreshold = .35f;

	//------------------
	// Input
	//------------------

	// Attacks
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	bool bLightAttackIsPressed = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	bool bHeavyAttackIsPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	float LightAttackHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	float LightAttackHoldTimeThreshold = .5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	float HeavyAttackHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Attacks")
	float HeavyAttackHoldTimeThreshold = .5f;

	// Context Action
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Context Action")
	bool bContextActionIsPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Context Action")
	float ContextActionHoldTime = .0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Context Action")
	float ContextActionDodgeThreshold = .33f;

	// Gamepad Mappings
	//------------------
	
	// A TMap containing all the actions bound to the input component that are used by a gamepad
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Gamepad Mappings")
	TMap<EGamePadActionMappings, FInputActionKeyMapping> InputActionGamepadMappings;

	// A TMap containing the properties of specific selection of input axis
	TMap<FName, FInputAxisProperties> InputAxisProperties;

	// A TMap containing the state of each mapped action, used by widgets to inform the player about what actions they can currently take
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Gamepad Mappings")
	TMap<EGamePadActionMappings, EInputButtonState> MappedActionStates;

	// Jump / Crouch
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Jump / Crouch")
	bool bJumpOrCrouchPressed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Jump / Crouch")
	float JumpOrCrouchHoldTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Input|Jump / Crouch")
	float JumpOrCrouchHoldTimeThreshold = .5f;

	//------------------
	// Pawn
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn")
	ARPGProjectPlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn")
	UCombatComponent* CombatComponentRef;


	// Movement
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	bool bIsMovementStopped;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	float MoveForwardValue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	float MoveRightValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	FRotator PlayerInputRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	FRotator DesiredActorRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	float PawnTurnSpeed = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	bool bDisablePawnRotation = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	bool bOverridePawnRotation = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|Pawn|Movement")
	bool bOverridePawnRotationForLockOn = false;

	//------------------
	// UI
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Controller|UI")
	bool bIsPlayerInMenu;

private: 	// --- FUNCTIONS --- \\



private:	// --- VARIABLES --- \\



};