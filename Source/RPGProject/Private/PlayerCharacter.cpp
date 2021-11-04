// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/ArrowComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "HealthComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 70.0f;
	BaseLookUpRate = 70.0f;

	PlayerMoveState = EPlayerMoveState::PMS_Walking;

	MovementSpeed = 600;
	
	// Don't rotate when the controller rotates. Let that just affect the camera. - Taken from 3rdP Character BP
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Character Movement settings
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	

	// Create the camera arm
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->SetRelativeLocation(FVector(0, 0, 40.0f));
	CameraArm->TargetArmLength = 400.0f;
	CameraArm->bUsePawnControlRotation = true;
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
	
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//GetComponents<UArrowComponent>(ArrowArray);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	// Normalise before tick
	// ControlInputVector;

	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to 
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	/*
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump); // &APlayerCharacter::Jump also works
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping); // &APlayerCharacter::StopJumping also works
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);
	PlayerInputComponent->BindAction("HoldCrouch", IE_Pressed, this, &APlayerCharacter::HoldCrouch);
	PlayerInputComponent->BindAction("HoldCrouch", IE_Released, this, &APlayerCharacter::StopHoldingCrouch);
	PlayerInputComponent->BindAction("ToggleCrouch", IE_Pressed, this, &APlayerCharacter::ToggleCrouch);

	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	*/
}

void APlayerCharacter::FellOutOfWorld(const class UDamageType& dmgType)
{
	OnDeath(true);
}

void APlayerCharacter::OnDeath(bool IsFellOut)
{
	// GetWorld()->GetFirstPlayerController()->RestartLevel();

	APlayerController* PlayerController = GetController <APlayerController>();
	if (PlayerController)
	{
		PlayerController->RestartLevel();
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::TakeDamage Damage %.2f"), Damage);
	if (HealthComponent)
	{
		HealthComponent->TakeDamage(Damage);
		if (HealthComponent->IsDead())
		{
			OnDeath(false);
		}
	}
	return Damage;
}

void APlayerCharacter::MoveCameraToArrowLocation(FName ArrowName)
{
	UArrowComponent* ArrowComp = Cast<UArrowComponent>(GetDefaultSubobjectByName(ArrowName));
	FVector NewCameraLocation = ArrowComp->GetRelativeLocation();

	if (FollowCamera != NULL && ArrowComp != nullptr)
	{
		FollowCamera->SetRelativeLocation(NewCameraLocation);
	}
	else
	{
		if (FollowCamera == NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::MoveCameraToArrowLocation FollowCamera is NULL"));
		}
		else if (ArrowComp == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerCharacter::MoveCameraToArrowLocation ArrowComp is NULL, ArrowName might be wrong"));
		}
		
	}
	// FollowCamera->SetRelativeLocation(NewCameraLocation);
}

//--------------------------------------------------------------
// Action Mappings
//--------------------------------------------------------------
/*
void APlayerCharacter::Sprint()
{
	PlayerMoveState = EPlayerMoveState::PMS_Sprinting;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * 1.5f;
	GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration;

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
}

void APlayerCharacter::StopSprinting()
{
	PlayerMoveState = EPlayerMoveState::PMS_Walking;
	GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0;
	GetCharacterMovement()->MaxAcceleration = WalkingMaxAcceleration;

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
}

void APlayerCharacter::HoldCrouch()
{
	PlayerMoveState = EPlayerMoveState::PMS_Crouching;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 48.0f);
}

void APlayerCharacter::StopHoldingCrouch()
{
	PlayerMoveState = EPlayerMoveState::PMS_Walking;
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Emerald, TEXT("PlayerState: ") + UEnum::GetDisplayValueAsText(PlayerMoveState).ToString());
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
}

void APlayerCharacter::ToggleCrouch()
{

}
*/

//--------------------------------------------------------------
// Axis Mappings
//--------------------------------------------------------------

/*
void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0 && Controller != nullptr)
	{
		PlayerMoveState = EPlayerMoveState::PMS_Walking;

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);

		// FString DisplayValue = "MoveForward: " + FString::SanitizeFloat(Value);
		// GEngine->AddOnScreenDebugMessage(1, 0.5, FColor::Emerald, DisplayValue);

	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0 && Controller != nullptr)
	{
		PlayerMoveState = EPlayerMoveState::PMS_Walking;
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);

		// FString DisplayValue = "MoveRight: " + FString::SanitizeFloat(Value);
		// GEngine->AddOnScreenDebugMessage(2, 0.5, FColor::Emerald, DisplayValue);
		
	}
}

void APlayerCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
*/