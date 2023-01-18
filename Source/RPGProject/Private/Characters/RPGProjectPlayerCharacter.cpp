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
#include "Structs/RPGDamageStructs.h"

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

	PopulateCameraSpringArmMap();
	PopulateCameraArrowMap();

	// Create the follow camera
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(CameraSpringArmMap[ECameraView::Exploration], USpringArmComponent::SocketName);
	PlayerCamera->SetRelativeLocation(CameraArrowMap[ECameraView::Exploration]->GetRelativeLocation());
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
			RPGPlayerCameraManager->SetSpringArmForCameraView(ECameraView::Exploration, CameraSpringArmMap[ECameraView::Exploration]);
			RPGPlayerCameraManager->SetSpringArmForCameraView(ECameraView::Action, CameraSpringArmMap[ECameraView::Action]);
			RPGPlayerCameraManager->SetSpringArmForCameraView(ECameraView::Aim, CameraSpringArmMap[ECameraView::Aim]);
			RPGPlayerCameraManager->SetSpringArmForCameraView(ECameraView::LockOn, CameraSpringArmMap[ECameraView::LockOn]);
			RPGPlayerCameraManager->SetSpringArmForCameraView(ECameraView::Skill, CameraSpringArmMap[ECameraView::Skill]);

			RPGPlayerCameraManager->SetArrowForCameraView(ECameraView::Exploration, CameraArrowMap[ECameraView::Exploration]);
			RPGPlayerCameraManager->SetArrowForCameraView(ECameraView::Action, CameraArrowMap[ECameraView::Action]);
			RPGPlayerCameraManager->SetArrowForCameraView(ECameraView::Aim, CameraArrowMap[ECameraView::Aim]);
			RPGPlayerCameraManager->SetArrowForCameraView(ECameraView::LockOn, CameraArrowMap[ECameraView::LockOn]);
			RPGPlayerCameraManager->SetArrowForCameraView(ECameraView::Skill, CameraArrowMap[ECameraView::Skill]);
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
}

// Called every frame
void ARPGProjectPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCurves();

	if (bIsRagdollDeath && HealthComponent)
	{
		HealthComponent->ReduceHealth(HealthComponent->GetMaxHealthPoints());
	}

	if (HealthComponent)
	{
		if (HealthComponent->IsDead())
		{
			GetCharacterMovement()->DisableMovement();
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SetPlayerActionState(EPlayerActionState::Incapacitated);
		}
	}

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

	PlayerHorizontalMobilityUpdate();

	CombatStanceUpdate();

	InteractionTrace();

}

void ARPGProjectPlayerCharacter::EnableDodgeCollision(bool bActive)
{
	if (bActive)
	{
		if (GetMesh())
		{
			GetMesh()->SetCollisionProfileName("CharacterMeshDodge");
		}

		// Disable collision on equipment and weapons
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

		// Enable collision on equipment and weapons
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
		if (RPGPlayerCameraManager->GetCameraView() != ECameraView::Action)
		{
			RPGPlayerCameraManager->SetCameraView(ECameraView::Action);
			bUseControllerRotationYaw = false;
			CombatComponent->SetAimAtCrosshair(false);
		}
		break;

	case EWeaponStanceType::ST_Guard:
		PC->DisablePawnRotation(false);
		break;

	case EWeaponStanceType::ST_MAX:

		break;
	}

	CombatComponent->SetCurrentWeaponStanceType(EWeaponStanceType::ST_None);


}

void ARPGProjectPlayerCharacter::SetWeaponStance(ECombatWeaponStance CombatWeaponStanceType, UItemWeaponData* StanceWeaponData)
{
	if (!CombatComponent) { return; }
	if (CombatWeaponStanceType == ECombatWeaponStance::None) { return; }

	EWeaponStanceType StanceType = EWeaponStanceType::ST_None;
	FWeaponStanceInfo InWeaponStanceInfo;

	switch (CombatWeaponStanceType)
	{
	case ECombatWeaponStance::Mainhand:
		StanceType = StanceWeaponData->MainhandStanceType;
		InWeaponStanceInfo = StanceWeaponData->MainhandStanceInfo;
		break;

	case ECombatWeaponStance::Offhand:
		StanceType = StanceWeaponData->OffhandStanceType;
		InWeaponStanceInfo = StanceWeaponData->OffhandStanceInfo;
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
			RPGPlayerCameraManager->SetCameraView(ECameraView::Aim);
		}
		bUseControllerRotationYaw = true;
		CombatComponent->SetAimAtCrosshair(true);
		break;

	case EWeaponStanceType::ST_Guard:
		if (PC)
		{
			PC->DisablePawnRotation(true);
		}
		break;
	}

	if (StaminaComponent && StanceType != EWeaponStanceType::ST_None)
	{
		StaminaComponent->SetStaminaRegenMultiplier(InWeaponStanceInfo.StaminaRegenMultiplier);
		StaminaComponent->SetStaminaRegenDelayMultiplier(InWeaponStanceInfo.StaminaRegenDelayMultiplier);
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

float ARPGProjectPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!CombatComponent) { return 0.f; }

	// Only deal damage if the character isn't dodging
	if (!CombatComponent->GetIsDodging())
	{
		float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::TakeDamage Damage %.2f"), Damage);
		if (HealthComponent)
		{
			if (DamageEvent.IsOfType(FBodyPartDamageEvent::ClassID))
			{
				FBodyPartDamageEvent const* const BodyPartDamageEvent = (FBodyPartDamageEvent*)(&DamageEvent);

				HealthComponent->ReduceHealth(Damage, BodyPartDamageEvent->BodyPartHit);
			}
			else
			{
				HealthComponent->ReduceHealth(Damage);
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

void ARPGProjectPlayerCharacter::RequestCombatAction()
{
	if (!CombatComponent) { return; }
	if (!EquipmentComponent) { return; }

	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::None:
		CombatComponent->StartNeutralCombatAction();
		break;

	case ECombatWeaponStance::Mainhand:
		CombatComponent->StartStanceCombatAction();
		break;

	case ECombatWeaponStance::Offhand:
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
	case ECombatWeaponStance::None:
		CombatComponent->StopNeutralCombatAction();
		break;

	case ECombatWeaponStance::Mainhand:
		CombatComponent->StopStanceCombatAction();
		break;

	case ECombatWeaponStance::Offhand:
		CombatComponent->StopStanceCombatAction();
		break;

	default:
		break;
	}

	CombatComponent->SetCombatActionIsPressed(false);
}

void ARPGProjectPlayerCharacter::RequestDodge()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated || GetIsInUninterruptableAction() || GetCharacterMovement()->IsFalling()) { return; }
	if (!CombatComponent) { if (CombatComponent->GetIsInAttackRecovery()) { return; } }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }

	CombatComponent->CombatDodge();
}

void ARPGProjectPlayerCharacter::RequestHeavyAttack()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (GetIsInUninterruptableAction()) { return; }

	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::None:
		CombatComponent->CharacterAttack(EAttackType::HeavyAttack);
		break;

	case ECombatWeaponStance::Mainhand:
		// Mainhand Skill #2
		break;

	case ECombatWeaponStance::Offhand:
		// Offhand Skill #2
		break;
	}
}

void ARPGProjectPlayerCharacter::RequestStopHeavyAttack()
{
	// Currently not used, but functionality is in place for special hold actions
}

void ARPGProjectPlayerCharacter::RequestHeavyAttackFinisher()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (GetIsInUninterruptableAction()) { return; }

	CombatComponent->CharacterAttack(EAttackType::HeavyFinisher);
}

void ARPGProjectPlayerCharacter::RequestHoldCrouch()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	if (!GetCharacterMovement()->IsMovingOnGround()) { return; }

	GetCharacterMovement()->bWantsToCrouch = true;

	if (RPGPlayerCameraManager)
	{
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::Exploration)
		{
			CameraSpringArmMap[ECameraView::Exploration]->SetRelativeLocation({ CameraSpringArmMap[ECameraView::Exploration]->GetRelativeLocation().X, CameraSpringArmMap[ECameraView::Exploration]->GetRelativeLocation().Y, 5.f });
		}
	}

	SetPlayerVerticalMobilityState(EPlayerVerticalMobility::Crouching);
}

void ARPGProjectPlayerCharacter::RequestStopCrouching()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }

	GetCharacterMovement()->bWantsToCrouch = false;

	if (RPGPlayerCameraManager)
	{
		if (RPGPlayerCameraManager->GetCameraView() == ECameraView::Exploration)
		{
			CameraSpringArmMap[ECameraView::Exploration]->SetRelativeLocation({ CameraSpringArmMap[ECameraView::Exploration]->GetRelativeLocation().X, CameraSpringArmMap[ECameraView::Exploration]->GetRelativeLocation().Y, 45.f });
		}
	}

	if (GetPlayerVerticalMobilityState() == EPlayerVerticalMobility::Crouching)
	{
		SetPlayerVerticalMobilityState(EPlayerVerticalMobility::Standing);
	}
}

void ARPGProjectPlayerCharacter::RequestToggleCrouch()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }

	if (!GetCharacterMovement()->bWantsToCrouch)
	{
		RequestHoldCrouch();
	}
	else
	{
		RequestStopCrouching();
	}
}

void ARPGProjectPlayerCharacter::RequestInteraction()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }

	if (LookedAtActor)
	{
		if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(LookedAtActor))
		{
			if (AItemBase* ItemCast = Cast<AItemBase>(LookedAtActor))
			{
				ItemCast->OnItemPickup.BindUObject(PC, &ARPGProjectPlayerController::BPCreateNotification);
			}
			InteractionInterface->InteractionRequested(this);
		}
	}
}

void ARPGProjectPlayerCharacter::RequestJump()
{
	if (GetIsInUninterruptableAction() || GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }

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

void ARPGProjectPlayerCharacter::RequestLightAttack()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (GetIsInUninterruptableAction()) { return; }

	switch (CombatComponent->GetCombatWeaponStance())
	{
	case ECombatWeaponStance::None:
		CombatComponent->CharacterAttack(EAttackType::LightAttack);
		break;

	case ECombatWeaponStance::Mainhand:
		// Mainhand Skill #1
		break;

	case ECombatWeaponStance::Offhand:
		// Offhand Skill #1
		break;
	}
}

void ARPGProjectPlayerCharacter::RequestStopLightAttack()
{
	// Currently not used, but functionality is in place for special hold actions
}

void ARPGProjectPlayerCharacter::RequestLightAttackFinisher()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	if (!CombatComponent) { return; }
	if (StaminaComponent) { if (StaminaComponent->IsStaminaExhausted()) { return; } }
	if (GetIsInUninterruptableAction()) { return; }

	CombatComponent->CharacterAttack(EAttackType::LightFinisher);
}

void ARPGProjectPlayerCharacter::RequestMainhandStance()
{
	if (RequestedCombatWeaponStance != ECombatWeaponStance::None) { return; }

	RequestedCombatWeaponStance = ECombatWeaponStance::Mainhand;
}

void ARPGProjectPlayerCharacter::RequestStopMainhandStance()
{
	if (RequestedCombatWeaponStance == ECombatWeaponStance::Mainhand)
	{
		RequestedCombatWeaponStance = ECombatWeaponStance::None;
	}
}

void ARPGProjectPlayerCharacter::RequestOffhandStance()
{
	if (RequestedCombatWeaponStance != ECombatWeaponStance::None) { return; }

	RequestedCombatWeaponStance = ECombatWeaponStance::Offhand;

}

void ARPGProjectPlayerCharacter::RequestStopOffhandStance()
{
	if (RequestedCombatWeaponStance == ECombatWeaponStance::Offhand)
	{
		RequestedCombatWeaponStance = ECombatWeaponStance::None;
	}
}

void ARPGProjectPlayerCharacter::RequestSheatheUnsheatheWeapon()
{
	if (!CombatComponent) { return; }
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated || GetIsInUninterruptableAction()) { return; }

	CombatComponent->ToggleCombatState();
}

void ARPGProjectPlayerCharacter::RequestSprint()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated) { return; }
	SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::Sprinting);
}

void ARPGProjectPlayerCharacter::RequestStopSprinting()
{
	if (GetPlayerHorizontalMobilityState() == EPlayerHorizontalMobility::Sprinting)
	{
		SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility::Jogging);
	}
}

void ARPGProjectPlayerCharacter::RequestSwapWeaponLoadout()
{
	if (GetPlayerActionState() == EPlayerActionState::Incapacitated || GetIsInUninterruptableAction()) { return; }

	if (CombatComponent)
	{
		CombatComponent->SwapWeaponLoadout();
	}
}

void ARPGProjectPlayerCharacter::RequestContextAction()
{
	if (PlayerActionState == EPlayerActionState::Incapacitated) { return; }

	/*if (GetVelocity().Length() > 151.f)
	{
		RequestDodge();
	}
	else
	{
		RequestInteraction();
	}*/
}

void ARPGProjectPlayerCharacter::RequestHoldContextAction()
{
	if (PlayerActionState == EPlayerActionState::Incapacitated) { return; }

	RequestSprint();
}

void ARPGProjectPlayerCharacter::RequestStopContextAction(bool bWasButtonHeld)
{
	if (PlayerActionState == EPlayerActionState::Incapacitated) { return; }

}

void ARPGProjectPlayerCharacter::InteractionTrace()
{
	if (PC)
	{
		FVector Location;
		FRotator Rotation;
		PC->GetPlayerViewPoint(Location, Rotation);

		const FVector PlayerViewForward = Rotation.Vector();
		const float AdditionalDistance = (Location - GetActorLocation()).Size();

		// Set the end position of the interaction trace
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

		EDrawDebugTrace::Type DebugTrace = CVarDisplayTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);

		// Centre of character capsule
		FVector StartPos = Location + (PlayerViewForward * AdditionalDistance);

		// Run the interaction trace, will return an array of blocked interactable actors
		bTraceWasBlocked = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), StartPos, EndPos, SphereCastRadius, InteractionObjectTypeArray, false, ActorsToIgnore, DebugTrace, HitResultArray, true);

		bool bFirstInteractableFound = false;
		if (bTraceWasBlocked)
		{
			for (int i = 0; i < HitResultArray.Num(); i++)
			{
				AActor* HitActor = HitResultArray[i].GetActor();
				if (HitActor)
				{
					HitActorArray.Emplace(HitActor);
					// Check each actor 
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
							// Disable the highlight on the previously detected interactable actor
							if (LookedAtActor != HitActor)
							{
								if (IHighlightInterface* HighlightInterface = Cast<IHighlightInterface>(LookedAtActor))
								{
									HighlightInterface->EnableHighlight(false);
									SetIsInteractionAvailable(false);
								}
							}

							LookedAtActor = HitActor;

							// Highlight the actor if they can be interacted with and are within interactable range
							if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(LookedAtActor))
							{
								bool bWillHightlight = InteractionInterface->GetIsInInteractableRange(this);

								if (InteractionInterface->CanActorBeInteractedWith())
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

							bFirstInteractableFound = true;
							break;
						}
					}
				}

			}

		}

		// If no interactable is found, disable the highlight on the last seen interactable actor
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

void ARPGProjectPlayerCharacter::SetPlayerHorizontalMobilityState(EPlayerHorizontalMobility NewState)
{
	LastPlayerHorizontalMobilityState = PlayerHorizontalMobilityState;
	PlayerHorizontalMobilityState = NewState;
}

void ARPGProjectPlayerCharacter::SetPlayerVerticalMobilityState(EPlayerVerticalMobility NewState)
{
	LastPlayerVerticalMobilityState = PlayerVerticalMobilityState;
	PlayerVerticalMobilityState = NewState;
}

void ARPGProjectPlayerCharacter::SetPlayerActionState(EPlayerActionState NewState)
{
	LastPlayerActionState = PlayerActionState;
	PlayerActionState = NewState;
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
	SetPlayerActionState(EPlayerActionState::Incapacitated);
}

void ARPGProjectPlayerCharacter::OnDeathTimerFinished()
{
	if (PC)
	{
		PC->RestartLevel();
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

void ARPGProjectPlayerCharacter::PlayerHorizontalMobilityUpdate()
{
	// Reduce total speed by the movement speed reduction anim curve
	if (StaminaComponent)
	{
		// Reduce speed even further if the character has exhausted their stamina
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

	switch (GetPlayerHorizontalMobilityState())
	{
	case EPlayerHorizontalMobility::Idle:
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;

		break;
	}
	case EPlayerHorizontalMobility::Walking:
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkMovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;

		break;
	}
	case EPlayerHorizontalMobility::Jogging:
	{
		GetCharacterMovement()->MaxWalkSpeed = MovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MinAnalogWalkSpeed = CharacterMinAnalogWalkSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxAcceleration = NormalMaxAcceleration * TotalSpeedModifier;

		break;
	}
	case EPlayerHorizontalMobility::Sprinting:
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchMovementSpeed * CrouchSprintSpeedModifier * TotalSpeedModifier;
		GetCharacterMovement()->MinAnalogWalkSpeed = GetCharacterMovement()->MaxWalkSpeed * TotalSpeedModifier;
		GetCharacterMovement()->MaxAcceleration = SprintingMaxAcceleration * TotalSpeedModifier;

		if (CurrentCharacterXYVelocity > (MovementSpeed * TotalSpeedModifier) && GetCharacterMovement()->IsMovingOnGround())
		{
			if (StaminaComponent)
			{
				StaminaComponent->DrainStaminaPerSecond(EStaminaDrainType::Sprint);
			}
		}

		break;
	}
	}
}

void ARPGProjectPlayerCharacter::CombatStanceUpdate()
{
	if (!CombatComponent) { return; }
	if (!EquipmentComponent) { return; }

	if (CombatComponent->GetCombatState() == ECombatState::CombatReady)
	{
		// Set the character's weapons stance if they are not in their requested stance
		if (CombatComponent->GetCombatWeaponStance() != RequestedCombatWeaponStance)
		{
			if (RequestedCombatWeaponStance == ECombatWeaponStance::None)
			{
				CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::None);
				ResetWeaponStance();

				return;
			}

			if (CombatComponent->GetCurrentWeaponStanceType() != EWeaponStanceType::ST_None) { ResetWeaponStance(); }

			UItemWeaponData* StanceWeaponData = nullptr;

			switch (RequestedCombatWeaponStance)
			{
			case ECombatWeaponStance::Mainhand:
				if (EquipmentComponent->GetMainhandWeaponData())
				{
					StanceWeaponData = EquipmentComponent->GetMainhandWeaponData();
				}
				break;

			case ECombatWeaponStance::Offhand:
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
	}
	else
	{
		// Exit any weapon stance the character is currently in
		if (CombatComponent->GetCombatWeaponStance() != ECombatWeaponStance::None)
		{
			CombatComponent->SetCombatWeaponStance(ECombatWeaponStance::None);
			ResetWeaponStance();
		}
	}
}

void ARPGProjectPlayerCharacter::CheckCharacterExhaustion()
{
	if (!StaminaComponent) { return; }
	if (!CombatComponent) { return; }

	if (StaminaComponent->IsStaminaExhausted() && !CombatComponent->GetIsGuarding() && GetVelocity().Length() <= 0.f)
	{
		RequestStopCrouching();
		CombatComponent->StaminaExhausted();
	}
	else if (StaminaComponent->IsStaminaExhausted() && GetVelocity().Length() >= 0.f)
	{
		StaminaComponent->ResetStaminaRegenDelay();
	}
}

void ARPGProjectPlayerCharacter::PopulateHealthComponentHitboxMap()
{
	if (HealthComponent)
	{
		HealthComponent->AddToBodyPartMap("Head", { {"head", "neck_01"}, .1f, 100.f, 100.f, 1.25f, true, true });
		HealthComponent->AddToBodyPartMap("Torso", { {"pelvis", "spine_01", "spine_02"}, .3f, 300.f, 300.f, 1.f, true , true });
		HealthComponent->AddToBodyPartMap("Left Arm", { {"upperarm_l", "lowerarm_l", "hand_l"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Right Arm", { {"upperarm_r", "lowerarm_r", "hand_r"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Left Leg", { {"thigh_l", "calf_l", "foot_l", "ball_l"}, .15f, 150.f, 150.f, 1.f, false, true });
		HealthComponent->AddToBodyPartMap("Right Leg", { {"thigh_r", "calf_r", "foot_r", "ball_r"}, .15f, 150.f, 150.f, 1.f, false, true });
	}
}

void ARPGProjectPlayerCharacter::PopulateCameraSpringArmMap()
{
	CameraSpringArmMap.Emplace(ECameraView::Exploration, CreateDefaultSubobject<USpringArmComponent>(TEXT("Exploration_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::Action, CreateDefaultSubobject<USpringArmComponent>(TEXT("Action_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::Aim, CreateDefaultSubobject<USpringArmComponent>(TEXT("Aim_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::LockOn, CreateDefaultSubobject<USpringArmComponent>(TEXT("LockOn_SpringArm")));
	CameraSpringArmMap.Emplace(ECameraView::Skill, CameraSpringArmMap[ECameraView::Action]);

	for (uint8 i = 1; i < (uint8)ECameraView::MAX; i++)
	{

		if (CameraSpringArmMap.Contains((ECameraView)i))
		{
			CameraSpringArmMap[(ECameraView)i]->SetupAttachment(RootComponent);
			CameraSpringArmMap[(ECameraView)i]->bUsePawnControlRotation = true;
			CameraSpringArmMap[(ECameraView)i]->bInheritRoll = false;
		}
	}

	CameraSpringArmMap[ECameraView::Exploration]->SetRelativeLocation({ 0.f, 0.f, 45.f });
	CameraSpringArmMap[ECameraView::Exploration]->TargetArmLength = 300.f;
	CameraSpringArmMap[ECameraView::Exploration]->bEnableCameraLag = true;

	CameraSpringArmMap[ECameraView::Action]->SetRelativeLocation({ 0.f, 0.f, 40.f });
	CameraSpringArmMap[ECameraView::Action]->TargetArmLength = 450.f;
	CameraSpringArmMap[ECameraView::Action]->bEnableCameraLag = false;

	CameraSpringArmMap[ECameraView::Aim]->SetRelativeLocation({ 0.f, 80.f, 75.f });
	CameraSpringArmMap[ECameraView::Aim]->TargetArmLength = 200.f;
	CameraSpringArmMap[ECameraView::Aim]->bEnableCameraLag = false;

	CameraSpringArmMap[ECameraView::LockOn]->SetRelativeLocation({ 0.f, 0.f, 95.f });
	CameraSpringArmMap[ECameraView::LockOn]->TargetArmLength = 500.f;
	CameraSpringArmMap[ECameraView::LockOn]->bEnableCameraLag = false;
	CameraSpringArmMap[ECameraView::LockOn]->bEnableCameraRotationLag = true;
	CameraSpringArmMap[ECameraView::LockOn]->CameraRotationLagSpeed = 10.f;
}

void ARPGProjectPlayerCharacter::PopulateCameraArrowMap()
{
	CameraArrowMap.Emplace(ECameraView::Exploration, CreateDefaultSubobject<UArrowComponent>(TEXT("Exploration_Arrow")));
	CameraArrowMap.Emplace(ECameraView::Action, CreateDefaultSubobject<UArrowComponent>(TEXT("Action_Arrow")));
	CameraArrowMap.Emplace(ECameraView::Aim, CreateDefaultSubobject<UArrowComponent>(TEXT("Aim_Arrow")));
	CameraArrowMap.Emplace(ECameraView::LockOn, CreateDefaultSubobject<UArrowComponent>(TEXT("LockOn_Arrow")));
	CameraArrowMap.Emplace(ECameraView::Skill, CreateDefaultSubobject<UArrowComponent>(TEXT("Skill_Arrow")));

	for (uint8 i = 1; i < (uint8)ECameraView::MAX; i++)
	{
		if (CameraArrowMap.Contains((ECameraView)i))
		{

			if (i != (uint8)ECameraView::Skill)
			{
				CameraArrowMap[(ECameraView)i]->SetupAttachment(CameraSpringArmMap[(ECameraView)i], USpringArmComponent::SocketName);
			}
			else
			{
				CameraArrowMap[(ECameraView)i]->SetupAttachment(CameraSpringArmMap[ECameraView::Action], USpringArmComponent::SocketName);
			}
			CameraSpringArmMap[ECameraView::Exploration]->SetRelativeLocation({ 0.f, 0.f, 0.f });
			CameraArrowMap[(ECameraView)i]->ArrowLength = 40.f;
		}
	}
}

