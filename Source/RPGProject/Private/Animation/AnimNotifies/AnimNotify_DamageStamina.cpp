// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_DamageStamina.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "Actors/Components/StaminaComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/CombatComponent.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_DamageStamina
//////////////////////////////////////////////////////////////////////////

UAnimNotify_DamageStamina::UAnimNotify_DamageStamina(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_DamageStamina::GetNotifyName_Implementation() const
{
	if (bOverrideWeaponStaminaCost)
	{
		return ("Reduce Stamina by " + FString::SanitizeFloat(StaminaDamage));
	}
	else if (WeaponToUseStaminaCostFrom != EWeaponToUse::None)
	{
		return ("Reduce Stamina with " + UEnum::GetDisplayValueAsText(WeaponToUseStaminaCostFrom).ToString() + "'s Attack");
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_DamageStamina::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		UCombatComponent* CombatComponentRef = Cast<UCombatComponent>(Owner->GetComponentByClass(UCombatComponent::StaticClass()));
		UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(Owner->GetComponentByClass(UEquipmentComponent::StaticClass()));
		UStaminaComponent* StaminaComponentRef = Cast<UStaminaComponent>(Owner->GetComponentByClass(UStaminaComponent::StaticClass()));
		
		if (bOverrideWeaponStaminaCost && StaminaComponentRef)
		{
			StaminaComponentRef->ReduceCurrentStamina(StaminaDamage);
			return;
		}

		if (CombatComponentRef && EquipmentComponentRef && StaminaComponentRef)
		{
			StaminaDamage = 0.f;

			AItemWeapon* MainhandWeaponRef = EquipmentComponentRef->GetMainhandWeaponActor();
			AItemWeapon* OffhandWeaponRef = EquipmentComponentRef->GetOffhandWeaponActor();

			switch (WeaponToUseStaminaCostFrom)
			{
			case EWeaponToUse::Mainhand:
				if (MainhandWeaponRef) 
				{
					StaminaDamage = CombatComponentRef->GetCurrentWeaponAttackInfo(MainhandWeaponRef).StaminaCost;
				}
				break;

			case EWeaponToUse::Offhand:
				if (OffhandWeaponRef) 
				{ 
					StaminaDamage = CombatComponentRef->GetCurrentWeaponAttackInfo(OffhandWeaponRef).StaminaCost;
				}
				break;

			case EWeaponToUse::Both:
				if (MainhandWeaponRef && OffhandWeaponRef) 
				{ 
					StaminaDamage = CombatComponentRef->GetCurrentWeaponAttackInfo(MainhandWeaponRef).StaminaCost + CombatComponentRef->GetCurrentWeaponAttackInfo(OffhandWeaponRef).StaminaCost;
				}
				break;

			default:
				break;
			}

			StaminaComponentRef->ReduceCurrentStamina(StaminaDamage);
		}
	}
}

void UAnimNotify_DamageStamina::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			RPGProjectAnimInstance->OnDamageStamina.Broadcast(NotifyName, BranchingPointPayload);
		}
	}
}