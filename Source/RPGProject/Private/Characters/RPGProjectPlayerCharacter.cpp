// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RPGProjectPlayerCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "AIController.h"

#include "Actors/Components/CharacterStatisticComponent.h"
#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/Components/DamageHandlerComponent.h"
#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"

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

	PopulateCameraSpringArmMap();
	PopulateCameraArrowMap();

	// Create the follow camera
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraSpringArmMap[ECameraView::CV_Exploration], USpringArmComponent::SocketName);
	PlayerCamera->SetRelativeLocation(CameraArrowMap[ECameraView::CV_Exploration]->GetRelativeLocation());
	PlayerCamera->bUsePawnControlRotation = false;
	
	HitFXOverride = CreateDefaultSubobject<UHitFXData>(TEXT("Hit FX Override"));
	
	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(GetMesh(), "spine_02");

	CharacterStatisticComponent = CreateDefaultSubobject<UCharacterStatisticComponent>(TEXT("CharacterStatComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	PopulateHealthComponentHitboxMap();

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	
	DamageHandlerComponent = CreateDefaultSubobject<UDamageHandlerComponent>(TEXT("DamageHandlerComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	EquipmentComponent->AttachEquipmentToMesh(GetMesh());

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	QuiverMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("QuiverMesh"));
	QuiverMesh->SetupAttachment(GetMesh(), "Equipment_Quiver");

	for (int i = 1; i <= 20.f; i++)
	{
		FName ArrowName = (FName)("Arrow_" + FString::FromInt(i));

		ArrowMeshMap.Add(i);
		ArrowMeshMap[i] = CreateDefaultSubobject<UStaticMeshComponent>(ArrowName);
		ArrowMeshMap[i]->SetupAttachment(QuiverMesh, ArrowName);
	}
	

	// Tags.Add("Player");

}

// Called when the game starts or when spawned
void ARPGProjectPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ARPGProjectPlayerController>(GetController()); //GetWorld()->GetFirstPlayerController());
	AIPC = Cast< AAIController>(GetController());

	if (PC)
	{
		RPGPlayerCameraManager = PC->GetRPGPlayerCameraManager();
		if (RPGPlayerCameraManager)
		{
			RPGPlayerCameraManager->SetCameraSpringArmMap(ECameraView::CV_Exploration, CameraSpringArmMap[ECameraView::CV_Exploration]);
			RPGPlayerCameraManager->SetCameraSpringArmMap(ECameraView::CV_Action, CameraSpringArmMap[ECameraView::CV_Action]);
			RPGPlayerCameraManager->SetCameraSpringArmMap(ECameraView::CV_Aim, CameraSpringArmMap[ECameraView::CV_Aim]);
			RPGPlayerCameraManager->SetCameraSpringArmMap(ECameraView::CV_LockOn, CameraSpringArmMap[ECameraView::CV_LockOn]);
			RPGPlayerCameraManager->SetCameraSpringArmMap(ECameraView::CV_Skill, CameraSpringArmMap[ECameraView::CV_Skill]);

			RPGPlayerCameraManager->SetCameraArrowMap(ECameraView::CV_Exploration, CameraArrowMap[ECameraView::CV_Exploration]);
			RPGPlayerCameraManager->SetCameraArrowMap(ECameraView::CV_Action, CameraArrowMap[ECameraView::CV_Action]);
			RPGPlayerCameraManager->SetCameraArrowMap(ECameraView::CV_Aim, CameraArrowMap[ECameraView::CV_Aim]);
			RPGPlayerCameraManager->SetCameraArrowMap(ECameraView::CV_LockOn, CameraArrowMap[ECameraView::CV_LockOn]);
			RPGPlayerCameraManager->SetCameraArrowMap(ECameraView::CV_Skill, CameraArrowMap[ECameraView::CV_Skill]);
		}
	}

	if (HealthComponent)
	{
		FCharacterStatistics CharacterStatistics = CharacterStatisticComponent->GetCharacterStatistics();

		HealthComponent->SetMaxHealthPoints(CharacterStatistics.HealthPoints);
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

	UpdateCurves();

	if (!IsAlive())
	{
		GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetPlayerActionState(EPlayerActionState::PAS_Incapacitated);
	}

	DeltaSeconds = DeltaTime;

	bIsFalling = GetVelocity().Z != 0;

	if (bIsFalling)
	{
		RequestStopCrouching();
	}

	// CrouchMovementSpeed = MovementSpeed * 0.66f;
	SprintMovementSpeed = MovementSpeed * SprintSpeedModifier;
	CombatMovementSpeed = MovementSpeed * CombatSpeedModifier;

	CurrentCharacterXYVelocity = UKismetMathLibrary::VSizeXY(GetVelocity());

	CheckCharacterExhaustion();

	CheckPlayerVerticalMobility();
	CheckPlayerHorizontalMobility();
	CheckPlayerActionState();

	PlayerVerticalMobilityUpdate();
	PlayerHorizontalMobilityUpdate();
	PlayerActionStateUpdate();

	CombatStanceUpdate();

	InteractionTrace();
}



// Called to bind functionality to 
void ARPGProjectPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARPGProjectPlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	if (HealthComponent && !HealthComponent->IsDead())
	{
		HealthComponent->SetCurrentHealthPoints(0.0f);
		OnDeath(true);
	}
}

void ARPGProjectPlayerCharacter::OnDeath(bool IsFellOut)
{
	GetWorldTimerManager().SetTimer(RestartLevelTimerHandle, this, &ARPGProjectPlayerCharacter::OnDeathTimerFinished, TimeRestartAfterDeath, false);
	GetCharacterMovement()->DisableMovement();
	PlayerActionState = EPlayerActionState::PAS_Incapacitated;
}

void ARPGProjectPlayerCharacter::OnDeathTimerFinished()
{
	APlayerController* PlayerController = GetController <APlayerController>();
	if (PlayerController)
	{
		PlayerController->RestartLevel();
	}
}

void ARPGProjectPlayerCharacter::UpdateCurves()
{
	if (!GetMesh()) { return; }

	if (UAnimInstance* CharacterAnimInstance = GetMesh()->GetAnimInstance())
	{
		float CurveValue = 0.f;
		CharacterAnimInstance->GetCurveValue("MovementSpeedReductionScale", CurveValue);
		MovementSpeedReductionScaleCurve = 1.f - CurveValue;
	}
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
		if (StaminaComponent)
		{
			if (StaminaComponent->IsStaminaExhausted())
			{
				TotalSpeedModifier = MovementSpeedReductionScaleCurve * StaminaExhaustedSpeedModifier;
			}
			else
			{
				TotalSpeedModifier = MovementSpeedReductionScaleCurve;
			}
		}
		else
		{
			TotalSpeedModifier = MovementSpeedReductionScaleCurve;
		}
		

		switch (PlayerHorizontalMobilityState)
		{
			case EPlayerHorizontalMobility::PHM_Idle:
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;
							
				break;
			}
			case EPlayerHorizontalMobility::PHM_Walking:
			{
				GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;

				break;
			}
			case EPlayerHorizontalMobility::PHM_Jogging:
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;
				
				break;
			}
			case EPlayerHorizontalMobility::PHM_Sprinting:
			{
				GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * CrouchSprintSpeedModifier * TotalSpeedModifier;
				GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * TotalSpeedModifier;
				GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration * TotalSpeedModifier;
				
				if (CurrentCharacterXYVelocity > (MovementSpeed * TotalSpeedModifier) && GetCharacterMovement()->IsMovingOnGround()) //&& PlayerVerticalMobilityState == EPlayerVerticalMobility::PVM_Standing)
				{
					ReduceCurrentStamina(StaminaDamagePerSecond * GetWorld()->GetDeltaSeconds());
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

void ARPGProjectPlayerCharacter::CombatStanceUpdate()
{
	if (!CombatComponent) { return; }
	if (!EquipmentComponent) { return; }

	if (CombatComponent->GetCombatState() == ECombatState::CS_CombatReady) 
	{
		if (CombatComponent->GetCombatWeaponStance() != RequestedCombatWeaponStance)//ECombatWeaponStance::CWS_None) { return; }
		{
			if (RequestedCombatWeaponStance == ECombatWeaponStance::CWS_None)
			{
				CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_None);
				ResetWeaponStance();

				return;
			}

			if (CombatComponent->GetCurrentWeaponStanceType() != EWeaponStanceType::ST_None) { ResetWeaponStance(); }

			UItemWeaponData* StanceWeaponData = nullptr;

			switch (RequestedCombatWeaponStance)
			{
			case ECombatWeaponStance::CWS_Mainhand:
				if (EquipmentComponent->GetMainhandWeaponData())
				{
					StanceWeaponData = EquipmentComponent->GetMainhandWeaponData();
				}
				break;

			case ECombatWeaponStance::CWS_Offhand:
				if (EquipmentComponent->GetOffhandWeaponData())
				{
					StanceWeaponData = EquipmentComponent->GetOffhandWeaponData();
				}
				else if (EquipmentComponent->GetMainhandWeaponData())
				{
					StanceWeaponData = EquipmentComponent->GetMainhandWeaponData();
				}
				break;

			default:
				break;
			}
			
			SetWeaponStance(RequestedCombatWeaponStance, StanceWeaponData);
		}
		else
		{

		}
	}
	else
	{
		if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::CWS_None)
		{
			CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_None);
			ResetWeaponStance();
		}
	}
	
	
}

void ARPGProjectPlayerCharacter::ClearLastPlayerActionStateChanges()
{

}

//--------------------------------------------------------------
//--------------------------------------------------------------

void ARPGProjectPlayerCharacter::RequestJump()
{
	if (bIsInUninterruptableAction || PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }


	if (GetCharacterMovement()->bWantsToCrouch)
	{
		RequestStopCrouching();
	}
	else
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
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
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
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	if (!GetCharacterMovement()->IsMovingOnGround()) { return; }

	GetCharacterMovement()->bWantsToCrouch = true;
	SetIsCrouched(true);

	if (RPGPlayerCameraManager)
	{
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::CV_Exploration)
		{
			CameraSpringArmMap[ECameraView::CV_Exploration]->SetRelativeLocation({ CameraSpringArmMap[ECameraView::CV_Exploration]->GetRelativeLocation().X, CameraSpringArmMap[ECameraView::CV_Exploration]->GetRelativeLocation().Y, 5.f });
		}
	}

	SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Crouching);
}

void ARPGProjectPlayerCharacter::RequestStopCrouching()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }

	GetCharacterMovement()->bWantsToCrouch = false;
	SetIsCrouched(false);

	if (RPGPlayerCameraManager)
	{
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::CV_Exploration)
		{
			CameraSpringArmMap[ECameraView::CV_Exploration]->SetRelativeLocation({ CameraSpringArmMap[ECameraView::CV_Exploration]->GetRelativeLocation().X, CameraSpringArmMap[ECameraView::CV_Exploration]->GetRelativeLocation().Y, 45.f });
		}
	}

	if (GetPlayerVerticalMobilityState() == EPlayerVerticalMobility::PVM_Crouching)
	{
		SetPlayerVerticalMobilityState(EPlayerVerticalMobility::PVM_Standing);
	}
}

void ARPGProjectPlayerCharacter::RequestToggleCrouch()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	//ToDo - Fix this as it will not trigger the else statement
	if (!GetCharacterMovement()->bWantsToCrouch)
	{
		RequestHoldCrouch();
	}
	else
	{
		RequestStopCrouching();
	}
}

void ARPGProjectPlayerCharacter::RequestMainhandStance()
{
	if (RequestedCombatWeaponStance != ECombatWeaponStance::CWS_None) { return; }

	RequestedCombatWeaponStance = ECombatWeaponStance::CWS_Mainhand;

	/*if (!CombatComponent) { return; }

	if (CombatComponent->GetCombatState() != ECombatState::CS_CombatReady) { return; }
	if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::CWS_None) { return; }

	CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_Mainhand);

	if (CombatComponent->GetCurrentWeaponStanceType() != EWeaponStanceType::ST_None) { ResetWeaponStance(); }

	SetWeaponStance(ECombatWeaponStance::CWS_Mainhand, EquipmentComponent->GetMainhandWeaponData());*/
}

void ARPGProjectPlayerCharacter::RequestStopMainhandStance()
{
	if (RequestedCombatWeaponStance == ECombatWeaponStance::CWS_Mainhand)
	{
		RequestedCombatWeaponStance = ECombatWeaponStance::CWS_None;
	}
	/*if (!CombatComponent) { return; }

	if (CombatComponent->GetCombatState() != ECombatState::CS_CombatReady) { return; }
	if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::CWS_Mainhand) { return; }

	CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_None);

	ResetWeaponStance();*/
}

void ARPGProjectPlayerCharacter::RequestOffhandStance()
{
	if (RequestedCombatWeaponStance != ECombatWeaponStance::CWS_None) { return; }

	RequestedCombatWeaponStance = ECombatWeaponStance::CWS_Offhand;

	/*if (!EquipmentComponent) { return; }
	if (!CombatComponent) { return; }

	if (CombatComponent->GetCombatState() != ECombatState::CS_CombatReady) { return; }
	if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::CWS_None) { return; }

	CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_Offhand);

	if (CombatComponent->GetCurrentWeaponStanceType() != EWeaponStanceType::ST_None) { ResetWeaponStance(); }

	if (EquipmentComponent->GetOffhandWeaponData())
	{
		SetWeaponStance(ECombatWeaponStance::CWS_Offhand, EquipmentComponent->GetOffhandWeaponData());
	}
	else if (EquipmentComponent->GetMainhandWeaponData())
	{
		SetWeaponStance(ECombatWeaponStance::CWS_Offhand, EquipmentComponent->GetMainhandWeaponData());
	}*/
}

void ARPGProjectPlayerCharacter::RequestStopOffhandStance()
{
	if (RequestedCombatWeaponStance == ECombatWeaponStance::CWS_Offhand)
	{
		RequestedCombatWeaponStance = ECombatWeaponStance::CWS_None;
	}
	/*if (!CombatComponent) { return; }
	if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::CWS_Offhand) { return; }

	CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::CWS_None);

	ResetWeaponStance();*/
}

void ARPGProjectPlayerCharacter::RequestSheatheUnsheatheWeapon()
{
	if (!CombatComponent) { return; }
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated || bIsInUninterruptableAction) { return; }
	
	CombatComponent->ToggleCombatState();

}

void ARPGProjectPlayerCharacter::RequestWalkMode()
{

}

void ARPGProjectPlayerCharacter::RequestStopWalkMode()
{

}

void ARPGProjectPlayerCharacter::RequestContextAction()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }

	if (GetVelocity().Length() > 151.f)
	{
		RequestDodge();
	}
	else
	{
		RequestInteraction();
	}
}

void ARPGProjectPlayerCharacter::RequestHoldContextAction()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }

	RequestSprint();
}

void ARPGProjectPlayerCharacter::RequestStopContextAction(bool bWasButtonHeld)
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }

	/*if (bWasButtonHeld)
	{
		RequestStopSprinting();
	}
	else if (GetVelocity().Length() > 151.f)
	{
		RequestDodge();
	}
	else
	{
		RequestInteraction();
	}*/
}

void ARPGProjectPlayerCharacter::RequestInteraction()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }

	if (LookedAtActor)
	{
		//if (LookedAtActor->Implements<IInteractionInterface>())
		if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(LookedAtActor))
		{
			if (AItemBase* ItemCast = Cast<AItemBase>(LookedAtActor))
			{
				ItemCast->OnItemPickup.BindUObject(PC, &ARPGProjectPlayerController::BPCreateNotification);
			}
			InteractionInterface->InteractionRequested(this);

			
			//IInteractionInterface::InteractionRequested(LookedAtActor, this);
		}

		// Check if actor is an interactable, then interact with it
	}
}

void ARPGProjectPlayerCharacter::RequestDodge()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated || bIsInUninterruptableAction || GetCharacterMovement()->IsFalling()) { return; }
	if (!CombatComponent) { return; }
	if (CombatComponent->GetIsInAttackRecovery()) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }

	CombatComponent->CombatDodge();
}

void ARPGProjectPlayerCharacter::RequestLightAttack()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (bIsInUninterruptableAction) { return; }
	
	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::CWS_None:
		CombatComponent->CharacterAttack(EAttackType::AT_LightAttack);
		break;

	case ECombatWeaponStance::CWS_Mainhand:
		// Mainhand Skill #1
		break;

	case ECombatWeaponStance::CWS_Offhand:
		// Offhand Skill #1
		break;
	}
}

void ARPGProjectPlayerCharacter::RequestStopLightAttack()
{

}

void ARPGProjectPlayerCharacter::RequestHeavyAttack()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (bIsInUninterruptableAction) { return; }

	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::CWS_None:
		CombatComponent->CharacterAttack(EAttackType::AT_HeavyAttack);
		break;

	case ECombatWeaponStance::CWS_Mainhand:
		// Mainhand Skill #2
		break;

	case ECombatWeaponStance::CWS_Offhand:
		// Offhand Skill #2
		break;
	}
}

void ARPGProjectPlayerCharacter::RequestStopHeavyAttack()
{

}

void ARPGProjectPlayerCharacter::RequestLightAttackFinisher()
{

	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (bIsInUninterruptableAction) { return; }
		
	CombatComponent->CharacterAttack(EAttackType::AT_LightFinisher);

}

void ARPGProjectPlayerCharacter::RequestHeavyAttackFinisher()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (bIsInUninterruptableAction) { return; }

	CombatComponent->CharacterAttack(EAttackType::AT_HeavyFinisher);
	
}

void ARPGProjectPlayerCharacter::RequestCombatAction()
{
	if (!CombatComponent) { return; }
	if (!EquipmentComponent) { return; }
	
	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::CWS_None:
		CombatComponent->StartNeutralCombatAction();
		break;

	case ECombatWeaponStance::CWS_Mainhand:
		CombatComponent->StartStanceCombatAction();
		break;

	case ECombatWeaponStance::CWS_Offhand:
		CombatComponent->StartStanceCombatAction();
		break;

	default:
		break;
	}

	CombatComponent->SetCombatActionIsPressed(true);
}

void ARPGProjectPlayerCharacter::RequestStopCombatAction()
{
	if (!CombatComponent) { return; }
	if (!EquipmentComponent) { return; }

	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::CWS_None:
		CombatComponent->StopNeutralCombatAction();
		break;

	case ECombatWeaponStance::CWS_Mainhand:
		CombatComponent->StopStanceCombatAction();
		break;

	case ECombatWeaponStance::CWS_Offhand:
		CombatComponent->StopStanceCombatAction();
		break;

	default:
		break;
	}

	CombatComponent->SetCombatActionIsPressed(false);
}

void ARPGProjectPlayerCharacter::RequestSwapWeaponLoadout()
{
	if (PlayerActionState == EPlayerActionState::PAS_Incapacitated || bIsInUninterruptableAction) { return; }

	if (CombatComponent)
	{
		CombatComponent->SwapWeaponLoadout();
	}
}

void ARPGProjectPlayerCharacter::PopulateHealthComponentHitboxMap()
{
	if (HealthComponent)
	{
		HealthComponent->AddToBodyPartMap("Head",		{ {"head", "neck_01"}, .1f, 100.f, 100.f, 1.25f, true, true });
		HealthComponent->AddToBodyPartMap("Torso",		{ {"pelvis", "spine_01", "spine_02"}, .3f, 300.f, 300.f, 1.f, true , true });
		HealthComponent->AddToBodyPartMap("Left Arm",	{ {"upperarm_l", "lowerarm_l", "hand_l"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Right Arm",	{ {"upperarm_r", "lowerarm_r", "hand_r"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Left Leg",	{ {"thigh_l", "calf_l", "foot_l", "ball_l"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Right Leg",	{ {"thigh_r", "calf_r", "foot_r", "ball_r"}, .15f, 150.f, 150.f, 1.f, false, true });
	}
}

void ARPGProjectPlayerCharacter::PopulateCameraSpringArmMap()
{
	CameraSpringArmMap.Emplace(ECameraView::CV_Exploration, CreateDefaultSubobject<USpringArmComponent>(TEXT("Exploration_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::CV_Action, CreateDefaultSubobject<USpringArmComponent>(TEXT("Action_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::CV_Aim, CreateDefaultSubobject<USpringArmComponent>(TEXT("Aim_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::CV_LockOn, CreateDefaultSubobject<USpringArmComponent>(TEXT("LockOn_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::CV_Skill, CameraSpringArmMap[ECameraView::CV_Action]);

	for (uint8 i = 1; i < (uint8)ECameraView::CV_MAX; i++)
	{

		if (CameraSpringArmMap.Contains((ECameraView)i))
		{
			CameraSpringArmMap[(ECameraView)i]->SetupAttachment(RootComponent);
			CameraSpringArmMap[(ECameraView)i]->bUsePawnControlRotation = true;
			CameraSpringArmMap[(ECameraView)i]->bInheritRoll = false;
		}
	}
	
	CameraSpringArmMap[ECameraView::CV_Exploration]->SetRelativeLocation({ 0.f, 0.f, 45.f });
	CameraSpringArmMap[ECameraView::CV_Exploration]->TargetArmLength = 300.f;
	CameraSpringArmMap[ECameraView::CV_Exploration]->bEnableCameraLag = true;
	
	CameraSpringArmMap[ECameraView::CV_Action]->SetRelativeLocation({ 0.f, 0.f, 40.f });
	CameraSpringArmMap[ECameraView::CV_Action]->TargetArmLength = 450.f;
	CameraSpringArmMap[ECameraView::CV_Action]->bEnableCameraLag = false;
	
	CameraSpringArmMap[ECameraView::CV_Aim]->SetRelativeLocation({ 0.f, 80.f, 75.f });
	CameraSpringArmMap[ECameraView::CV_Aim]->TargetArmLength = 200.f;
	CameraSpringArmMap[ECameraView::CV_Aim]->bEnableCameraLag = false;

	CameraSpringArmMap[ECameraView::CV_LockOn]->SetRelativeLocation({ 0.f, 0.f, 95.f });
	CameraSpringArmMap[ECameraView::CV_LockOn]->TargetArmLength = 500.f;
	CameraSpringArmMap[ECameraView::CV_LockOn]->bEnableCameraLag = false;
	CameraSpringArmMap[ECameraView::CV_LockOn]->bEnableCameraRotationLag = true;
	CameraSpringArmMap[ECameraView::CV_LockOn]->CameraRotationLagSpeed = 10.f;
}

void ARPGProjectPlayerCharacter::PopulateCameraArrowMap()
{
	CameraArrowMap.Emplace(ECameraView::CV_Exploration, CreateDefaultSubobject<UArrowComponent>(TEXT("Exploration_Arrow")));
	CameraArrowMap.Emplace(ECameraView::CV_Action, CreateDefaultSubobject<UArrowComponent>(TEXT("Action_Arrow")));
	CameraArrowMap.Emplace(ECameraView::CV_Aim, CreateDefaultSubobject<UArrowComponent>(TEXT("Aim_Arrow")));
	CameraArrowMap.Emplace(ECameraView::CV_LockOn, CreateDefaultSubobject<UArrowComponent>(TEXT("LockOn_Arrow")));
	CameraArrowMap.Emplace(ECameraView::CV_Skill, CreateDefaultSubobject<UArrowComponent>(TEXT("Skill_Arrow")));

	for (uint8 i = 1; i < (uint8)ECameraView::CV_MAX; i++)
	{
		if (CameraArrowMap.Contains((ECameraView)i))
		{
			if (i != (uint8)ECameraView::CV_Skill)
			{
				CameraArrowMap[(ECameraView)i]->SetupAttachment(CameraSpringArmMap[(ECameraView)i], USpringArmComponent::SocketName);
			}
			else
			{
				CameraArrowMap[(ECameraView)i]->SetupAttachment(CameraSpringArmMap[ECameraView::CV_Action], USpringArmComponent::SocketName);
			}
			CameraSpringArmMap[ECameraView::CV_Exploration]->SetRelativeLocation({ 0.f, 0.f, 0.f });
			CameraArrowMap[(ECameraView)i]->ArrowLength = 40.f;
		}
	}
}

void ARPGProjectPlayerCharacter::SetWeaponStance(ECombatWeaponStance CombatWeaponStanceType, UItemWeaponData* StanceWeaponData)
{
	//if (!PC) { return; }
	//if (!RPGPlayerCameraManager) { return; }
	if (!CombatComponent) { return; }
	if (CombatWeaponStanceType == ECombatWeaponStance::CWS_None) { return; }

	EWeaponStanceType StanceType = EWeaponStanceType::ST_None;

	switch (CombatWeaponStanceType)
	{
	case ECombatWeaponStance::CWS_Mainhand:
		StanceType = StanceWeaponData->MainhandStanceType;
		break;
	
	case ECombatWeaponStance::CWS_Offhand:
		StanceType = StanceWeaponData->OffhandStanceType;
		break;
	
	default:
		break;
	}

	CombatComponent->SetCombatWeaponStance(RequestedCombatWeaponStance);

	CombatComponent->SetCurrentWeaponStanceType(StanceType);

	switch (StanceType)
	{
	case EWeaponStanceType::ST_None:

		break;

	case EWeaponStanceType::ST_Ranged:
		if (RPGPlayerCameraManager)
		{
			RPGPlayerCameraManager->SetCameraView(ECameraView::CV_Aim);
		}
		bUseControllerRotationYaw = true;
		CombatComponent->SetAimAtCrosshair(true);
		break;

	case EWeaponStanceType::ST_Guard:
		if (PC)
		{
			PC->DisableCharacterRotation(true);
		}
		break;
	}

	if (StaminaComponent && StanceType != EWeaponStanceType::ST_None)
	{
		StaminaComponent->SetStaminaRegenMultiplier(StanceWeaponData->WeaponStanceStaminaRegenMultiplier);
		StaminaComponent->SetStaminaRegenDelayMultiplier(StanceWeaponData->WeaponStanceStaminaRegenDelayMultiplier);
	}

	

}

void ARPGProjectPlayerCharacter::ResetWeaponStance()
{
	if (!PC) { return; }
	if (!RPGPlayerCameraManager) { return; }
	if (!CombatComponent) { return; }

	if (StaminaComponent && CombatComponent->GetCurrentWeaponStanceType() != EWeaponStanceType::ST_None)
	{
		StaminaComponent->SetStaminaRegenMultiplier(1.f);
		StaminaComponent->SetStaminaRegenDelayMultiplier(1.f);

	}
	switch (CombatComponent->GetCurrentWeaponStanceType())
	{
	case EWeaponStanceType::ST_None:

		break;

	case EWeaponStanceType::ST_Ranged:
		if (RPGPlayerCameraManager->GetCameraView() != ECameraView::CV_Action)
		{
			RPGPlayerCameraManager->SetCameraView(ECameraView::CV_Action);
			bUseControllerRotationYaw = false;
			CombatComponent->SetAimAtCrosshair(false);
		}
		break;

	case EWeaponStanceType::ST_Guard:
		PC->DisableCharacterRotation(false);
		break;

	case EWeaponStanceType::ST_MAX:

		break;
	}

	CombatComponent->SetCurrentWeaponStanceType(EWeaponStanceType::ST_None);

	
}

void ARPGProjectPlayerCharacter::EnableDodgeCollision(bool bActive)
{
	if (bActive)
	{
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName("CharacterMeshDodge");
		}

		if (EquipmentComponent)
		{
			TMap<EEquipmentSlot, UChildActorComponent*> EquipmentMap = EquipmentComponent->GetWornEquipmentActorMap();

			for (uint8 i = 1; i < (uint8)EEquipmentSlot::ES_MAX; i++)
			{
				if (EquipmentMap.Contains((EEquipmentSlot)i))
				{
					AActor* ChildActor = EquipmentMap[(EEquipmentSlot)i]->GetChildActor();
					AItemBase* EquippedItem = Cast<AItemBase>(ChildActor);
					if (EquippedItem) { EquippedItem->GetItemMesh()->SetCollisionProfileName("NoCollision"); }
				}
			}
		}
	}
	else
	{
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName("CharacterMesh");
		}

		if (EquipmentComponent)
		{
			TMap<EEquipmentSlot, UChildActorComponent*> EquipmentMap = EquipmentComponent->GetWornEquipmentActorMap();

			for (uint8 i = 1; i < (uint8)EEquipmentSlot::ES_MAX; i++)
			{
				if (EquipmentMap.Contains((EEquipmentSlot)i))
				{
					AActor* ChildActor = EquipmentMap[(EEquipmentSlot)i]->GetChildActor();
					AItemBase* EquippedItem = Cast<AItemBase>(ChildActor);
					if (EquippedItem) { EquippedItem->GetItemMesh()->SetCollisionProfileName("EquippedItem"); }
				}
			}
		}
	}
}

void ARPGProjectPlayerCharacter::CheckCharacterExhaustion()
{
	if (!StaminaComponent) { return; }
	if (!CombatComponent) { return; }

	if (StaminaComponent->IsStaminaExhausted() && !CombatComponent->GetIsGuarding() && GetVelocity().Length() <= 0.f)
	{
		CombatComponent->StaminaExhausted();
	}
	else if (StaminaComponent->IsStaminaExhausted() && GetVelocity().Length() >= 0.f)
	{
		StaminaComponent->ResetStaminaRegenDelay();
	}
}

float ARPGProjectPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!CombatComponent) { return 0.f; }

	if (!CombatComponent->GetIsDodging())
	{
		float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::TakeDamage Damage %.2f"), Damage);
		if (HealthComponent && !HealthComponent->IsDead())
		{
			if (GetBodyPartHit().IsNone())
			{
				HealthComponent->TakeDamage(Damage);
			}
			else
			{
				HealthComponent->TakeDamage(Damage, BodyPartHit);
				SetBodyPartHit("");
			}

			if (HealthComponent->IsDead())
			{
				OnDeath(false);
			}
		}
		return Damage;
	}

	return 0.f;
}

void ARPGProjectPlayerCharacter::ReduceCurrentStamina(float Damage)
{
	if (StaminaComponent)
	{
		StaminaComponent->ReduceCurrentStamina(Damage);
	}
}

void ARPGProjectPlayerCharacter::SetOnFire(float BaseDamage, float DamageTotalTime, float TakeDamageInterval)
{
	if (!CombatComponent) { return; }

	if (!CombatComponent->GetIsDodging() && DamageHandlerComponent)
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

	if (PlayerCamera != NULL && ArrowComp != nullptr)
	{
		UKismetSystemLibrary::MoveComponentTo(PlayerCamera, ArrowComp->GetRelativeLocation(), ArrowComp->GetRelativeRotation(), false, false, 0.15f,false, EMoveComponentAction::Move, ActionInfo);
		// PlayerCamera->SetRelativeLocation(NewCameraLocation);
	}
	else
	{
		if (PlayerCamera == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation FollowCamera is NULL"));
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation FollowCamera is NULL"));
		}
		else if (ArrowComp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::MoveCameraToArrowLocation ArrowComp is NULL, ArrowName might be wrong"));
		}
		
	}
	// PlayerCamera->SetRelativeLocation(NewCameraLocation);
}

const bool ARPGProjectPlayerCharacter::IsAlive() const
{
	if (HealthComponent)
	{
		return !HealthComponent->IsDead();
	}
	return false;
}

const float ARPGProjectPlayerCharacter::GetCurrentHealthPoints() const
{
	if (HealthComponent)
	{
		return HealthComponent->GetCurrentHealthPoints();
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
					SetIsInteractionAvailable(false);
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
						if (HitActor->GetComponentByClass(UMeshComponent::StaticClass()))
						{
							FVector CharacterInteractionCheck = GetMesh()->DoesSocketExist("InteractionCheck") ? GetMesh()->GetSocketLocation("InteractionCheck") : GetActorLocation();

							UMeshComponent* InteractableMesh = Cast<UMeshComponent>(HitActor->GetComponentByClass(UMeshComponent::StaticClass()));
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
									SetIsInteractionAvailable(false);
								}
							}

							LookedAtActor = HitActor;

							if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(LookedAtActor))
							{
								bool bWillHightlight = InteractionInterface->GetIsInInteractableRange(this);

								if (InteractionInterface->CanBeInteractedWith())
								{
									InteractionInterface->EnableHighlight(bWillHightlight);
									SetIsInteractionAvailable(bWillHightlight);
								}
								else
								{
									InteractionInterface->EnableHighlight(false);
									SetIsInteractionAvailable(false);
								}
							}
							else if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
							{
								HighlightInterface->EnableHighlight(true);
								SetIsInteractionAvailable(true);
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
				SetIsInteractionAvailable(false);
			}
			LookedAtActor = nullptr;
		}

#if ENABLE_DRAW_DEBUG
		if (CVarDisplayTrace->GetBool())
		{
			//const FString DotResultString = "DotResult: " + FString::SanitizeFloat(DotResult);
			//GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Red, DotResultString);

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


