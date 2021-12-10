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

#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/Components/DamageHandlerComponent.h"
#include "Actors/Weapons/WeaponBase.h"

#include "Controllers/RPGProjectPlayerController.h"



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
	PlayerCombatState = EPlayerCombatState::PCS_AtEase;
	PlayerActionState = EPlayerActionState::PAS_Idle;

	LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;
	LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;
	LastPlayerCombatState = PlayerCombatState;
	LastPlayerActionState = PlayerActionState;

	EquippedWeaponType = EWeaponType::WT_None;

	// Variables for character movement
	SprintSpeedMultiplier = 1.6875f;
	CrouchSpeedMultiplier = 0.66f;
	CombatSpeedMultiplier = 1.0f;

	NormalMaxAcceleration = 2048;
	SprintingMaxAcceleration = 8192;
	CharacterMinAnalogWalkSpeed = 0;

	MovementSpeed = 400;
	WalkMovementSpeed = 150;
	
	// Don't rotate when the controller rotates. Let that just affect the camera. - Taken from 3rdP Character BP
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character Movement settings
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	
	bIsCrouched = false;
	bIsRagdollDeath = false;
	bIsExhausted = false;
	bIsAttacking = false;
	bCanAttack = true;
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

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	
	DamageHandlerComponent = CreateDefaultSubobject<UDamageHandlerComponent>(TEXT("DamageHandlerComponent"));

	EquippedWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("EquippedWeapon"));
	EquippedWeapon->SetupAttachment(GetMesh(), FName("SheathRBack"));
	// Tags.Add("Player");

}

// Called when the game starts or when spawned
void ARPGProjectPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ARPGProjectPlayerController>(GetWorld()->GetFirstPlayerController());
	
	//GetComponents<UArrowComponent>(ArrowArray);
}

// Called every frame
void ARPGProjectPlayerCharacter::Tick(float DeltaTime)
{
	// Normalise before tick
	// ControlInputVector;

	Super::Tick(DeltaTime);

	bIsFalling = GetVelocity().Z != 0;

	CrouchMovementSpeed = MovementSpeed * CrouchSpeedMultiplier;
	SprintMovementSpeed = MovementSpeed * SprintSpeedMultiplier;
	CombatMovementSpeed = MovementSpeed * CombatSpeedMultiplier;

	CurrentCharacterXYVelocity = UKismetMathLibrary::VSizeXY(GetVelocity());

	CheckCharacterExhaustion();

	CheckPlayerVerticalMobility();
	CheckPlayerHorizontalMobility();
	CheckPlayerCombatState();
	CheckPlayerActionState();

	PlayerVerticalMobilityUpdate();
	PlayerHorizontalMobilityUpdate();
	PlayerCombatStateUpdate();
	PlayerActionStateUpdate();
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
				GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;

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
				/*GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;*/
				
				break;
			}
			case EPlayerHorizontalMobility::PHM_Walking:
			{
				GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;

				break;
			}
			case EPlayerHorizontalMobility::PHM_Jogging:
			{
				GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration;

				break;
			}
			case EPlayerHorizontalMobility::PHM_Sprinting:
			{
				GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
				GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
				GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;
				
				if (CurrentCharacterXYVelocity > 0 && PlayerVerticalMobilityState == EPlayerVerticalMobility::PVM_Standing)
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
// EPlayerCombatState functions
//-------------------------------------

void ARPGProjectPlayerCharacter::SetPlayerCombatState(EPlayerCombatState NewState)
{
	LastPlayerCombatState = PlayerCombatState;
	PlayerCombatState = NewState;
}

void ARPGProjectPlayerCharacter::CheckPlayerCombatState()
{

}

void ARPGProjectPlayerCharacter::PlayerCombatStateUpdate()
{

}

void ARPGProjectPlayerCharacter::ClearLastPlayerCombatStateChanges()
{

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

void ARPGProjectPlayerCharacter::SetCapsuleHeight(float NewCapsuleHeight)
{ 
	GetCapsuleComponent()->SetCapsuleHalfHeight(NewCapsuleHeight); 
}

void ARPGProjectPlayerCharacter::ResetCapsuleHeight() 
{ 
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f); 
}