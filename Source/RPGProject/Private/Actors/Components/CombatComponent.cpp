// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/ItemTypes/ItemWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	AttackCount;
	
	AttackType = EAttackType::AT_None;
	DodgeType = EDodgeType::DT_None;
	CombatState = ECombatState::CS_AtEase;

	QueueAttackWindowCurve = 0.0f;
	EnableHitCurve = 0.0f;

	bIsInCombat = false;
	bIsAttackQueued = false;
	bIsInAttackSequence = false;
	bIsInAttackWindow = false;
	bIsInAttackWindUp = false;

}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<ACharacter>(GetOwner());
	if (CharacterRef) { MeshComponentRef = CharacterRef->GetMesh(); }
	CharacterAnimInstance = MeshComponentRef->GetAnimInstance();
	EquipmentComponentRef = Cast<UEquipmentComponent>(CharacterRef->GetComponentByClass(UEquipmentComponent::StaticClass()));
	HealthComponentRef = Cast<UHealthComponent>(CharacterRef->GetComponentByClass(UEquipmentComponent::StaticClass()));
	StaminaComponentRef = Cast <UStaminaComponent>(CharacterRef->GetComponentByClass(UStaminaComponent::StaticClass()));
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCurveFloats();
	UpdateCombatBooleans();
	AttackTracing();
}

void UCombatComponent::CharacterAttack(EAttackType CharacterAttackType)
{
	if (CombatState != ECombatState::CS_CombatReady) { return; }

	if (EquipmentComponentRef) 
	{
		if (GetEquippedWeaponData())
		{
			WeaponMontage = GetEquippedWeaponData()->AttackMontage;
		}
	}

	if (!bIsInAttackSequence)
	{
		StartAttackSequence(CharacterAttackType);
	}
	else if (bIsInAttackWindow)
	{
		bIsAttackQueued = true;
		AttackType = CharacterAttackType;
	}
}

void UCombatComponent::StartAttackSequence(EAttackType SequenceAttackType)
{
	if (CombatState != ECombatState::CS_CombatReady) { return; }

	if (!CharacterRef) { return; }
	if (!StaminaComponentRef) { return; }

	bIsInAttackSequence = true;
	bIsInAttackWindUp = true;
	AttackCount = 1;

	switch (SequenceAttackType)
	{
	case EAttackType::AT_LightAttack:
		CurrentSectionInMontage = (FName)("LightAttack" + FString::FromInt(AttackCount));
		break;
	case EAttackType::AT_HeavyAttack:
		CurrentSectionInMontage = (FName)("HeavyAttack" + FString::FromInt(AttackCount));
		break;
	default:
		break;
	}

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(WeaponMontage, PlayRate) > 0.f;

	if (bPlayedSuccessfully)
	{
		if (CharacterAnimInstance)
		{
			if (!BlendingOutDelegate.IsBound())
			{
				BlendingOutDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, WeaponMontage);

			if (!MontageEndedDelegate.IsBound())
			{
				MontageEndedDelegate.BindUObject(this, &UCombatComponent::OnAttackMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, WeaponMontage);

			CharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
			CharacterAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
		}
	}
}

void UCombatComponent::ToggleCombatState()
{
	switch (CombatState)
	{
	case ECombatState::CS_AtEase:
		UnSheathWeapon();
		break;
	case ECombatState::CS_CombatReady:
		SheathWeapon();
		break;
	default:
		break;
	}
}

bool UCombatComponent::UnSheathWeapon()
{
	if (!CharacterRef) { return false; }
	if (!MeshComponentRef) { return false; }

	EquippedWeaponUnSheathSection = (GetEquippedWeaponData() != nullptr) ? GetEquippedWeaponData()->WeaponUnSheathMontageSection : "";

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(UnSheathMontage, PlayRate, EquippedWeaponUnSheathSection) > 0.f;
	
	if (bPlayedSuccessfully)
	{
		if (CharacterAnimInstance)
		{
			if (!BlendingOutDelegate.IsBound())
			{
				BlendingOutDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, UnSheathMontage);

			if (!MontageEndedDelegate.IsBound())
			{
				MontageEndedDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, UnSheathMontage);

			CharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCombatComponent::OnSheathingNotifyBeginReceived);
			CharacterAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UCombatComponent::OnSheathingNotifyEndReceived);
		}
	}

	return bPlayedSuccessfully;
}

bool UCombatComponent::SheathWeapon()
{
	if (!CharacterRef) { return false; }
	if (!MeshComponentRef) { return false; }
	if (!EquipmentComponentRef) { return false; }

	EquippedWeaponSheathSection = (GetEquippedWeaponData() != nullptr) ? GetEquippedWeaponData()->WeaponSheathMontageSection : "";

	const float PlayRate = 1.0f;
	bool bPlayedSuccessfully = CharacterRef->PlayAnimMontage(SheathMontage, PlayRate, EquippedWeaponSheathSection) > 0.f;

	if (bPlayedSuccessfully)
	{
		if (CharacterAnimInstance)
		{
			if (!BlendingOutDelegate.IsBound())
			{
				BlendingOutDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageBlendingOut);
			}

			CharacterAnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, SheathMontage);

			if (!MontageEndedDelegate.IsBound())
			{
				MontageEndedDelegate.BindUObject(this, &UCombatComponent::OnSheathingMontageEnded);
			}

			CharacterAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, SheathMontage);

			CharacterAnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UCombatComponent::OnSheathingNotifyBeginReceived);
			CharacterAnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UCombatComponent::OnSheathingNotifyEndReceived);
		}
	}

	return bPlayedSuccessfully;
}

void UCombatComponent::OnSheathingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UnbindSheathingMontage();

	if (Montage == UnSheathMontage)
	{
		CombatState = ECombatState::CS_CombatReady;
	}
	else if (Montage == SheathMontage)
	{
		CombatState = ECombatState::CS_AtEase;
	}
}

void UCombatComponent::OnSheathingMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{

}

void UCombatComponent::OnSheathingNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnSheathingNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	
}

void UCombatComponent::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	EndAttackSequence();
}

void UCombatComponent::OnAttackNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::OnAttackNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{

}

void UCombatComponent::UnbindSheathingMontage()
{
	if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UCombatComponent::OnSheathingNotifyBeginReceived);
		CharacterAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UCombatComponent::OnSheathingNotifyEndReceived);
	}
}

void UCombatComponent::UnbindAttackMontage()
{
	if (CharacterAnimInstance)
	{
		CharacterAnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyBeginReceived);
		CharacterAnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UCombatComponent::OnAttackNotifyEndReceived);
	}
}

void UCombatComponent::UpdateCurveFloats()
{
	if (!MeshComponentRef) { return; }

	MeshComponentRef->GetAnimInstance()->GetCurveValue("QueueAttackWindow", QueueAttackWindowCurve);

	MeshComponentRef->GetAnimInstance()->GetCurveValue("EnableHit", EnableHitCurve);
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
		if (bIsInAttackWindow && (QueueAttackWindowCurve < 1.f && QueueAttackWindowCurve >= 0.1f))
		{
			bIsInAttackWindow = false;
			bIsInAttackSequence = true;
			SetupNextAttack();
		}
	}
	else if (!bIsInAttackWindUp)
	{
		EndAttackSequence();
	}
}

void UCombatComponent::SetupNextAttack()
{
	if (!StaminaComponentRef) { return; }
	if (!EquipmentComponentRef) { return; }
	if (!CharacterAnimInstance) { return; }

	UItemWeaponData* WeaponData = GetEquippedWeaponData();

	switch (AttackType)
	{
	case EAttackType::AT_LightAttack:
		if (bIsAttackQueued && AttackCount < WeaponData->LightAttackComboLimit)
		{
			NextSectionInMontage = (FName)("LightAttack" + FString::FromInt(AttackCount + 1));
			StaminaComponentRef->TakeStaminaDamage(20.f);
			CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponMontage);
			CurrentSectionInMontage = NextSectionInMontage;
			AttackCount+= 1.f;
			bIsInAttackWindUp = true;
		}
		else
		{
			NextSectionInMontage = (FName)("LightAttack" + FString::FromInt(AttackCount) + "End");
			CharacterAnimInstance->Montage_SetNextSection(CurrentSectionInMontage, NextSectionInMontage, WeaponMontage);
			AttackCount = 0.f;
		}
		break;

	case EAttackType::AT_HeavyAttack:
		if (bIsAttackQueued && AttackCount < WeaponData->HeavyAttackComboLimit)
		{
			NextSectionInMontage = (FName)("HeavyAttack" + FString::FromInt(AttackCount + 1));
			StaminaComponentRef->TakeStaminaDamage(40.f);
			CharacterAnimInstance->Montage_JumpToSection(NextSectionInMontage, WeaponMontage);
			CurrentSectionInMontage = NextSectionInMontage;
			AttackCount++;
			bIsInAttackWindUp = true;
		}
		else
		{
			NextSectionInMontage = (FName)("HeavyAttack" + FString::FromInt(AttackCount) + "End");
			CharacterAnimInstance->Montage_SetNextSection(CurrentSectionInMontage, NextSectionInMontage, WeaponMontage);
			AttackCount = 0.f;
		}
		break;

	default:
		break;
	}

	bIsAttackQueued = false;
}

void UCombatComponent::EndAttackSequence()
{
	UnbindAttackMontage();
	AttackCount = 0;
	bIsInAttackSequence = false;
	bIsInAttackWindUp = false;
	bIsAttackQueued = false;
	AttackType = EAttackType::AT_None;
}

void UCombatComponent::AttackTracing()
{
	if (!EquipmentComponentRef) { return; }

	const EEquipmentSlot WeaponSlot = EquipmentComponentRef->GetCurrentlyEquippedWeaponSet();
	if (WeaponSlot == EEquipmentSlot::EES_None) { return; }

	AItemWeapon* EquippedWeapon = Cast<AItemWeapon>(EquipmentComponentRef->GetWornEquipmentActorInSlot(WeaponSlot)->GetChildActor());
	if (!EquippedWeapon) { return; }

	if (EnableHitCurve == 1.f)
	{
		if (EquippedWeapon->GetTraceType() == ETraceType::Unique_Trace)
		{
			EquippedWeapon->UniqueWeaponTrace();
		}
		else
		{
			EquippedWeapon->WeaponTrace(EquippedWeapon->GetTraceType());
		}
		

		TArray<FHitResult> WeaponTraceHitArray = EquippedWeapon->GetWeaponTraceHitResults();

		for (int i = 0; i < WeaponTraceHitArray.Num(); i++)
		{
			if (EquippedWeapon->GetWeaponTraceHasHit())
			{
				if (WeaponTraceHitArray[i].GetActor())
				{
					if (!HitActorArray.Contains(WeaponTraceHitArray[i].GetActor()))
					{
						// ToDo - SpawnEmitterAtLocation

						UHealthComponent* HitActorHealthComponent = Cast<UHealthComponent>(WeaponTraceHitArray[i].GetActor()->GetComponentByClass(UHealthComponent::StaticClass()));

						if (HitActorHealthComponent)
						{
							UItemWeaponData* WeaponData = Cast<UItemWeaponData>(EquippedWeapon->GetItemData());
							if (WeaponData) { HitActorHealthComponent->TakeDamage(WeaponData->WeaponDamage); }
						}

						HitActorArray.AddUnique(WeaponTraceHitArray[i].GetActor());

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
	if (!EquipmentComponentRef) { return nullptr; }
	const EEquipmentSlot WeaponSlot = EquipmentComponentRef->GetCurrentlyEquippedWeaponSet();
	if (WeaponSlot == EEquipmentSlot::EES_None) { return nullptr; }
	return Cast<UItemWeaponData>(EquipmentComponentRef->GetWornEquipmentDataInSlot(WeaponSlot));
}