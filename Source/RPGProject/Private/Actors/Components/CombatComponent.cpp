// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Controllers/RPGProjectPlayerController.h"
#include "DataAssets/HitFXData.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Interfaces/HitFXInterface.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/ProjectileActor.h"
#include "Structs/RPGDamageStructs.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AttackCount;
	
	CurrentAttackType = EAttackType::None;
	DodgeType = EDodgeType::None;
	CombatState = ECombatState::AtEase;

	QueueAttackWindowCurve = 0.0f;
	EnableHitCurve = 0.0f;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<ACharacter>(GetOwner());
	RPGProjectCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef);

	if (CharacterRef) 
	{ 
		PC = Cast<ARPGProjectPlayerController>(CharacterRef->GetController());

		MeshComponentRef = CharacterRef->GetMesh(); 

		if (MeshComponentRef) { CharacterAnimInstance = Cast<URPGProjectAnimInstance>(MeshComponentRef->GetAnimInstance()); }

		if (PC)
		{
			RPGPlayerCameraManagerRef = PC->GetRPGPlayerCameraManager();
		}

		EquipmentComponentRef = Cast<UEquipmentComponent>(CharacterRef->GetComponentByClass(UEquipmentComponent::StaticClass()));
		HealthComponentRef = Cast<UHealthComponent>(CharacterRef->GetComponentByClass(UEquipmentComponent::StaticClass()));
		StaminaComponentRef = Cast <UStaminaComponent>(CharacterRef->GetComponentByClass(UStaminaComponent::StaticClass()));
	}

	if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnUnsheatheWeapon.AddDynamic(this, &UCombatComponent::OnUnsheatheWeaponNotifyReceived);
		CharacterAnimInstance->OnSheatheWeapon.AddDynamic(this, &UCombatComponent::OnSheatheWeaponNotifyReceived);

		CharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
		CharacterAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
		CharacterAnimInstance->OnPlayAttackSFX.AddDynamic(this, &UCombatComponent::OnPlayAttackSFXNotifyReceived);

		CharacterAnimInstance->OnReloadWeapon.AddDynamic(this, &UCombatComponent::OnReloadWeaponNotifyReceived);
	}
}

void UCombatComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnUnsheatheWeapon.RemoveDynamic(this, &UCombatComponent::OnUnsheatheWeaponNotifyReceived);
		CharacterAnimInstance->OnSheatheWeapon.RemoveDynamic(this, &UCombatComponent::OnSheatheWeaponNotifyReceived);

		CharacterAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
		CharacterAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
		CharacterAnimInstance->OnPlayAttackSFX.RemoveDynamic(this, &UCombatComponent::OnPlayAttackSFXNotifyReceived);

		CharacterAnimInstance->OnReloadWeapon.RemoveDynamic(this, &UCombatComponent::OnReloadWeaponNotifyReceived);
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (EquipmentComponentRef)
	{
		MainhandWeaponRef = EquipmentComponentRef->GetMainhandWeaponActor();
		OffhandWeaponRef = EquipmentComponentRef->GetOffhandWeaponActor();

		if (!MainhandWeaponRef && !OffhandWeaponRef)
		{
			//OnSheathingMontageEnded(nullptr, false);
		}
	}

	UpdateCurveFloats();
	UpdateCombatBooleans();
	AttackTracing();
	UpdateGuardState();
	StaminaExhaustionUpdate();

}

void UCombatComponent::CharacterAttack(EAttackType CharacterAttackType)
{
	if (CombatState != ECombatState::CombatReady) { return; }
	if (!bCanAttack) { return; }
	if (bIsAttackQueued && (CharacterAttackType != EAttackType::LightFinisher && CharacterAttackType != EAttackType::HeavyFinisher)) { return; }

	if (!EquipmentComponentRef) { return; }
	if (EquipmentComponentRef) { if (!EquipmentComponentRef->GetMainhandWeaponData()) { return; } }

	if (CharacterAttackType == EAttackType::LightAttack && EquipmentComponentRef->GetMainhandWeaponData()->LightAttackComboLimit <= 0) { return; }
	if (CharacterAttackType == EAttackType::HeavyAttack && EquipmentComponentRef->GetMainhandWeaponData()->HeavyAttackComboLimit <= 0) { return; }

	if (DoesAttackNeedAmmunition(EWeaponToUse::Mainhand, CharacterAttackType))
	{
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::CharacterAttack EndAttackSequence called."));
		if (bIsInAttackSequence) { EndAttackSequence(); }

		if (AItemWeaponRanged* MainhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor()))
		{
			WeaponReloadMontage = MainhandWeaponRanged->GetItemWeaponRangedData()->MainhandReloadMontage;
			if (WeaponReloadMontage) { ReloadWeapon(EWeaponToUse::Mainhand); }
		}
		return; 
	}

	WeaponAttackMontage = EquipmentComponentRef->GetMainhandWeaponData()->AttackMontage;

	if (!bIsInAttackSequence && !bIsAttackQueued)
	{
		bIsAttackQueued = true;
		NextAttackType = CharacterAttackType;
		StartAttackSequence(NextAttackType);
	}
	else if (CharacterAttackType == EAttackType::LightFinisher || CharacterAttackType == EAttackType::HeavyFinisher)
	{
		bIsFinisherQueued = true;
		FinisherType = CharacterAttackType;
	}
	else if (bIsInAttackWindow && !bIsAttackQueued)
	{
		bIsAttackQueued = true;
		NextAttackType = CharacterAttackType;
	}
}

void UCombatComponent::StartAttackSequence(EAttackType SequenceAttackType)
{
	if (CombatState != ECombatState::CombatReady) { return; }
	if (bIsInAttackSequence) { return; }

	if (!CharacterRef) { return; }
	//if (!StaminaComponentRef) { return; }

	if (CharacterAnimInstance) 
	{
		if (CharacterAnimInstance->GetCurrentActiveMontage() == WeaponAttackMontage) { return; }
	}

	switch (SequenceAttackType)
	{
	case EAttackType::LightAttack:
		CurrentSectionInMontage = (FName)("Light_Attack_" + FString::FromInt(AttackCount + 1));
		//StaminaComponentRef->ReduceCurrentStamina(20.f);
		break;
	case EAttackType::HeavyAttack:
		CurrentSectionInMontage = (FName)("Heavy_Attack_" + FString::FromInt(AttackCount + 1));
		//StaminaComponentRef->ReduceCurrentStamina(40.f);
		break;
	default:
		break;
	}

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(WeaponAttackMontage, PlayRate, CurrentSectionInMontage) > 0.f;

	if (bPlayedSuccessfully)
	{
		bIsAttackQueued = false;
		bIsInAttackSequence = true;
		bIsInAttackWindUp = true;
		AttackCount = 1;
		CurrentAttackType = SequenceAttackType;

		if (CharacterAnimInstance)
		{
			if (!WeaponAttackBlendingOutDelegate.IsBound())
			{
				WeaponAttackBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(WeaponAttackBlendingOutDelegate, WeaponAttackMontage);

			if (!WeaponAttackMontageEndedDelegate.IsBound())
			{
				WeaponAttackMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(WeaponAttackMontageEndedDelegate, WeaponAttackMontage);

			/*CharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
			CharacterAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
			CharacterAnimInstance->OnPlayAttackSFX.AddDynamic(this, &UCombatComponent::OnPlayAttackSFXNotifyReceived);*/
		}
	}
}

void UCombatComponent::StartNeutralCombatAction()
{
	
}

void UCombatComponent::StopNeutralCombatAction()
{

}

void UCombatComponent::StartStanceCombatAction()
{
	if (!EquipmentComponentRef) { return; }
	if (CombatState != ECombatState::CombatReady) { return; }
	if (bIsInAttackSequence) { return; }

	if (!CharacterRef) { return; }
	if (!CharacterAnimInstance) { return; }

	UItemWeaponData* CombatActionWeaponData = nullptr;
	UItemWeaponData* MainhandWeaponData = EquipmentComponentRef->GetMainhandWeaponData();
	UItemWeaponData* OffhandWeaponData = EquipmentComponentRef->GetOffhandWeaponData();

	FName MontageStartingSection = "";

	switch (CombatWeaponStance)
	{
	case ECombatWeaponStance::Mainhand:
		if (MainhandWeaponData)
		{
			CombatActionWeaponData = MainhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->MainhandStanceCombatActionInfo;
		}
		else if (OffhandWeaponData)
		{
			CombatActionWeaponData = OffhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->MainhandStanceCombatActionInfo;
		}
		break;
	
	case ECombatWeaponStance::Offhand:
		if (OffhandWeaponData)
		{
			CombatActionWeaponData = OffhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->OffhandStanceCombatActionInfo;
		}
		else if (MainhandWeaponData)
		{
			CombatActionWeaponData = MainhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->OffhandStanceCombatActionInfo;
		}
		break;

	default:
		break;
	}

	if (CharacterAnimInstance->GetCurrentActiveMontage() != nullptr /*== CombatActionInfo.CombatActionMontage*/) { return; }

	if (CombatActionInfo.bRequiresAmmo)
	{
		if (UItemWeaponRangedData* CombatActionWeaponRangedData = Cast<UItemWeaponRangedData>(CombatActionWeaponData))
		{
			if (CombatActionWeaponRangedData->AmmoInMagazine < CombatActionInfo.AmmoRequired && CombatActionInfo.CombatActionMontage->IsValidSectionName("Reload"))
			{
				MontageStartingSection = "Reload";
				bCombatActionNeedsToReload = true;
			}
		}
	}


	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(CombatActionInfo.CombatActionMontage, PlayRate, MontageStartingSection) > 0.f;

	if (bPlayedSuccessfully)
	{
		if (!CombatActionBlendingOutDelegate.IsBound())
		{
			CombatActionBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnCombatActionMontageBlendingOut);
		}
		CharacterAnimInstance->Montage_SetBlendingOutDelegate(CombatActionBlendingOutDelegate, CombatActionInfo.CombatActionMontage);

		if (!CombatActionMontageEndedDelegate.IsBound())
		{
			CombatActionMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnCombatActionEnded);
		}
		CharacterAnimInstance->Montage_SetEndDelegate(CombatActionMontageEndedDelegate, CombatActionInfo.CombatActionMontage);
	}
}

void UCombatComponent::StopStanceCombatAction()
{
	if (!EquipmentComponentRef) { return; }
	//if (CombatState != ECombatState::CombatReady) { return; }
	//if (bIsInAttackSequence) { return; }
	if (!CharacterRef) { return; }
	if (!CharacterAnimInstance) { return; }

	//FCombatActionInfo CombatActionInfo;

	AItemWeapon* CombatActionWeaponActor = nullptr;
	UItemWeaponData* CombatActionWeaponData = nullptr;

	AItemWeapon* MainhandWeaponActor = EquipmentComponentRef->GetMainhandWeaponActor();
	UItemWeaponData* MainhandWeaponData = EquipmentComponentRef->GetMainhandWeaponData();

	AItemWeapon* OffhandWeaponActor = EquipmentComponentRef->GetOffhandWeaponActor();
	UItemWeaponData* OffhandWeaponData = EquipmentComponentRef->GetOffhandWeaponData();

	switch (CombatWeaponStance)
	{
	case ECombatWeaponStance::Mainhand:
		if (MainhandWeaponData && MainhandWeaponActor)
		{
			CombatActionWeaponActor = MainhandWeaponActor;
			CombatActionWeaponData = MainhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->MainhandStanceCombatActionInfo;
		}
		else if (OffhandWeaponData && OffhandWeaponActor)
		{
			CombatActionWeaponActor = OffhandWeaponActor;
			CombatActionWeaponData = OffhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->MainhandStanceCombatActionInfo;
		}
		break;

	case ECombatWeaponStance::Offhand:
		if (OffhandWeaponData && OffhandWeaponActor)
		{
			CombatActionWeaponActor = OffhandWeaponActor;
			CombatActionWeaponData = OffhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->OffhandStanceCombatActionInfo;
		}
		else if (MainhandWeaponData && MainhandWeaponActor)
		{
			CombatActionWeaponActor = MainhandWeaponActor;
			CombatActionWeaponData = MainhandWeaponData;
			CombatActionInfo = CombatActionWeaponData->OffhandStanceCombatActionInfo;
		}
		break;

	default:
		break;
	}

	UAnimMontage* CurrentMontage = CharacterAnimInstance->GetCurrentActiveMontage();

	if (CurrentMontage == CombatActionInfo.CombatActionMontage)
	{
		if (CombatActionInfo.bRequiresAmmo)
		{
			if (AItemWeaponRanged* CombatActionWeaponRangedActor = Cast<AItemWeaponRanged>(CombatActionWeaponActor))
			{
				UItemWeaponRangedData* CombatActionWeaponRangedData = Cast<UItemWeaponRangedData>(CombatActionWeaponData);

				if (CharacterAnimInstance->GetCurveValue("EnableFire") >= 1.f && CombatActionWeaponRangedData->AmmoInMagazine > 0.f)
				{
					if (CharacterAnimInstance->Montage_GetCurrentSection(CurrentMontage) != "Reload" && CurrentMontage->IsValidSectionName("Release"))
					{
						// Check if in sweetspot
						CombatActionWeaponRangedActor->SetEnableDrawSweetSpotCurve(CharacterAnimInstance->GetCurveValue("EnableDrawSweetSpot") >= 1.f);

						// Get Projectile Strength
						CombatActionWeaponRangedActor->SetProjectileStrengthScaleCurve(CharacterAnimInstance->GetCurveValue("ProjectileStrengthScale"));

						CharacterAnimInstance->Montage_JumpToSection("Release");
					}
				}
				else if (/*CombatActionWeaponRangedData->AmmoInMagazine <= 0.f && */CharacterAnimInstance->Montage_GetCurrentSection(CurrentMontage) != "Reload")
				{
					CharacterAnimInstance->Montage_Stop(0.25f, CurrentMontage);
				}
			}
		}
	}


}

void UCombatComponent::ReloadWeapon(EWeaponToUse WeaponToReload)
{
	if (!CharacterRef) { return; }
	if (!CharacterAnimInstance) { return; }

	if (CharacterAnimInstance->GetCurrentActiveMontage() != nullptr) { return; }

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(WeaponReloadMontage, PlayRate) > 0.f;

	if (bPlayedSuccessfully)
	{
		bCanAttack = false;

		if (CharacterAnimInstance)
		{
			if (!ReloadMontageBlendingOutDelegate.IsBound())
			{
				ReloadMontageBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnReloadMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(ReloadMontageBlendingOutDelegate, WeaponReloadMontage);

			if (!ReloadMontageEndedDelegate.IsBound())
			{
				ReloadMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnReloadMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(ReloadMontageEndedDelegate, WeaponReloadMontage);

			//CharacterAnimInstance->OnReloadWeapon.AddDynamic(this, &UCombatComponent::OnReloadWeaponNotifyReceived);
		}
	}
}

void UCombatComponent::ToggleCombatState()
{
	if (CharacterAnimInstance->GetCurrentActiveMontage() == SheatheMontage || CharacterAnimInstance->GetCurrentActiveMontage() == UnsheatheMontage) { return ; }
	switch (CombatState)
	{
	case ECombatState::AtEase:
		UnsheatheWeapon();
		break;
	case ECombatState::CombatReady:
		SheatheWeapon();
		break;
	default:
		break;
	}
}

bool UCombatComponent::UnsheatheWeapon()
{
	if (!CharacterRef) { return false; }
	if (!CharacterAnimInstance) { return false; }
	if (!MeshComponentRef) { return false; }
	if (!EquipmentComponentRef) { return false; }
	if (bIsInAttackSequence) { return false; }

	if (CharacterAnimInstance->GetCurrentActiveMontage() == SheatheMontage && !bSwapWeaponLoadout) { return false; }

	FName EquippedWeaponUnsheatheSection = "";
	//FName EquippedWeaponUnsheatheSection = (EquipmentComponentRef->GetMainhandWeaponData() /*GetEquippedWeaponData()*/ != nullptr) ? /*GetEquippedWeaponData()*/EquipmentComponentRef->GetMainhandWeaponData()->WeaponUnsheatheMontageSection : "";

	if (EquipmentComponentRef->GetMainhandWeaponData() && EquipmentComponentRef->GetOffhandWeaponData())
	{
		EquippedWeaponUnsheatheSection = "UnsheatheDualBack";
	}
	else if (EquipmentComponentRef->GetMainhandWeaponData())
	{
		EquippedWeaponUnsheatheSection = EquipmentComponentRef->GetMainhandWeaponData()->WeaponUnsheatheMontageSection;
	}
	else if (EquipmentComponentRef->GetOffhandWeaponData())
	{
		EquippedWeaponUnsheatheSection = EquipmentComponentRef->GetOffhandWeaponData()->WeaponUnsheatheMontageSection;
	}
	else
	{
		return false;
	}

	bMainHandSheathed = true;
	bOffHandSheathed = true;

	//if (EquippedWeaponUnsheatheSection == "") { return false; }

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(UnsheatheMontage, PlayRate, EquippedWeaponUnsheatheSection) > 0.f;
	
	if (bPlayedSuccessfully)
	{
		bCanAttack = false;

		if (CharacterAnimInstance)
		{
			if (!UnSheatheBlendingOutDelegate.IsBound())
			{
				UnSheatheBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnUnsheatheMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(UnSheatheBlendingOutDelegate, UnsheatheMontage);

			if (!UnSheatheMontageEndedDelegate.IsBound())
			{
				UnSheatheMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnUnsheatheMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(UnSheatheMontageEndedDelegate, UnsheatheMontage);

			/*if (!CharacterAnimInstance->OnUnsheatheWeapon.IsBound())
			{
				CharacterAnimInstance->OnUnsheatheWeapon.AddDynamic(this, &UCombatComponent::OnUnsheatheWeaponNotifyReceived);
			}*/
		}
	}

	return bPlayedSuccessfully;
}

bool UCombatComponent::SheatheWeapon()
{
	if (!CharacterRef) { return false; }
	if (!CharacterAnimInstance) { return false; }
	if (!MeshComponentRef) { return false; }
	if (!EquipmentComponentRef) { return false; }
	if (bIsInAttackSequence) { return false; }

	if (CharacterAnimInstance->GetCurrentActiveMontage() == UnsheatheMontage && !bSwapWeaponLoadout) { return false; }

	FName EquippedWeaponSheatheSection = "";
	//EquippedWeaponSheatheSection = (/*GetEquippedWeaponData()*/ EquipmentComponentRef->GetMainhandWeaponData() != nullptr) ? /*GetEquippedWeaponData()*/ EquipmentComponentRef->GetMainhandWeaponData()->WeaponSheatheMontageSection : "";

	if (EquipmentComponentRef->GetMainhandWeaponData() && EquipmentComponentRef->GetOffhandWeaponData())
	{
		EquippedWeaponSheatheSection = "SheatheDualBack";
		bMainHandSheathed = false;
		bOffHandSheathed = false;
	}
	else if (EquipmentComponentRef->GetMainhandWeaponData())
	{
		EquippedWeaponSheatheSection = EquipmentComponentRef->GetMainhandWeaponData()->WeaponSheatheMontageSection;
		bMainHandSheathed = false;
		bOffHandSheathed = true;
	}
	else if (EquipmentComponentRef->GetOffhandWeaponData())
	{
		EquippedWeaponSheatheSection = EquipmentComponentRef->GetOffhandWeaponData()->WeaponSheatheMontageSection;
		bMainHandSheathed = true;
		bOffHandSheathed = false;
	}

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(SheatheMontage, PlayRate, EquippedWeaponSheatheSection) > 0.f;

	if (bPlayedSuccessfully)
	{
		bCanAttack = false;

		if (CharacterAnimInstance)
		{
			if (!SheatheBlendingOutDelegate.IsBound())
			{
				SheatheBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(SheatheBlendingOutDelegate, SheatheMontage);

			if (!SheatheMontageEndedDelegate.IsBound())
			{
				SheatheMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(SheatheMontageEndedDelegate, SheatheMontage);

			/*if (!CharacterAnimInstance->OnSheatheWeapon.IsBound())
			{
				CharacterAnimInstance->OnSheatheWeapon.AddDynamic(this, &UCombatComponent::OnSheatheWeaponNotifyReceived);
			}*/
		}
	}

	return bPlayedSuccessfully;
}

bool UCombatComponent::CombatDodge()
{
	if (!CharacterRef) { return false; }
	if (!CharacterAnimInstance) { return false; }
	if (!MeshComponentRef) { return false; }
	if (bIsInActionRecovery) { return false; }

	if (CharacterAnimInstance->GetCurrentActiveMontage() == DodgeMontage) { return false; }

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(DodgeMontage, PlayRate) > 0.f;

	if (bPlayedSuccessfully)
	{
		bCanAttack = false;

		if (ARPGProjectPlayerCharacter* RPGPlayerCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef))
		{
			RPGPlayerCharacterRef->SetIsInUninterruptableAction(true);
			RPGPlayerCharacterRef->SetPlayerActionState(EPlayerActionState::Dodging);
		}

		if (CharacterAnimInstance)
		{
			if (!DodgeMontageBlendingOutDelegate.IsBound())
			{
				DodgeMontageBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnDodgeMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(DodgeMontageBlendingOutDelegate, DodgeMontage);

			if (!DodgeMontageEndedDelegate.IsBound())
			{
				DodgeMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnDodgeMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(DodgeMontageEndedDelegate, DodgeMontage);
		}
	}
	return bPlayedSuccessfully;
}

bool UCombatComponent::StaminaExhausted(EStaminaExhaustionType StaminaExhaustionType)
{
	if (!CharacterRef) { return false; }
	if (!MeshComponentRef) { return false; }
	if (!bIsInAttackWindUp && bIsInActionRecovery) { return false; }

	if (CharacterAnimInstance->GetCurrentActiveMontage() == StaminaExhaustedMontage) { return false; }

	FName MontageStartingSection = NAME_None;

	switch (StaminaExhaustionType)
	{
	case EStaminaExhaustionType::GuardBreak:
		if (StaminaExhaustedMontage->IsValidSectionName("Guard_Break"))
		{
			MontageStartingSection = "Guard_Break";
		}
		break;

	default:
		break;
	}

	

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(StaminaExhaustedMontage, PlayRate, MontageStartingSection) > 0.f;

	if (bPlayedSuccessfully)
	{
		bCanAttack = false;

		if (ARPGProjectPlayerCharacter* RPGPlayerCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef))
		{
			RPGPlayerCharacterRef->SetIsInUninterruptableAction(true);
		}

		if (CharacterAnimInstance)
		{
			if (!StaminaExhaustedMontageBlendingOutDelegate.IsBound())
			{
				StaminaExhaustedMontageBlendingOutDelegate.BindUObject(this, &UCombatComponent::OnStaminaExhaustedMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(StaminaExhaustedMontageBlendingOutDelegate, StaminaExhaustedMontage);

			if (!StaminaExhaustedMontageEndedDelegate.IsBound())
			{
				StaminaExhaustedMontageEndedDelegate.BindUObject(this, &UCombatComponent::OnStaminaExhaustedMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(StaminaExhaustedMontageEndedDelegate, StaminaExhaustedMontage);
		}
	}
	return bPlayedSuccessfully;
}

void UCombatComponent::SwapWeaponLoadout()
{
	if (!EquipmentComponentRef) { return; }

	if (GetCombatState() == ECombatState::CombatReady)
	{
		// Queue weapon swap at end of sheath montage
		//bSwapWeaponLoadout = true;

		// Sheathe weapon loadout
		bSwapWeaponLoadout = SheatheWeapon();

	}
	else
	{
		// Swap to other weapon loadout
		if (EquipmentComponentRef->GetCurrentWeaponLoadout() == EWeaponLoadout::WL_LoadoutOne)
		{
			EquipmentComponentRef->SetCurrentWeaponLoadout(EWeaponLoadout::WL_LoadoutTwo);
		}
		else
		{
			EquipmentComponentRef->SetCurrentWeaponLoadout(EWeaponLoadout::WL_LoadoutOne);
		}
	}
}

void UCombatComponent::EvaluateHitResult(FHitResult InHitResult, AItemWeapon* InItemWeapon, AProjectileActor* InProjectileActor)
{
	//FString ActorName = InHitResult.GetActor()->GetName();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, ActorName);


	// Get CombatComponent

	UCombatComponent* HitCombatComponentRef = nullptr;

	if (InHitResult.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()))
	{
		HitCombatComponentRef = Cast<UCombatComponent>(InHitResult.GetActor()->GetComponentByClass(UCombatComponent::StaticClass()));
	}
	else if (InHitResult.GetActor()->GetParentActor())
	{
		if (InHitResult.GetActor()->GetParentActor()->GetComponentByClass(UCombatComponent::StaticClass()))
		{
			HitCombatComponentRef = Cast<UCombatComponent>(InHitResult.GetActor()->GetParentActor()->GetComponentByClass(UCombatComponent::StaticClass()));
		}
	}

	// Check if attack was blocked, prevent damage if so
	
	bool bBlockAttack = false;

	if (HitCombatComponentRef)
	{
		const bool bIsTargetGuarding = HitCombatComponentRef->GetIsGuarding();
		const ECollisionChannel HitComponentObjectType = InHitResult.GetComponent()->GetCollisionObjectType();

		if (!bBlockAttack && bIsTargetGuarding && HitComponentObjectType == ECC_GameTraceChannel8)
		{
			bBlockAttack = true;
		}

		if (!bBlockAttack && bIsTargetGuarding) //HitCombatComponentRef->GetIsGuarding() && InHitResult.GetComponent()->GetCollisionObjectType() == ECC_GameTraceChannel8)
		{
			bBlockAttack = HitCombatComponentRef->IsAttackFromBlockedAngle(GetOwner());
		}

		if (bBlockAttack && !HitActorArray.Contains(InHitResult.GetActor()->GetParentActor()))
		{
			HitActorArray.AddUnique(InHitResult.GetActor());
			
			FWeaponAttackInfo WeaponAttackInfo;

			if (InItemWeapon)
			{
				WeaponAttackInfo = GetCurrentWeaponAttackInfo(InItemWeapon);
			}

			HitCombatComponentRef->AttackBlocked(WeaponAttackInfo, InItemWeapon, CurrentAttackType, InProjectileActor);

			const float DurabilityCost = InItemWeapon->GetItemWeaponData()->ItemCurrentDurability - 0.5f;
			InItemWeapon->SetDurability(DurabilityCost);

			HitActorArray.AddUnique(InHitResult.GetActor()->GetParentActor());

			if (InItemWeapon) { PlayHitFX(InHitResult, InItemWeapon, true, EPhysicalSurface::SurfaceType3); }
			else if (InProjectileActor) { PlayHitFX(InHitResult, InProjectileActor); }
			else { PlayHitFX(InHitResult, nullptr, true, EPhysicalSurface::SurfaceType3); }

			return;
		}
	}

	if (AItemWeapon* ItemWeaponRef = Cast<AItemWeapon>(InHitResult.GetActor()))
	{
		return;
	}

	// Play relevant SFX and VFX (if any)
	if (InItemWeapon) { PlayHitFX(InHitResult, InItemWeapon); }
	else if (InProjectileActor) { PlayHitFX(InHitResult, InProjectileActor); }
	else { PlayHitFX(InHitResult, nullptr, true, EPhysicalSurface::SurfaceType3); }

	// Calculate damage

	UHealthComponent* HitActorHealthComponent = Cast<UHealthComponent>(InHitResult.GetActor()->GetComponentByClass(UHealthComponent::StaticClass()));
	UItemWeaponData* WeaponData = nullptr;
	FWeaponAttackInfo CurrentWeaponAttackInfo;

	if (InItemWeapon) 
	{ 
		WeaponData = InItemWeapon->GetItemWeaponData(); 
		CurrentWeaponAttackInfo = GetCurrentWeaponAttackInfo(InItemWeapon);
	}

	float AttackDamage = 0.f;

	if (InProjectileActor)
	{
		if (InProjectileActor->GetProjectileOwner() == InItemWeapon)
		{
			if (AItemWeaponRanged* InItemWeaponRanged = Cast<AItemWeaponRanged>(InItemWeapon))
			{
				UItemWeaponRangedData* WeaponRangedData = InItemWeaponRanged->GetItemWeaponRangedData();

				AttackDamage = truncf((InProjectileActor->GetProjectileDamage() + WeaponRangedData->MinimumWeaponDamage + UKismetMathLibrary::RandomIntegerInRange(0, WeaponRangedData->WeaponDamageVariance)) * InProjectileActor->GetProjectileStrengthMultiplier());
			}
		}
		else
		{
			AttackDamage = InProjectileActor->GetProjectileDamage() * InProjectileActor->GetProjectileStrengthMultiplier();
		}
	}
	else if (WeaponData)
	{
		AttackDamage = truncf(CurrentWeaponAttackInfo.AttackDamage + UKismetMathLibrary::RandomIntegerInRange(0, WeaponData->WeaponDamageVariance));
	}
	
	// Deal damage to hit actor

	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FBodyPartDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = ValidDamageTypeClass;

	if (HitActorHealthComponent)
	{
		if (!HitActorHealthComponent->GetBodyPartMap().IsEmpty())
		{
			FName HitBone = InHitResult.BoneName;
			FName BodyPartToDamage = "";
			float BodyPartDamageModifier = 1.0f;

			if (InHitResult.BoneName != "")
			{
				BodyPartToDamage = HitActorHealthComponent->GetBodyPartFromBone(InHitResult.BoneName);

				if (BodyPartToDamage != "")
				{
					BodyPartDamageModifier = HitActorHealthComponent->GetBodyPartMap()[BodyPartToDamage].DamageModifier;
				}
				else if (HitActorHealthComponent->GetBodyPartMap().Num())
				{
					TArray<FName> BodyPartNameArray;
					HitActorHealthComponent->GetBodyPartMap().GenerateKeyArray(BodyPartNameArray);
					BodyPartDamageModifier = HitActorHealthComponent->GetBodyPartMap()[BodyPartNameArray[0]].DamageModifier;
				}
				if (ARPGProjectPlayerCharacter* HitPlayerCharacter = Cast<ARPGProjectPlayerCharacter>(InHitResult.GetActor()))
				{
					DamageEvent.BodyPartHit = BodyPartToDamage;
				}
				InHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, nullptr, GetOwner());
			}
			else
			{
				InHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, nullptr, GetOwner());
			}
		}
		else
		{
			InHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, nullptr, GetOwner());
		}

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::FromInt(AttackDamage));
	}
	else
	{
		InHitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, nullptr, GetOwner());
	}


	// Reduce weapon durability

	if (WeaponData)
	{
		const float DurabilityCost = WeaponData->ItemCurrentDurability - 0.5f;
		InItemWeapon->SetDurability(DurabilityCost);
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::SanitizeFloat(WeaponData->ItemCurrentDurability));
	}


	HitActorArray.AddUnique(InHitResult.GetActor());

}

bool UCombatComponent::IsAttackFromBlockedAngle(AActor* AttackingActor)
{
	if (AttackingActor != GetOwner())
	{
		const FRotator WorldRotationToTarget = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), AttackingActor->GetActorLocation());
		const FRotator RelativeRotationToTarget = UKismetMathLibrary::ComposeRotators(WorldRotationToTarget, UKismetMathLibrary::NegateRotator(GetOwner()->GetActorRotation()));

		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::SanitizeFloat(fabs(RelativeRotationToTarget.Yaw)));

		if (fabs(RelativeRotationToTarget.Yaw) <= CurrentWeaponBlockAngle)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("True"));
			return true;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, TEXT("False"));
	return false;
}

void UCombatComponent::AttackBlocked(FWeaponAttackInfo& BlockedAttackInfo, AItemWeapon* InItemWeapon, EAttackType BlockedAttackType, AProjectileActor* InProjectileActor)
{
	if (!EquipmentComponentRef) { return; }
	if (!CharacterAnimInstance) { return; }

	switch (BlockingWeapon)
	{
	case EWeaponToUse::Mainhand:
		if (MainhandWeaponRef)
		{
			if (InItemWeapon) { CharacterAnimInstance->SetReactToBlock(true); }
			
			MainhandWeaponRef->AttackBlocked(BlockedAttackInfo, InItemWeapon, BlockedAttackType, InProjectileActor);
		}
		break;

	case EWeaponToUse::Offhand:
		if (OffhandWeaponRef)
		{
			if (InItemWeapon) { CharacterAnimInstance->SetReactToBlock(true); }

			OffhandWeaponRef->AttackBlocked(BlockedAttackInfo, InItemWeapon, BlockedAttackType, InProjectileActor);
		}
		break;

	case EWeaponToUse::Both:
		if (MainhandWeaponRef)
		{
			if (InItemWeapon) { CharacterAnimInstance->SetReactToBlock(true); }

			MainhandWeaponRef->AttackBlocked(BlockedAttackInfo, InItemWeapon, BlockedAttackType, InProjectileActor);
		}
		if (OffhandWeaponRef)
		{
			if (InItemWeapon) { CharacterAnimInstance->SetReactToBlock(true); }

			OffhandWeaponRef->AttackBlocked(BlockedAttackInfo, InItemWeapon, BlockedAttackType, InProjectileActor);
		}
		break;

	default:
		break;
	}

	if (StaminaComponentRef)
	{
		if (StaminaComponentRef->GetCurrentStamina() <= 0.f)
		{
			StaminaExhausted(EStaminaExhaustionType::GuardBreak);
		}
	}
}

FWeaponAttackInfo UCombatComponent::GetCurrentWeaponAttackInfo(AItemWeapon* AttackingWeapon)
{
	if (!AttackingWeapon) { FWeaponAttackInfo(); }

	if (CharacterAnimInstance->GetCurrentActiveMontage() == CombatActionInfo.CombatActionMontage)
	{
		return CombatActionInfo.ActionAttackInfo;
	}

	switch (CurrentAttackType)
	{
	case EAttackType::LightAttack:
		if (AttackingWeapon->GetLightAttackInfo().Contains(AttackCount))
		{
			return AttackingWeapon->GetLightAttackInfoAtIndex(AttackCount);
		}
		else if (AttackingWeapon->GetLightAttackInfo().Contains(0.f))
		{
			return AttackingWeapon->GetLightAttackInfoAtIndex(0.f);
		}
		break;

	case EAttackType::HeavyAttack:
		if (AttackingWeapon->GetHeavyAttackInfo().Contains(AttackCount))
		{
			return AttackingWeapon->GetHeavyAttackInfoAtIndex(AttackCount);
		}
		else if (AttackingWeapon->GetHeavyAttackInfo().Contains(0.f))
		{
			return AttackingWeapon->GetHeavyAttackInfoAtIndex(0.f);
		}
		break;

	case EAttackType::LightFinisher:
		if (AttackingWeapon->GetLightAttackInfo().Contains(AttackCount))
		{
			return AttackingWeapon->GetLightAttackInfoAtIndex(AttackCount);
		}
		else if (AttackingWeapon->GetLightAttackInfo().Contains(0.f))
		{
			return AttackingWeapon->GetLightAttackInfoAtIndex(0.f);
		}
		break;

	case EAttackType::HeavyFinisher:
		if (AttackingWeapon->GetHeavyAttackInfo().Contains(AttackCount))
		{
			return AttackingWeapon->GetHeavyAttackInfoAtIndex(AttackCount);
		}
		else if (AttackingWeapon->GetHeavyAttackInfo().Contains(0.f))
		{
			return AttackingWeapon->GetHeavyAttackInfoAtIndex(0.f);
		}
		break;

	default:
		break;
	}
	return FWeaponAttackInfo();
}

void UCombatComponent::PlayHitFX(FHitResult InHitResult, AActor* AttackingActor, bool bOverrideHitSurface, TEnumAsByte<EPhysicalSurface> HitSurfaceOverride)
{
	UNiagaraSystem* HitVFXSystem = nullptr;
	USoundCue* HitSoundCue = nullptr;

	const TEnumAsByte<EPhysicalSurface> HitSurface = HitSurfaceOverride != EPhysicalSurface::SurfaceType_Max ? InHitResult.PhysMaterial.Get() ? InHitResult.PhysMaterial.Get()->SurfaceType : EPhysicalSurface::SurfaceType_Default : HitSurfaceOverride;

	const UEnum* PhysicalSurfaceEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhysicalSurface"));
	FString SurfaceName = PhysicalSurfaceEnum->GetNameStringByValue(HitSurface.GetValue());

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, SurfaceName);

	if (IHitFXInterface* ProjecitleHitFXInterface = Cast<IHitFXInterface>(AttackingActor))
	{
		if (ProjecitleHitFXInterface->GetHitFXData()->HitVFXMap.Contains(HitSurface))
		{
			HitVFXSystem = ProjecitleHitFXInterface->GetHitFXData()->HitVFXMap[HitSurface].NiagaraSystem;
			HitSoundCue = ProjecitleHitFXInterface->GetHitFXData()->HitVFXMap[HitSurface].SoundCue;
		}
	}
	else if (IHitFXInterface* HitFXInterface = Cast<IHitFXInterface>(InHitResult.GetActor()))
	{
		if (HitFXInterface->GetHitFXData()->HitVFXMap.Contains(HitSurface))
		{
			HitVFXSystem = HitFXInterface->GetHitFXData()->HitVFXMap[HitSurface].NiagaraSystem;
			HitSoundCue = HitFXInterface->GetHitFXData()->HitVFXMap[HitSurface].SoundCue;
		}
	}
	if (DefaultHitFXData)
	{
		if (!HitVFXSystem && DefaultHitFXData->HitVFXMap.Contains(HitSurface))
		{
			HitVFXSystem = DefaultHitFXData->HitVFXMap[HitSurface].NiagaraSystem;
		}
		if (!HitSoundCue && DefaultHitFXData->HitVFXMap.Contains(HitSurface))
		{
			HitSoundCue = DefaultHitFXData->HitVFXMap[HitSurface].SoundCue;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::EvaluateHitResult DefaultHitFXData is nullptr."));
	}


	if (HitVFXSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFXSystem, InHitResult.ImpactPoint, InHitResult.ImpactNormal.Rotation(), { 1.f, 1.f, 1.f }, true, true, ENCPoolMethod::None, true);
	}

	if (HitSoundCue)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::EvaluateHitResult HitSoundCue location = %s, %s, %s"), *FString::SanitizeFloat(InHitResult.ImpactPoint.X), *FString::SanitizeFloat(InHitResult.ImpactPoint.Y), *FString::SanitizeFloat(InHitResult.ImpactPoint.Z));
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSoundCue, InHitResult.ImpactPoint, 1.f, 1.f, 0.f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::EvaluateHitResult HitSoundCue is nullptr."));
	}
}

void UCombatComponent::OnSheathingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnSheathingMontageBlendingOut called."));
}

void UCombatComponent::OnSheathingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnSheathingMontageEnded called."));

	UnbindSheatheMontage();

	bCanAttack = true;
	if (bIsInAttackSequence) { EndAttackSequence(); }
}

void UCombatComponent::OnSheatheWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload, EWeaponToUse WeaponToSheathe)
{
	UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnSheatheMainHandNotifyReceived called."));

	if (WeaponToSheathe == EWeaponToUse::None) { return; }

	switch (WeaponToSheathe)
	{
	case EWeaponToUse::Mainhand:
		SheatheMainhand();
		break;

	case EWeaponToUse::Offhand:
		SheatheOffhand();
		break;

	case EWeaponToUse::Both:
		SheatheMainhand();
		SheatheOffhand();
		break;

	default:
		break;
	}

	CombatState = ECombatState::AtEase;

	SetCombatWeaponStance(ECombatWeaponStance::None);
	if (ARPGProjectPlayerCharacter* RPGPlayerCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef))
	{
		RPGPlayerCharacterRef->ResetWeaponStance();
	}

	if (RPGPlayerCameraManagerRef)
	{
		RPGPlayerCameraManagerRef->SetCameraView(ECameraView::Exploration);
	}

	if (bSwapWeaponLoadout && (bMainHandSheathed && bOffHandSheathed))
	{
		if (EquipmentComponentRef)
		{
			if (EquipmentComponentRef->GetCurrentWeaponLoadout() == EWeaponLoadout::WL_LoadoutOne)
			{
				EquipmentComponentRef->SetCurrentWeaponLoadout(EWeaponLoadout::WL_LoadoutTwo);
			}
			else
			{
				EquipmentComponentRef->SetCurrentWeaponLoadout(EWeaponLoadout::WL_LoadoutOne);
			}
		}

		//UnbindSheatheMontage();
		//UnbindUnsheatheMontage();
		UnsheatheWeapon();
		bSwapWeaponLoadout = false;
	}
}

void UCombatComponent::SheatheMainhand()
{
	if (EquipmentComponentRef)
	{
		if (EquipmentComponentRef->GetMainhandWeaponData())
		{
			EquipmentComponentRef->AttachWeaponToSocket(EquipmentComponentRef->GetMainhandWeaponActor(), EquipmentComponentRef->GetMainhandWeaponData()->MainhandSheatheSocket);
			bMainHandSheathed = true;
		}

		if (AItemWeaponRanged* WeaponRangedRef = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor()))
		{
			if (WeaponRangedRef->GetItemWeaponRangedData()->WeaponType == EWeaponType::WT_2HandBow)
			{
				WeaponRangedRef->UnloadWeapon();
			}
		}
	}	
}

void UCombatComponent::SheatheOffhand()
{
	if (EquipmentComponentRef)
	{
		if (EquipmentComponentRef->GetOffhandWeaponData())
		{
			EquipmentComponentRef->AttachWeaponToSocket(EquipmentComponentRef->GetOffhandWeaponActor(), EquipmentComponentRef->GetOffhandWeaponData()->OffhandSheatheSocket);
			bOffHandSheathed = true;
		}

		if (AItemWeaponRanged* WeaponRangedRef = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor()))
		{
			if (WeaponRangedRef->GetItemWeaponRangedData()->WeaponType == EWeaponType::WT_2HandBow)
			{
				WeaponRangedRef->UnloadWeapon();
			}
		}
	}
}

void UCombatComponent::OnUnsheatheMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnUnsheathingMontageBlendingOut called."));

}

void UCombatComponent::OnUnsheatheMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnUnsheatheMontageEnded called."));

	UnbindSheatheMontage();

	if (!bInterrupted)
	{
		bCanAttack = true;
		if (bIsInAttackSequence) { EndAttackSequence(); }
	}
}

void UCombatComponent::OnUnsheatheWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload, EWeaponToUse WeaponToUnsheathe)
{
	UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnUnsheatheMainHandNotifyReceived called."));

	if (WeaponToUnsheathe == EWeaponToUse::None) { return; }

	switch (WeaponToUnsheathe)
	{
	case EWeaponToUse::Mainhand:
		UnsheatheMainhand();
		break;

	case EWeaponToUse::Offhand:
		UnsheatheOffhand();
		break;

	case EWeaponToUse::Both:
		UnsheatheMainhand();
		UnsheatheOffhand();
		break;

	default:
		break;
	}

	CombatState = ECombatState::CombatReady;

	if (RPGPlayerCameraManagerRef)
	{
		RPGPlayerCameraManagerRef->SetCameraView(ECameraView::Action);
	}
}

void UCombatComponent::UnsheatheMainhand()
{
	if (EquipmentComponentRef)
	{
		if (EquipmentComponentRef->GetMainhandWeaponData())
		{
			EquipmentComponentRef->AttachWeaponToSocket(EquipmentComponentRef->GetMainhandWeaponActor(), EquipmentComponentRef->GetMainhandWeaponData()->MainhandEquippedSocket);
			bMainHandSheathed = false;
		}
	}
}

void UCombatComponent::UnsheatheOffhand()
{
	if (EquipmentComponentRef)
	{
		if (EquipmentComponentRef->GetOffhandWeaponData())
		{
			EquipmentComponentRef->AttachWeaponToSocket(EquipmentComponentRef->GetOffhandWeaponActor(), EquipmentComponentRef->GetOffhandWeaponData()->OffhandEquippedSocket);
			bOffHandSheathed = false;
		}
	}
}

void UCombatComponent::OnPlayAttackSFXNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (!EquipmentComponentRef) { return; }
	if (!EquipmentComponentRef->GetMainhandWeaponActor()) { return; }
	
	EquipmentComponentRef->GetMainhandWeaponActor()->PlayAttackSFX();
}

void UCombatComponent::OnReloadWeaponNotifyReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	
}

void UCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindAttackMontage();

	UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnAttackMontageEnded EndAttackSequence called."));
	if (bIsInAttackSequence) { EndAttackSequence(); }
}

void UCombatComponent::OnAttackNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnAttackNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnCombatActionMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{

}

void UCombatComponent::OnCombatActionEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindCombatActionMontage();

	UItemWeaponData* MainhandWeaponData = EquipmentComponentRef->GetMainhandWeaponData();
	UItemWeaponData* OffhandWeaponData = EquipmentComponentRef->GetOffhandWeaponData();

	switch (CombatWeaponStance)
	{
	case ECombatWeaponStance::Mainhand:
		if (MainhandWeaponData)
		{
			CombatActionInfo = MainhandWeaponData->MainhandStanceCombatActionInfo;
		}
		else if (OffhandWeaponData)
		{
			CombatActionInfo = OffhandWeaponData->MainhandStanceCombatActionInfo;
		}
		break;

	case ECombatWeaponStance::Offhand:
		if (OffhandWeaponData)
		{
			CombatActionInfo = OffhandWeaponData->OffhandStanceCombatActionInfo;
		}
		else if (MainhandWeaponData)
		{
			CombatActionInfo = MainhandWeaponData->OffhandStanceCombatActionInfo;
		}
		break;

	default:
		break;
	}

	if (CombatActionInfo.bRepeatIfCombatActionButtonPressed && GetCombatActionIsPressed() && !bInterrupted)
	{
		switch (GetCombatWeaponStance())
		{
		case ECombatWeaponStance::None:
			StartNeutralCombatAction();
			break;

		case ECombatWeaponStance::Mainhand:
			StartStanceCombatAction();
			break;

		case ECombatWeaponStance::Offhand:
			StartStanceCombatAction();
			break;

		default:
			break;
		}
	}
	else
	{
		CombatActionInfo = {};
	}
}

void UCombatComponent::OnReloadMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (bCombatActionNeedsToReload)
	{
		bCombatActionNeedsToReload = false;
		StopStanceCombatAction();
	}
}

void UCombatComponent::OnReloadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindReloadMontage();
	bCanAttack = true;
	UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::OnReloadMontageEnded EndAttackSequence called."));
	if (bIsInAttackSequence) { EndAttackSequence(); }
}

void UCombatComponent::OnDodgeMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	bCanAttack = true;

	if (ARPGProjectPlayerCharacter* RPGPlayerCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef))
	{
		RPGPlayerCharacterRef->SetIsInUninterruptableAction(false);
		RPGPlayerCharacterRef->SetPlayerActionState(RPGPlayerCharacterRef->GetLastPlayerActionState());
	}
}

void UCombatComponent::OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindDodgeMontage();
}

void UCombatComponent::OnStaminaExhaustedMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	bCanAttack = true;

	if (ARPGProjectPlayerCharacter* RPGPlayerCharacterRef = Cast<ARPGProjectPlayerCharacter>(CharacterRef))
	{
		RPGPlayerCharacterRef->SetIsInUninterruptableAction(false);
	}
}

void UCombatComponent::OnStaminaExhaustedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindStaminaExhaustedMontage();


}

void UCombatComponent::UnbindSheatheMontage()
{
	if (SheatheBlendingOutDelegate.IsBound()) { SheatheBlendingOutDelegate.Unbind(); }
	if (SheatheMontageEndedDelegate.IsBound()) { SheatheMontageEndedDelegate.Unbind(); }

	/*if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnSheatheWeapon.RemoveDynamic(this, &UCombatComponent::OnSheatheWeaponNotifyReceived);
	}*/
}

void UCombatComponent::UnbindUnsheatheMontage()
{
	if (UnSheatheBlendingOutDelegate.IsBound()) { UnSheatheBlendingOutDelegate.Unbind(); }
	if (UnSheatheMontageEndedDelegate.IsBound()) { UnSheatheMontageEndedDelegate.Unbind(); }

	/*if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnUnsheatheWeapon.RemoveDynamic(this, &UCombatComponent::OnUnsheatheWeaponNotifyReceived);
	}*/
}

void UCombatComponent::UnbindAttackMontage()
{
	if (WeaponAttackBlendingOutDelegate.IsBound()) { WeaponAttackBlendingOutDelegate.Unbind(); }
	if (WeaponAttackMontageEndedDelegate.IsBound()) { WeaponAttackMontageEndedDelegate.Unbind(); }

	/*if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
		CharacterAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
		CharacterAnimInstance->OnPlayAttackSFX.RemoveDynamic(this, &UCombatComponent::OnPlayAttackSFXNotifyReceived);
	}*/
}

void UCombatComponent::UnbindCombatActionMontage()
{
	if (CombatActionBlendingOutDelegate.IsBound()) { CombatActionBlendingOutDelegate.Unbind(); }
	if (CombatActionMontageEndedDelegate.IsBound()) { CombatActionMontageEndedDelegate.Unbind(); }
}

void UCombatComponent::UnbindReloadMontage()
{
	if (ReloadMontageBlendingOutDelegate.IsBound()) { ReloadMontageBlendingOutDelegate.Unbind(); }
	if (ReloadMontageEndedDelegate.IsBound()) { ReloadMontageEndedDelegate.Unbind(); }

	/*if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnReloadWeapon.RemoveDynamic(this, &UCombatComponent::OnPlayAttackSFXNotifyReceived);
	}*/
}

void UCombatComponent::UnbindDodgeMontage()
{
	if (DodgeMontageBlendingOutDelegate.IsBound()) { DodgeMontageBlendingOutDelegate.Unbind(); }
	if (DodgeMontageEndedDelegate.IsBound()) { DodgeMontageEndedDelegate.Unbind(); }
}

void UCombatComponent::UnbindStaminaExhaustedMontage()
{
	if (StaminaExhaustedMontageBlendingOutDelegate.IsBound()) { StaminaExhaustedMontageBlendingOutDelegate.Unbind(); }
	if (StaminaExhaustedMontageEndedDelegate.IsBound()) { StaminaExhaustedMontageEndedDelegate.Unbind(); }
}

bool UCombatComponent::DoesAttackNeedAmmunition(EWeaponToUse AttackingWeapon, EAttackType InAttackType)
{
	AItemWeaponRanged* MainhandWeaponRanged = nullptr;
	UItemWeaponRangedData* MainhandWeaponRangedData = nullptr;
	FWeaponAttackInfo MainhandAttackInfo;

	AItemWeaponRanged* OffhandWeaponRanged = nullptr;
	UItemWeaponRangedData* OffhandWeaponRangedData = nullptr;
	FWeaponAttackInfo OffhandAttackInfo;

	switch (AttackingWeapon)
	{
	case EWeaponToUse::Mainhand:
		MainhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor());
		MainhandWeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetMainhandWeaponData());
		break;

	case EWeaponToUse::Offhand:
		OffhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor());
		OffhandWeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetOffhandWeaponData());
		break;

	case EWeaponToUse::Both:
		MainhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor());
		MainhandWeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetMainhandWeaponData());
		OffhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor());
		OffhandWeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetOffhandWeaponData());
		break;

	default:
		break;
	}

	if (MainhandWeaponRanged && MainhandWeaponRangedData)
	{
		switch (InAttackType)
		{
		case EAttackType::LightFinisher:
		case EAttackType::LightAttack:
			if (MainhandWeaponRanged->GetLightAttackInfo().Contains(AttackCount))
			{
				if (MainhandWeaponRanged->GetLightAttackInfoAtIndex(AttackCount).bUsesAmmunition && MainhandWeaponRangedData->AmmoInMagazine <= 0.f)
				{
					return true;
				}
			}
			break;

		case EAttackType::HeavyFinisher:
		case EAttackType::HeavyAttack:
			if (MainhandWeaponRanged->GetHeavyAttackInfo().Contains(AttackCount))
			{
				if (MainhandWeaponRanged->GetHeavyAttackInfoAtIndex(AttackCount).bUsesAmmunition && MainhandWeaponRangedData->AmmoInMagazine <= 0.f)
				{
					return true;
				}
			}
			break;
		
		default:
			break;
		}
	}

	if (OffhandWeaponRanged && OffhandWeaponRangedData)
	{
		switch (InAttackType)
		{
		case EAttackType::LightFinisher:
		case EAttackType::LightAttack:
			if (OffhandWeaponRanged->GetLightAttackInfo().Contains(AttackCount))
			{
				if (OffhandWeaponRanged->GetLightAttackInfoAtIndex(AttackCount).bUsesAmmunition && OffhandWeaponRangedData->AmmoInMagazine <= 0.f)
				{
					return true;
				}
			}
			break;

		case EAttackType::HeavyFinisher:
		case EAttackType::HeavyAttack:
			if (OffhandWeaponRanged->GetHeavyAttackInfo().Contains(AttackCount))
			{
				if (OffhandWeaponRanged->GetHeavyAttackInfoAtIndex(AttackCount).bUsesAmmunition && OffhandWeaponRangedData->AmmoInMagazine <= 0.f)
				{
					return true;
				}
			}
			break;

		default:
			break;
		}
	}

	return false;
}

void UCombatComponent::UpdateCurveFloats()
{
	if (!CharacterAnimInstance) { return; }

	CharacterAnimInstance->GetCurveValue("RotationSpeedReductionScale", RotationSpeedReductionScaleCurve);

	CharacterAnimInstance->GetCurveValue("QueueAttackWindow", QueueAttackWindowCurve);

	CharacterAnimInstance->GetCurveValue("EnableHit", EnableHitCurve);

	CharacterAnimInstance->GetCurveValue("EnableWeaponVFX", EnableWeaponVFXCurve);

	CharacterAnimInstance->GetCurveValue("EnableActionRecovery", EnableActionRecoveryCurve);

	CharacterAnimInstance->GetCurveValue("EnableDodge", EnableDodgeCurve);

	CharacterAnimInstance->GetCurveValue("EnableGuard", EnableGuardCurve);
}

void UCombatComponent::UpdateCombatBooleans()
{
	if (QueueAttackWindowCurve >= 1.f)
	{
		bIsInAttackWindUp = false;
		bIsInAttackWindow = true;
	}
	else if (QueueAttackWindowCurve > 0.09f)
	{
		if (bIsInAttackWindow && (QueueAttackWindowCurve < 1.f && QueueAttackWindowCurve >= 0.1f) && EnableHitCurve < 1.f)
		{
			bIsInAttackWindow = false;
			bIsInAttackSequence = true;
			SetupNextAttack();
		}
	}

	if (EnableWeaponVFXCurve >= 1.f)
	{
		if (MainhandWeaponRef)
		{
			//GEngine->AddOnScreenDebugMessage(1, GetWorld()->GetDeltaSeconds(), FColor::Green, TEXT("Enable VFX"));
			MainhandWeaponRef->EnableWeaponVFX();
		}
	}
	else
	{
		if (MainhandWeaponRef)
		{
			//GEngine->AddOnScreenDebugMessage(1, GetWorld()->GetDeltaSeconds(), FColor::Red, TEXT("Disable VFX"));
			MainhandWeaponRef->DisableWeaponVFX();
		}
	}

	if (EnableActionRecoveryCurve >= 1.f)
	{
		bIsInActionRecovery = true;
	}
	else
	{
		bIsInActionRecovery = false;
	}

	if (EnableDodgeCurve >= 1.f)
	{
		if (!bIsDodging && RPGProjectCharacterRef)
		{
			RPGProjectCharacterRef->EnableDodgeCollision(true);
		}

		bIsDodging = true;
	}
	else
	{
		if (bIsDodging && RPGProjectCharacterRef)
		{
			RPGProjectCharacterRef->EnableDodgeCollision(false);
		}

		bIsDodging = false;
	}

	if (EnableGuardCurve >= 1.f)
	{
		SetIsGuarding(true);
	}
	else
	{
		SetIsGuarding(false);
	}
	
}

void UCombatComponent::UpdateGuardState()
{
	if (!EquipmentComponentRef) { return; }

	bool bStaminaExhausted = false;
	if (StaminaComponentRef)
	{
		bStaminaExhausted = StaminaComponentRef->IsStaminaExhausted();
	}

	if (bIsGuarding && !bStaminaExhausted)
	{

		if (MainhandWeaponRef && OffhandWeaponRef) // Weapon in both hands
		{
			bool bBlockWithMainhand = false;
			bool bBlockWithOffhand = false;

			switch (OffhandWeaponRef->GetItemWeaponData()->WeaponType)
			{
			case EWeaponType::WT_None:
				break;

			case EWeaponType::WT_Unarmed:
				break;

			case EWeaponType::WT_Item:
				break;

			case EWeaponType::WT_1HandDagger:
				break;

			case EWeaponType::WT_1HandMace:
				break;

			case EWeaponType::WT_1HandPistol:
				break;

			case EWeaponType::WT_1HandSpear:
				break;

			case EWeaponType::WT_1HandSword:
				break;

			case EWeaponType::WT_1HandShield:
				bBlockWithOffhand = true;
				break;

			default:
				break;
			}

			if (bBlockWithOffhand)
			{
				OffhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CurrentWeaponBlockAngle = OffhandWeaponRef->GetItemWeaponData()->WeaponBlockAngle * 0.5f;
				BlockingWeapon = EWeaponToUse::Offhand;
			}

			if (bBlockWithMainhand)
			{
				MainhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				CurrentWeaponBlockAngle = MainhandWeaponRef->GetItemWeaponData()->WeaponBlockAngle * 0.5f;
				BlockingWeapon = EWeaponToUse::Mainhand;
			}

			if (bBlockWithMainhand && bBlockWithOffhand)
			{
				BlockingWeapon = EWeaponToUse::Both;
			}
		}
		else if (MainhandWeaponRef && !OffhandWeaponRef) // Weapon in Mainhand
		{
			MainhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CurrentWeaponBlockAngle = MainhandWeaponRef->GetItemWeaponData()->WeaponBlockAngle * 0.5f;
			BlockingWeapon = EWeaponToUse::Mainhand;
		}
		else if (!MainhandWeaponRef && OffhandWeaponRef) // Weapon in Offhand
		{
			OffhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CurrentWeaponBlockAngle = OffhandWeaponRef->GetItemWeaponData()->WeaponBlockAngle * 0.5f;
			BlockingWeapon = EWeaponToUse::Offhand;
		}
	}
	else
	{
		if (MainhandWeaponRef)
		{
			MainhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::NoCollision);
			BlockingWeapon = EWeaponToUse::None;
		}

		if (OffhandWeaponRef)
		{
			OffhandWeaponRef->SetAttackBlockerCollisionEnabled(ECollisionEnabled::NoCollision);
			BlockingWeapon = EWeaponToUse::None;
		}
	}
}

void UCombatComponent::SetupNextAttack()
{
	if (!EquipmentComponentRef) { return; }
	if (!CharacterAnimInstance) { return; }

	UItemWeaponData* WeaponData = EquipmentComponentRef->GetMainhandWeaponData();//GetEquippedWeaponData();
	if (bIsFinisherQueued)
	{
		switch (FinisherType)
		{
		case EAttackType::LightFinisher:
			if (AttackCount <= WeaponData->LightAttackComboLimit)
			{
				NextSectionInMontage = (FName)("Light_Attack_Finisher");
				CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponAttackMontage);
				CurrentSectionInMontage = NextSectionInMontage;
				AttackCount = UINT8_MAX;
				bIsInAttackWindUp = true;
			}
			break;
		
		case EAttackType::HeavyFinisher:
			if (AttackCount <= WeaponData->HeavyAttackComboLimit)
			{
				NextSectionInMontage = (FName)("Heavy_Attack_Finisher");
				CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponAttackMontage);
				CurrentSectionInMontage = NextSectionInMontage;
				AttackCount = UINT8_MAX;
				bIsInAttackWindUp = true;
			}
			break;
		
		default:
			break;
		}
		bIsFinisherQueued = false;
		FinisherType = EAttackType::None;
	}
	else if (bIsAttackQueued)
	{
		switch (NextAttackType)
		{
		case EAttackType::LightAttack:
			if (AttackCount < WeaponData->LightAttackComboLimit)
			{
				NextSectionInMontage = (FName)("Light_Attack_" + FString::FromInt(AttackCount + 1));
				CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponAttackMontage);
				CurrentSectionInMontage = NextSectionInMontage;
				AttackCount++;
				bIsInAttackWindUp = true;
			}
			break;

		case EAttackType::HeavyAttack:
			if (AttackCount < WeaponData->HeavyAttackComboLimit)
			{
				NextSectionInMontage = (FName)("Heavy_Attack_" + FString::FromInt(AttackCount + 1));
				CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponAttackMontage);
				CurrentSectionInMontage = NextSectionInMontage;
				AttackCount++;
				bIsInAttackWindUp = true;
			}
			break;

		default:
			break;
		}
		bIsAttackQueued = false;
		CurrentAttackType = NextAttackType;
		NextAttackType = EAttackType::None;
	}
}

void UCombatComponent::EndAttackSequence()
{
	UnbindAttackMontage();
	AttackCount = 0;
	bIsInAttackSequence = false;
	bIsInAttackWindUp = false;
	bIsAttackQueued = false;
	bIsFinisherQueued = false;
	CurrentAttackType = EAttackType::None;
	CurrentSectionInMontage = "";
}

void UCombatComponent::AttackTracing()
{
	if (!EquipmentComponentRef) { return; }

	const EEquipmentSlot WeaponSlot = EquipmentComponentRef->GetEquipmentSlotForCurrentWeaponLoadout();
	if (WeaponSlot == EEquipmentSlot::ES_None) { return; }

	AItemWeapon* EquippedWeapon = Cast<AItemWeapon>(EquipmentComponentRef->GetWornEquipmentActorInSlot(WeaponSlot)->GetChildActor());
	if (!EquippedWeapon) { return; }

	if (EnableHitCurve == 1.f)
	{
		FWeaponAttackInfo WeaponAttackInfo;

		switch (CurrentAttackType)
		{
		case EAttackType::LightFinisher:
		case EAttackType::LightAttack:
			if (EquippedWeapon->GetLightAttackInfo().Contains(AttackCount))
			{
				WeaponAttackInfo = EquippedWeapon->GetLightAttackInfoAtIndex(AttackCount);
			}
			else if (EquippedWeapon->GetLightAttackInfo().Contains(0.f))
			{
				WeaponAttackInfo = EquippedWeapon->GetLightAttackInfoAtIndex(0);
			}
			break;

		case EAttackType::HeavyFinisher:
		case EAttackType::HeavyAttack:
			if (EquippedWeapon->GetHeavyAttackInfo().Contains(AttackCount))
			{
				WeaponAttackInfo = EquippedWeapon->GetHeavyAttackInfoAtIndex(AttackCount);
			}
			else if ((EquippedWeapon->GetHeavyAttackInfo().Contains(0)))
			{
				WeaponAttackInfo = EquippedWeapon->GetHeavyAttackInfoAtIndex(0);
			}
			break;

		default:
			break;
		}

		if (WeaponAttackInfo.AttackTraceType == ETraceType::Unique_Trace)
		{
			EquippedWeapon->SetAttackComboCount(AttackCount);
			EquippedWeapon->SetCurrentAttackType(CurrentAttackType);
			EquippedWeapon->UniqueWeaponTrace();
		}
		else
		{
			EquippedWeapon->SetAttackComboCount(AttackCount);
			EquippedWeapon->SetCurrentAttackType(CurrentAttackType);
			EquippedWeapon->WeaponTrace();
		}

		TArray<FHitResult> WeaponTraceHitArray = EquippedWeapon->GetWeaponTraceHitResults();

		for (int i = 0; i < WeaponTraceHitArray.Num(); i++)
		{
			if (WeaponTraceHitArray[i].bBlockingHit) //EquippedWeapon->GetWeaponTraceHasHit())
			{
				if (WeaponTraceHitArray[i].GetActor())
				{
					if (!HitActorArray.Contains(WeaponTraceHitArray[i].GetActor()))
					{
						EvaluateHitResult(WeaponTraceHitArray[i], EquippedWeapon);
					}
				}
			}
		}
	}
	else
	{
		HitActorArray.Empty();
		EquippedWeapon->ClearWeaponTraceHitResults();
		EquippedWeapon->SetIsFirstTrace(true);
	}

}

UItemWeaponData* UCombatComponent::GetEquippedWeaponData()
{
	if (!EquipmentComponentRef)
	{ 
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::GetEquippedWeaponData EquipmentComponentRef is nullptr."));
		return nullptr; 
	}

	const EEquipmentSlot WeaponSlot = EquipmentComponentRef->GetEquipmentSlotForCurrentWeaponLoadout();
	if (WeaponSlot == EEquipmentSlot::ES_None) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("UCombatComponent::GetEquippedWeaponData WeaponSlot is None."));
		return nullptr; 
	}
	
	return Cast<UItemWeaponData>(EquipmentComponentRef->GetWornEquipmentDataInSlot(WeaponSlot));
}

void UCombatComponent::StaminaExhaustionUpdate()
{
	if (!CharacterAnimInstance) { return; }
	if (!StaminaComponentRef) { return; }
	
	if (!StaminaComponentRef->IsStaminaExhausted() && CharacterAnimInstance->GetCurrentActiveMontage() == StaminaExhaustedMontage)
	{
		if (CharacterAnimInstance->Montage_GetCurrentSection(StaminaExhaustedMontage) == "Default_Loop" && StaminaExhaustedMontage->IsValidSectionName("Default_End"))
		{
			CharacterAnimInstance->Montage_SetNextSection("Default_Loop", "Default_End", StaminaExhaustedMontage);
		}
	}
}
