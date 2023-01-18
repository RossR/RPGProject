// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "Actors/Components/CombatComponent.h"
#include "Interfaces/InteractionInterface.h"
#include "Interfaces/HitFXInterface.h"
#include "DataAssets/HitFXData.h"
#include "PlayerCameraManagers/RPGPlayerCameraManager.h"
#include "RPGProjectPlayerCharacter.generated.h"


UENUM(BlueprintType)
enum class EPlayerVerticalMobility : uint8
{
	Standing		UMETA(DisplayName = "Standing"),
	Crouching		UMETA(DisplayName = "Crouching"),
	Crawling		UMETA(DisplayName = "Crawling"),
	Jumping			UMETA(DisplayName = "Jumping"),
	Falling			UMETA(DisplayName = "Falling"),

	MAX				UMETA(Hidden)
};

	
UENUM(BlueprintType)
enum class EPlayerHorizontalMobility : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Walking			UMETA(DisplayName = "Walking"),
	Jogging			UMETA(DisplayName = "Jogging"),
	Sprinting		UMETA(DisplayName = "Sprinting"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EPlayerActionState : uint8
{
	Idle				UMETA(DisplayName = "Idle"),
	Dodging				UMETA(DisplayName = "Dodging"),
	Interacting			UMETA(DisplayName = "Interacting"),
	Guarding			UMETA(DisplayName = "Guarding"),
	Aiming				UMETA(DisplayName = "Aiming"),
	Casting				UMETA(DisplayName = "Casting"),
	CombatAction		UMETA(DisplayName = "Combat Action"),
	Incapacitated		UMETA(DisplayName = "Incapacitated"),

	MAX					UMETA(Hidden)
};

class ARPGProjectPlayerController;
class ARPGPlayerCameraManager;

class AAIController;
class UCameraShakeBase;
class UParticleSystemComponent;
class UChildActorComponent;
class UShapeComponent;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;

class UCharacterStatisticComponent;
class UHealthComponent;
class UStaminaComponent;
class UDamageHandlerComponent;
class UInventoryComponent;
class UEquipmentComponent;
class UCombatComponent;

UCLASS(BlueprintType)
class RPGPROJECT_API ARPGProjectPlayerCharacter : public ACharacter, public IHitFXInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARPGProjectPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:		// --- FUNCTIONS --- \\

	//<><><><><><><><><><><><><><><><><><>
	// Events
	//<><><><><><><><><><><><><><><><><><>

	// Event called when interacting with an actor
	UFUNCTION(BlueprintImplementableEvent, Category = "RPG Character| Events")
	void InteractionStarted(AActor* InteractableActor);


	//<><><><><><><><><><><><><><><><><><>
	// Functions
	//<><><><><><><><><><><><><><><><><><>

	//------------------
	// Camera System
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Camera System")
	UCameraComponent* GetPlayerCamera() { return HasActiveCameraComponent() ? PlayerCamera : nullptr; }

	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Camera System")
	ARPGPlayerCameraManager* GetRPGPlayerCameraManager() { return RPGPlayerCameraManager; }
	
	//------------------
	// Combat
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Combat")
	void EnableDodgeCollision(bool bActive);

	//------------------
	// Combat - States
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Combat|States")
	void ResetWeaponStance();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Combat|States")
	void SetWeaponStance(ECombatWeaponStance CombatWeaponStanceType, UItemWeaponData* StanceWeaponData);

	//------------------
	// Damage
	//------------------

	/**
	 * Sets the character on fire
	 * @param BaseDamage 
	 * @param DamageTotalTime The total duration that damage will be dealt to the character
	 * @param TakeDamageInterval How frequently the damage is dealt to the character
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Damage")	
	void SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval);

	// Deal damage to the character's health
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Damage")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	//------------------
	// Death
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Death")
	bool GetIsRagdollDeath() { return bIsRagdollDeath; }
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Death")
	void SetIsRagdollDeath(bool IsActive) { bIsRagdollDeath = IsActive; }

	//------------------
	// Input
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestCombatAction();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopCombatAction();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestDodge();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestHeavyAttack();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopHeavyAttack();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestHeavyAttackFinisher();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestHoldCrouch();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopCrouching();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestToggleCrouch();

	// If the character is looking at an interactable then try to interact with it
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestInteraction();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestJump();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopJumping();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestLightAttack();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopLightAttack();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestLightAttackFinisher();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestMainhandStance();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopMainhandStance();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestOffhandStance();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopOffhandStance();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestSheatheUnsheatheWeapon();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestSprint();
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestStopSprinting();

	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Input")
	void RequestSwapWeaponLoadout();

	// These 3 functions are currently not in use, but may be used in future combat/class updates
	void RequestContextAction();
	void RequestHoldContextAction();
	void RequestStopContextAction(bool bWasButtonHeld = false);
	// ---

	//------------------
	// Interactions
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Interactions")
	bool GetIsInteractionAvailable() { return bInteractionAvailable; }
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Interactions")
	void SetIsInteractionAvailable(bool IsActive) { bInteractionAvailable = IsActive; }

	// Searches for and highlights the first interactable actor in the character's line of sight
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Interactions")
	void InteractionTrace();

	//------------------
	// Movement
	//------------------

	// Returns true if the character is moving on the z-axis
	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Movement")
	bool IsCharacterFalling() { return bIsFalling; }

	//------------------
	// Movement - States
	//------------------

	// Returns the character's horizontal mobility state
	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Movement|States")
	EPlayerHorizontalMobility GetPlayerHorizontalMobilityState() { return PlayerHorizontalMobilityState; }
	// Sets the character's horizontal mobility state
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Movement|States")
	void SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility NewState);

	// Returns the character's vertical mobility state
	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Movement|States")
	EPlayerVerticalMobility GetPlayerVerticalMobilityState() { return PlayerVerticalMobilityState; }
	// Sets the character's vertical mobility state
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Movement|States")
	void SetPlayerVerticalMobilityState(EPlayerVerticalMobility NewState);

	//------------------
	// Player Actions
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Player Actions")
	bool GetIsInUninterruptableAction() { return bIsInUninterruptableAction; }
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Player Actions")
	void SetIsInUninterruptableAction(bool bActive) { bIsInUninterruptableAction = bActive; }
	
	// Get the character's last action state
	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Player Actions")
	EPlayerActionState GetLastPlayerActionState() { return LastPlayerActionState; }

	// Get the character's action state
	UFUNCTION(BlueprintPure, Category = "RPG Character|Functions|Player Actions")
	EPlayerActionState GetPlayerActionState() { return PlayerActionState; }
	// Set the character's action state
	UFUNCTION(BlueprintCallable, Category = "RPG Character|Functions|Player Actions")
	void SetPlayerActionState(EPlayerActionState NewState);

	//------------------
	// Other
	//------------------

	// Called when the actor falls out the world (KillZ)
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;


	//<><><><><><><><><><><><><><><><><><>
	// HitFX Interface
	//<><><><><><><><><><><><><><><><><><>

	UHitFXData* GetHitFXData() override { return HitFXOverride; };

public:		// --- VARIABLES --- \\

	//------------------
	// Effects
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Effects")
	UParticleSystemComponent* ParticleSystemComponent;

protected:		// --- FUNCTIONS --- \\

	void OnDeath(bool IsFellOut);

	UFUNCTION()
	void OnDeathTimerFinished();

	void UpdateCurves();

	void PlayerHorizontalMobilityUpdate();

	void CombatStanceUpdate();

	void CheckCharacterExhaustion();

protected:		// --- VARIABLES --- \\

	//------------------
	// Camera System
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|Camera System|Spring Arms")
	TMap<ECameraView, USpringArmComponent*> CameraSpringArmMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|Camera System|Arrow")
	TMap<ECameraView, UArrowComponent*> CameraArrowMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"), Category = "Character Components|Camera System|Camera")
	UCameraComponent* PlayerCamera;

	//------------------
	// Curves 
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Curves")
	float MovementSpeedReductionScaleCurve = 1.f;

	//------------------
	// Effects
	//------------------

	UPROPERTY(EditAnywhere, Category = "RPG Character|Effects")
	TSubclassOf<UCameraShakeBase> CamShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta = (DisplayName = "Hit FX Override"), Category = "RPG Character|Effects")
	UHitFXData* HitFXOverride;

	//------------------
	// Equipment
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Equipment|Quiver")
	UStaticMeshComponent* QuiverMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Equipment|Quiver")
	TMap<int, UStaticMeshComponent*> ArrowMeshMap;

	//------------------
	// Force Feedback
	//------------------

	UPROPERTY(EditAnywhere, Category = "RPG Character|Force Feedback")
	float ForceFeedbackIntensity = 1.0f;

	UPROPERTY(EditAnywhere, Category = "RPG Character|Force Feedback")
	float ForceFeedbackDuration = 1.0f;

	//------------------
	// Interaction Trace
	//------------------

	UPROPERTY(EditAnywhere, Category = "RPG Character|Interaction Trace")
	TEnumAsByte<ETraceTypeQuery> SeeInteractableTraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category = "RPG Character|Interaction Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> InteractionObjectTypeArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	float TraceDistance = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	float SphereCastRadius = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	AActor* LookedAtActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	TArray<FHitResult> HitResultArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	TArray<AActor*> HitActorArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Interaction Trace")
	bool bTraceWasBlocked = false;

	bool bInteractionAvailable = false;

	//------------------
	// Movement
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Movement")
	float CurrentCharacterXYVelocity = 0.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float MovementSpeed = 400.f;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float CombatMovementSpeed = 0.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float WalkMovementSpeed = 150.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Movement")
	float CrouchMovementSpeed = 265.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Movement")
	float SprintMovementSpeed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|Movement")
	float TotalSpeedModifier = 1.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float CombatSpeedModifier = 1.f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float SprintSpeedModifier = 1.6875f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float CrouchSprintSpeedModifier = 1.25f;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	float StaminaExhaustedSpeedModifier = 0.5f;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	int32 NormalMaxAcceleration = 2048;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	int32 SprintingMaxAcceleration = 8192;

	UPROPERTY(VisibleAnywhere, BluePrintReadWrite, Category = "RPG Character|Movement")
	int32 CharacterMinAnalogWalkSpeed = 0;

	bool bIsCrouched = false;
	bool bIsFalling = false;

	//------------------
	// On Death
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|On Death")
	float TimeRestartAfterDeath = 5.0f;

	FTimerHandle RestartLevelTimerHandle;

	bool bIsRagdollDeath = false;

	//------------------
	// Other Components
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UCharacterStatisticComponent* CharacterStatisticComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UStaminaComponent* StaminaComponent;

	// Leftover from course, only used for fire damage currently
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UDamageHandlerComponent* DamageHandlerComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UEquipmentComponent* EquipmentComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Character|Components")
	UCombatComponent* CombatComponent;

	//------------------
	// Player Actions
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|Player Actions")
	bool bIsInUninterruptableAction = false;

	//------------------
	// States - Action
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Action")
	EPlayerActionState PlayerActionState = EPlayerActionState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Action")
	EPlayerActionState LastPlayerActionState = PlayerActionState;

	//------------------
	// States - Combat
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Combat")
	ECombatWeaponStance RequestedCombatWeaponStance = ECombatWeaponStance::None;

	//------------------
	// States - Mobility
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Mobility|Horizonal")
	EPlayerHorizontalMobility PlayerHorizontalMobilityState = EPlayerHorizontalMobility::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Mobility|Horizonal")
	EPlayerHorizontalMobility LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Mobility|Vertical")
	EPlayerVerticalMobility PlayerVerticalMobilityState = EPlayerVerticalMobility::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG Character|States|Mobility|Vertical")
	EPlayerVerticalMobility LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;

	//------------------
	// References
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|References|Player Controller")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|References|AI Controller")
	AAIController* AIPC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Character|References|Player Camera Manager")
	ARPGPlayerCameraManager* RPGPlayerCameraManager;

private:		// --- FUNCTIONS --- \\

	void PopulateHealthComponentHitboxMap();

	void PopulateCameraSpringArmMap();
	void PopulateCameraArrowMap();

private:		// --- VARIABLES --- \\



};