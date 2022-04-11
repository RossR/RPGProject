// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RPGProjectPlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"

#include "Actors/Components/CharacterStatisticComponent.h"
#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/Components/DamageHandlerComponent.h"
#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/ItemTypes/ItemWeapon.h"

#include "Controllers/RPGProjectPlayerController.h"

static TAutoConsoleVariable<bool> CVarDisplayTrace
(
	TEXT("RPGProject.PlayerCharacter.Debug.DisplayTrace"),
	false,
	TEXT("Display Trace"),
	ECVF_Default
);


// Sets default values
ARPGProjectPlayerCharacter::ARPGProjectPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 70.0f;
	BaseLookUpRate = 70.0f;

	PlayerVerticalMobilityState = EPlayerVerticalMobility::PVM_Standing;
	PlayerHorizontalMobilityState = EPlayerHorizontalMobility::PHM_Idle;
	PlayerActionState = EPlayerActionState::PAS_Idle;

	LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;
	LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;
	LastPlayerActionState = PlayerActionState;

	EquippedWeaponType = EWeaponType::WT_None;

	// Variables for character movement
	SprintSpeedMultiplier = 1.6875f;
	CrouchSprintSpeedMultiplier = 1.25f;
	CombatSpeedMultiplier = 1.0f;

	NormalMaxAcceleration = 2048;
	SprintingMaxAcceleration = 8192;
	CharacterMinAnalogWalkSpeed = 0;

	MovementSpeed = 400;
	CrouchMovementSpeed = 265;
	WalkMovementSpeed = 150;
	
	// Don't rotate when the controller rotates. Let that just affect the camera. - Taken from 3rdP Character BP
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character Movement settings
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed;
	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;
	GetCharacterMovement()->SetCrouchedHalfHeight(66.0f);
	
	JumpMaxHoldTime = 0.2f;

	bIsCrouched = false;
	bIsRagdollDeath = false;
	bIsExhausted = false;
	bIsAttacking = false;
	bCanAttack = true;
	bIsAiming = false;
	bIsInUninterruptableAction = false;

	// Create the camera arm
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetRelativeLocation(FVector(0, 0, 40.0f));
	CameraArm->TargetArmLength = 400.0f;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bInheritPitch = true;
	CameraArm->bInheritYaw = true;
	CameraArm->bInheritRoll = false;
	CameraArm->bEnableCameraLag = true;
	CameraArm->CameraLagSpeed = 10.0f;

	// Create the follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	ChaseArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Chase"));
	ChaseArrow->SetupAttachment(CameraArm, USpringArmComponent::SocketName);

	RightShoulderArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("RightShoulder"));
	RightShoulderArrow->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	RightShoulderArrow->SetRelativeLocation({250.0f, 50.0f, 50.0f});
	
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);

	CharacterStatisticComponent = CreateDefaultSubobject<UCharacterStatisticComponent>(TEXT("CharacterStatComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	
	DamageHandlerComponent = CreateDefaultSubobject<UDamageHandlerComponent>(TEXT("DamageHandlerComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->AttachEquipmentToMesh(GetMesh());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	// Tags.Add("Player");

}

// Called when the game starts or when spawned
void ARPGProjectPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ARPGProjectPlayerController>(GetWorld()->GetFirstPlayerController());

	if (HealthComponent)
	{
		FCharacterStatistics CharacterStatistics = CharacterStatisticComponent->GetCharacterStatistics();

		HealthComponent->SetMaxHealth(CharacterStatistics.HealthPoints);
	}

	if (StaminaComponent)
	{
		FCharacterStatistics CharacterStatistics = CharacterStatisticComponent->GetCharacterStatistics();
		StaminaComponent->SetMaxStamina(CharacterStatistics.StaminaPoints);
		StaminaComponent->SetCurrentStamina(CharacterStatistics.StaminaPoints);
	}

	//GetComponents<UArrowComponent>(ArrowArray);
}

// Called every frame
void ARPGProjectPlayerCharacter::Tick(float DeltaTime)
{
	// Normalise before tick
	// ControlInputVector;

	Super::Tick(DeltaTime);

	DeltaSeconds = DeltaTime;

	CharacterStatisticsUpdate();

	bIsFalling = GetVelocity().Z != 0;

	// CrouchMovementSpeed = MovementSpeed * 0.66f;
	SprintMovementSpeed = MovementSpeed * SprintSpeedMultiplier;
	CombatMovementSpeed = MovementSpeed * CombatSpeedMultiplier;

	CurrentCharacterXYVelocity = UKismetMathLibrary::VSizeXY(GetVelocity());

	CheckCharacterExhaustion();

	CheckPlayerVerticalMobility();
	CheckPlayerHorizontalMobility();
	CheckPlayerActionState();

	PlayerVerticalMobilityUpdate();
	PlayerHorizontalMobilityUpdate();
	PlayerActionStateUpdate();

	InteractionTrace();
}

// Called to bind functionality to 
void ARPGProjectPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump); // &ARPGProjectPlayerCharacter::Jump also works
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping); // &ARPGProjectPlayerCharacter::StopJumping also works
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ARPGProjectPlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ARPGProjectPlayerCharacter::StopSprinting);
	PlayerInputComponent->BindAction("HoldCrouch", IE_Pressed, this, &ARPGProjectPlayerCharacter::HoldCrouch);
	PlayerInputComponent->BindAction("HoldCrouch", IE_Released, this, &ARPGProjectPlayerCharacter::StopHoldingCrouch);
	PlayerInputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &ARPGProjectPlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARPGProjectPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARPGProjectPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &ARPGProjectPlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ARPGProjectPlayerCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	*/
}

void ARPGProjectPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	if (HealthComponent && !HealthComponent->IsDead())
	{
		HealthComponent->SetCurrentHealth(0.0f);
		OnDeath(true);
	}
}

void ARPGProjectPlayerCharacter::OnDeath(bool IsFellOut)
{
	GetWorldTimerManager().SetTimer(RestartLevelTimerHandle, this, &ARPGProjectPlayerCharacter::OnDeathTimerFinished, TimeRestartAfterDeath, false);
	GetCharacterMovement()->DisableMovement();
	
}

void ARPGProjectPlayerCharacter::OnDeathTimerFinished()
{
	APlayerController* PlayerController = GetController <APlayerController>();
	if (PlayerController)
	{
		PlayerController->RestartLevel();
	}
}

void ARPGProjectPlayerCharacter::CharacterStatisticsUpdate()
{
	//if (CharacterStatComponent)
	//{
	//	int OldStamina = 0;

	//	FString AdditionalHealthPointsString = "AdditionalHealthPoints: " + FString::FromInt(CharacterStatComponent->AdditionalHealthPoints);
	//	// GEngine->AddOnScreenDebugMessage(1, DeltaSeconds, FColor::Yellow, AdditionalHealthPointsString);

	//	if (CharacterStatComponent->HaveStatisticsChanged())
	//	{
	//		OldStamina = CharacterStatComponent->AdditionalStaminaPoints;

	//		
	//	}

	//	FCharacterStatistics CharacterStatistics = CharacterStatComponent->GetCharacterStatistics();
	//	
	//	if (HealthComponent)
	//	{
	//		
	//		

	//		if (CharacterStatComponent->HaveStatisticsChanged())
	//		{
	//			HealthComponent->HealDamage(CharacterStatComponent->StatChangeHealthDifference);

	//			FString OldHealthPointsString = "OldHealthPoints: " + FString::FromInt(CharacterStatComponent->OldHealthPoints);
	//			GEngine->AddOnScreenDebugMessage(2, 2.0f, FColor::Red, OldHealthPointsString);
	//			
	//			FString HealthToHealString = "HealthToHeal: " + FString::FromInt(CharacterStatComponent->StatChangeHealthDifference);
	//			GEngine->AddOnScreenDebugMessage(3, 2.0f, FColor::Emerald, HealthToHealString);
	//		}
	//	}

	//	if (StaminaComponent)
	//	{
	//		StaminaComponent->SetMaxStamina(CharacterStatistics.StaminaPoints);

	//		if (CharacterStatComponent->HaveStatisticsChanged())
	//		{
	//			StaminaComponent->TakeStaminaDamage(OldStamina - CharacterStatComponent->AdditionalStaminaPoints);
	//		}
	//	}

	//	// CharacterStatComponent->OldHealthPoints = CharacterStatComponent->AdditionalHealthPoints;
	//	CharacterStatComponent->SetHaveStatisticsChanged(false);
	//}
}

//--------------------------------------------------------------
// State Machine Functions
//--------------------------------------------------------------

//-------------------------------------
// EPlayerVerticalMobility functions
//-------------------------------------

void ARPGProjectPlayerCharacter::SetPlayerVerticalMobilityState(EPlayerVerticalMobility NewState)
{
	LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;
	PlayerVerticalMobilityState = NewState;
}

void ARPGProjectPlayerCharacter::CheckPlayerVerticalMobility()
{

}

void ARPGProjectPlayerCharacter::PlayerVerticalMobilityUpdate()
{
	if (true)//HasPlayerVerticalMobilityStateChanged())
	{
		switch (PlayerVerticalMobilityState)
		{
			case EPlayerVerticalMobility::PVM_Standing:
			{
				break;
			}
			case EPlayerVerticalMobility::PVM_Crouching:
			{
				break;
			}
			case EPlayerVerticalMobility::PVM_Crawling:
			{
				break;
			}
			case EPlayerVerticalMobility::PVM_Jumping:
			{
				break;
			}
			case EPlayerVerticalMobility::PVM_Falling:
			{
				break;
			}
		}
	}
}

void ARPGProjectPlayerCharacter::ClearLastPlayerVerticalMobilityStateChanges()
{
	// Crouch needs to stop crouching
}

//-------------------------------------
// EPlayerHorizontalMobilty functions
//-------------------------------------

void ARPGProjectPlayerCharacter::SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility NewState)
{
	LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;
	PlayerHorizontalMobilityState = NewState;
}

void ARPGProjectPlayerCharacter::CheckPlayerHorizontalMobility()
{
	// Need to fix the code below so it does not override input functions of PC

	/*if (PlayerHorizontalMobilityState != EPlayerHorizontalMobility::PHM_Sprinting || PlayerHorizontalMobilityState != EPlayerHorizontalMobility::PHM_Walking)
	{
		if (CurrentCharacterXYVelocity > WalkMovementSpeed)
		{
			PlayerHorizontalMobilityState = EPlayerHorizontalMobility::PHM_Jogging;
		}
		else if (CurrentCharacterXYVelocity == 0.0f)
		{
			PlayerHorizontalMobilityState = EPlayerHorizontalMobility::PHM_Idle;
		}
	}*/
}

void ARPGProjectPlayerCharacter::PlayerHorizontalMobilityUpdate()
{
	if (true)//HasPlayerHorizontalMobilityStateChanged())
	{
		switch (PlayerHorizontalMobilityState)
		{
			case EPlayerHorizontalMobility::PHM_Idle:
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;
							
				break;
			}
			case EPlayerHorizontalMobility::PHM_Walking:
			{
				GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;

				break;
			}
			case EPlayerHorizontalMobility::PHM_Jogging:
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;
				
				break;
			}
			case EPlayerHorizontalMobility::PHM_Sprinting:
			{
				GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * CrouchSprintSpeedMultiplier;
				GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;
				
				if (CurrentCharacterXYVelocity > 0 && GetCharacterMovement()->IsMovingOnGround()) //&& PlayerVerticalMobilityState == EPlayerVerticalMobility::PVM_Standing)
				{
					TakeStaminaDamage(StaminaDamagePerInterval);
				}

				break;
			}
		}
	}
}

void ARPGProjectPlayerCharacter::ClearLastPlayerHorizontalMobilityStateChanges()
{
	//Sprint needs to stop sprinting function
	
}

//-------------------------------------
// EPlayerActionState functions
//-------------------------------------

void ARPGProjectPlayerCharacter::SetPlayerActionState(EPlayerActionState NewState)
{
	LastPlayerActionState = PlayerActionState;
	PlayerActionState = NewState;
}

void ARPGProjectPlayerCharacter::CheckPlayerActionState()
{
	
}

void ARPGProjectPlayerCharacter::PlayerActionStateUpdate()
{

}

void ARPGProjectPlayerCharacter::ClearLastPlayerActionStateChanges()
{

}


//--------------------------------------------------------------
//--------------------------------------------------------------

void ARPGProjectPlayerCharacter::RequestJump()
{
	if (bIsInUninterruptableAction) { return; }


	if (!GetCharacterMovement()->IsCrouching())
	{
		Jump();
	}
}

void ARPGProjectPlayerCharacter::RequestStopJumping()
{
	StopJumping();
}

void ARPGProjectPlayerCharacter::RequestSprint()
{
	SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Sprinting);
}

void ARPGProjectPlayerCharacter::RequestStopSprinting()
{
	if (GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::PHM_Sprinting)
	{
		// CheckSpeedToSetMoveState();
		SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::PHM_Jogging);
	}
}

void ARPGProjectPlayerCharacter::RequestHoldCrouch()
{
	if (!GetCharacterMovement()->IsMovingOnGround()) { return; }

	GetCharacterMovement()->bWantsToCrouch = true;
	SetIsCrouched(true);
	SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Crouching);
}

void ARPGProjectPlayerCharacter::RequestStopCrouching()
{
	GetCharacterMovement()->bWantsToCrouch = false;
	SetIsCrouched(false);

	if (GetPlayerVerticalMobilityState() == EPlayerVerticalMobility::PVM_Crouching)
	{
		SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Standing);
	}
}

void ARPGProjectPlayerCharacter::RequestToggleCrouch()
{
	if (!GetCharacterMovement()->IsCrouching())
	{
		RequestHoldCrouch();
	}
	else
	{
		RequestStopCrouching();
	}
}

void ARPGProjectPlayerCharacter::RequestAim()
{
	if (GetPlayerCombatState() == ECombatState::CS_AtEase)
	{
		MoveCameraToArrowLocation(FName(TEXT("RightShoulder")));
		bUseControllerRotationYaw = true;
		bIsAiming = true;
	}
}

void ARPGProjectPlayerCharacter::RequestStopAiming()
{
	MoveCameraToArrowLocation(FName(TEXT("Chase")));
	bUseControllerRotationYaw = false;
	bIsAiming = false;
}

void ARPGProjectPlayerCharacter::RequestReadyWeapon()
{
	if (CombatComponent)
	{
		CombatComponent->ToggleCombatState();
	}
}

void ARPGProjectPlayerCharacter::RequestWalkMode()
{
}

void ARPGProjectPlayerCharacter::RequestStopWalkMode()
{
}

void ARPGProjectPlayerCharacter::RequestInteractOrDodge()
{
	if (GetVelocity().Length() > 151.f || GetPlayerCombatState() == ECombatState::CS_CombatReady)
	{
		RequestDodge();
	}
	else
	{
		RequestInteraction();
	}
}

void ARPGProjectPlayerCharacter::RequestInteraction()
{
	if (LookedAtActor)
	{
		// Check if actor is item, then pick it up
		if (AItemBase* ItemRef = Cast<AItemBase>(LookedAtActor))
		{
			if (!InventoryComponent) { return; }

			InventoryComponent->AddItemToInventory(ItemRef->GetItemData());

			ItemRef->Destroy();
		}

		// Check if actor is an interactable, then interact with it
	}
}

void ARPGProjectPlayerCharacter::RequestDodge()
{
	if (bIsInUninterruptableAction || GetCharacterMovement()->IsFalling()) { return; }

	if (PlayDodgeMontage())
	{
		SetPlayerActionState(EPlayerActionState::PAS_Dodging);
		bIsInUninterruptableAction = true;
		bCanAttack = false;
	}
}



void ARPGProjectPlayerCharacter::RequestLightAttack()
{
	if (CombatComponent && !bIsExhausted)
	{
		CombatComponent->CharacterAttack(EAttackType::AT_LightAttack);
	}
}

void ARPGProjectPlayerCharacter::RequestHeavyAttack()
{
	if (CombatComponent && !bIsExhausted)
	{
		CombatComponent->CharacterAttack(EAttackType::AT_HeavyAttack);
	}
}



bool ARPGProjectPlayerCharacter::PlayDodgeMontage()
{
	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = PlayAnimMontage(DodgeMontage, PlayRate) > 0.f;
	if (bPlayedSuccessfully)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (!DodgeMontageBlendingOutDelegate.IsBound())
			{
				DodgeMontageBlendingOutDelegate.BindUObject(this, &ARPGProjectPlayerCharacter::OnDodgeMontageBlendingOut);
			}

			AnimInstance->Montage_SetBlendingOutDelegate(DodgeMontageBlendingOutDelegate, DodgeMontage);

			if (!DodgeMontageEndedDelegate.IsBound())
			{
				DodgeMontageEndedDelegate.BindUObject(this, &ARPGProjectPlayerCharacter::OnDodgeMontageEnded);
			}

			AnimInstance->Montage_SetEndDelegate(DodgeMontageEndedDelegate, DodgeMontage);

			AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &ARPGProjectPlayerCharacter::DodgeMontageOnNotifyBeginReceived);
			AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &ARPGProjectPlayerCharacter::DodgeMontageOnNotifyEndReceived);
		}
	}
	return bPlayedSuccessfully;
}

void ARPGProjectPlayerCharacter::UnbindDodgeMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &ARPGProjectPlayerCharacter::DodgeMontageOnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &ARPGProjectPlayerCharacter::DodgeMontageOnNotifyEndReceived);
	}
}

void ARPGProjectPlayerCharacter::OnDodgeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	bCanAttack = true;
	bIsInUninterruptableAction = false;
}

void ARPGProjectPlayerCharacter::OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindDodgeMontage();
	SetPlayerActionState(EPlayerActionState::PAS_Idle);
}

void ARPGProjectPlayerCharacter::DodgeMontageOnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void ARPGProjectPlayerCharacter::DodgeMontageOnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void ARPGProjectPlayerCharacter::CheckCharacterExhaustion()
{

	if (StaminaComponent)
	{
		if (StaminaComponent->IsStaminaExhausted())
		{
			bIsExhausted = true;
		}
		else
		{
			bIsExhausted = false;
		}
	}

}

float ARPGProjectPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::TakeDamage Damage %.2f"), Damage);
	if (HealthComponent && !HealthComponent->IsDead())
	{
		HealthComponent->TakeDamage(Damage);
		if (HealthComponent->IsDead())
		{
			OnDeath(false);
		}
	}
	return Damage;
}

void ARPGProjectPlayerCharacter::TakeStaminaDamage(float Damage)
{
	if (StaminaComponent)
	{
		StaminaComponent->TakeStaminaDamage(Damage);
	}
}

void ARPGProjectPlayerCharacter::SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval)
{
	
	if (DamageHandlerComponent)
	{
		DamageHandlerComponent->TakeFireDamage(BaseDamage, DamageTotalTime, TakeDamageInterval);
	}
}

void ARPGProjectPlayerCharacter::HandleItemCollected()
{
	ItemsCollected++;

	// Play effects here
	PC->PlayerCameraManager->StartCameraShake(CamShake, 1.0f);

	PC->PlayDynamicForceFeedback(ForceFeedbackIntensity, ForceFeedbackDuration, true, false, true, false, EDynamicForceFeedbackAction::Start);

	ItemCollected();
}

void ARPGProjectPlayerCharacter::MoveCameraToArrowLocation(FName ArrowName)
{
	UArrowComponent* ArrowComp = Cast<UArrowComponent>(GetDefaultSubobjectByName(ArrowName));
	// FVector NewCameraLocation = ArrowComp->GetRelativeLocation();
	// FRotator NewCameraRotation = ArrowComp->GetRelativeRotation();
	FLatentActionInfo ActionInfo;
	ActionInfo.CallbackTarget = this;

	if (FollowCamera != NULL && ArrowComp != nullptr)
	{
		UKismetSystemLibrary::MoveComponentTo(FollowCamera, ArrowComp->GetRelativeLocation(), ArrowComp->GetRelativeRotation(), false, false, 0.15f,false, EMoveComponentAction::Move, ActionInfo);
		// FollowCamera->SetRelativeLocation(NewCameraLocation);
	}
	else
	{
		if (FollowCamera == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation FollowCamera is NULL"));
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation FollowCamera is NULL"));
		}
		else if (ArrowComp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation ArrowComp is NULL, ArrowName might be wrong"));
		}
		
	}
	// FollowCamera->SetRelativeLocation(NewCameraLocation);
}

const bool ARPGProjectPlayerCharacter::IsAlive() const
{
	if (HealthComponent)
	{
		return !HealthComponent->IsDead();
	}
	return false;
}

const float ARPGProjectPlayerCharacter::GetCurrentHealth() const
{
	if (HealthComponent)
	{
		return HealthComponent->GetCurrentHealth();
	}
	return 0.0f;
}

const float ARPGProjectPlayerCharacter::GetCurrentStamina() const
{
	if (StaminaComponent)
	{
		return StaminaComponent->GetCurrentStamina();
	}
	return 0.0f;
}

bool ARPGProjectPlayerCharacter::IsStaminaFull()
{
	return StaminaComponent->GetCurrentStamina() == StaminaComponent->GetMaxStamina();
}

void ARPGProjectPlayerCharacter::ActivateRagdollCamera()
{
	CameraArm->SetupAttachment(GetMesh(), FName("CameraSocket"));
	CameraArm->SetRelativeLocation(FVector(0, 0, 0));
}

void ARPGProjectPlayerCharacter::AttachWeaponToSocket(FName SocketName)
{
	if (!EquipmentComponent) { return; }

	if (GetMesh()->DoesSocketExist(SocketName))
	{
		AItemWeapon* EquippedMainHandWeapon = Cast<AItemWeapon>(EquipmentComponent->GetWornEquipmentActorInSlot(EquipmentComponent->GetCurrentlyEquippedWeaponSet())->GetChildActor());

		if (EquippedMainHandWeapon)
		{
			EquippedMainHandWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, true), SocketName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::AttackWeaponToSocket SocketName is null."));
	}
}

void ARPGProjectPlayerCharacter::InteractionTrace()
{
	FVector Location;
	FRotator Rotation;
	if (PC)
	{
		PC->GetPlayerViewPoint(Location, Rotation);
		const FVector PlayerViewForward = Rotation.Vector();
		const float AdditionalDistance = (Location - GetActorLocation()).Size();
		FVector EndPos = Location + (PlayerViewForward * (TraceDistance + AdditionalDistance));

		const FVector CharacterForward = GetActorForwardVector();
		const float DotResult = FVector::DotProduct(PlayerViewForward, CharacterForward);

		

		// Prevent picking up objects behind us, this is when the camera is looking directly at the characters front side
		if (DotResult < -0.23f)
		{
			if (LookedAtActor)
			{
				if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
				{
					HighlightInterface->EnableHighlight(false);
				}
				
				LookedAtActor = nullptr;
			}
			return;
		}
		if (!HitActorArray.IsEmpty()) { HitActorArray.Empty(); }
		if (!HitResultArray.IsEmpty()) { HitResultArray.Empty(); }
		
		//TArray<FHitResult> HitResultArray;
		EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		// Centre of character capsule
		FVector StartPos = Location + (PlayerViewForward * AdditionalDistance);

		bTraceWasBlocked = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), StartPos, EndPos, SphereCastRadius, InteractionObjectTypeArray, false, ActorsToIgnore, DebugTrace, HitResultArray, true);
		// SphereTraceMulti(GetWorld(), Location, EndPos, SphereCastRadius, InteractionCollisionChannel, false, ActorsToIgnore, DebugTrace, HitResultArray, true);
		// ProcessTraceResult(HitResult);

		bool bFirstInteractableFound = false;
		if (bTraceWasBlocked)
		{
			
			for (int i = 0; i < HitResultArray.Num(); i++)
			{
				AActor* HitActor = HitResultArray[i].GetActor();
				if (HitActor)
				{
					HitActorArray.Emplace(HitActor);
					if (!bFirstInteractableFound && HitActor->ActorHasTag("Interactable"))
					{
						bool bCanSeeInteractable = false;

						// Line trace to see if player can see object (stops player from taking items through walls)
						if (HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()))
						{
							FVector CharacterInteractionCheck = GetMesh()->DoesSocketExist("InteractionCheck") ? GetMesh()->GetSocketLocation("InteractionCheck") : GetActorLocation();

							UStaticMeshComponent* InteractableMesh = Cast<UStaticMeshComponent>(HitActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
							FVector InteractableLocation = InteractableMesh->GetSocketLocation("InteractionCheck");

							FHitResult HitResult;
							UKismetSystemLibrary::LineTraceSingle(GetWorld(), CharacterInteractionCheck, InteractableLocation, SeeInteractableTraceCollisionChannel, false, ActorsToIgnore, DebugTrace, HitResult, true);

							if (HitResult.bBlockingHit)
							{
								if (HitResult.GetActor() == HitActor)
								{
									bCanSeeInteractable = true;
								}
							}
						}

						if (bCanSeeInteractable)
						{
							if (LookedAtActor != HitActor)
							{
								if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
								{
									HighlightInterface->EnableHighlight(false);
								}
							}
							LookedAtActor = HitActor;
							if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
							{
								HighlightInterface->EnableHighlight(true);
							}

							bFirstInteractableFound = true;
						}
					}
				}

			}
			
		}

		if (!bFirstInteractableFound)
		{
			if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
			{
				HighlightInterface->EnableHighlight(false);
			}
			LookedAtActor = nullptr;
		}

#if ENABLE_DRAW_DEBUG
		if (CVarDisplayTrace->GetBool())
		{
			const FString DotResultString = "DotResult: " + FString::SanitizeFloat(DotResult);
			GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red, DotResultString);

			static float FovDeg = 90.0f;
			DrawDebugCamera(GetWorld(), Location, Rotation, FovDeg);
			DrawDebugLine(GetWorld(), Location, EndPos, bTraceWasBlocked ? FColor::Red : FColor::White);
			DrawDebugPoint(GetWorld(), EndPos, SphereCastRadius, bTraceWasBlocked ? FColor::Red : FColor::White);
		}
#endif 
	}
}

void ARPGProjectPlayerCharacter::SetCapsuleHeight(float NewCapsuleHeight)
{ 
	GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHeight); 
}

void ARPGProjectPlayerCharacter::ResetCapsuleHeight() 
{ 
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f); 
}


