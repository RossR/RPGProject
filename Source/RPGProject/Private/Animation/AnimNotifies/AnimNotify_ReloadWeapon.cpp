// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_ReloadWeapon.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Actors/Components/CombatComponent.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_ReloadWeapon
//////////////////////////////////////////////////////////////////////////

UAnimNotify_ReloadWeapon::UAnimNotify_ReloadWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_ReloadWeapon::GetNotifyName_Implementation() const
{
	if (ReloadType != EReloadType::None && WeaponToReload != EWeaponToUse::None)
	{
		return (UEnum::GetDisplayValueAsText(ReloadType).ToString() + " on " + UEnum::GetDisplayValueAsText(WeaponToReload).ToString());
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_ReloadWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(Owner->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			AItemWeaponRanged* MainhandWeaponRanged = nullptr;
			AItemWeaponRanged* OffhandWeaponRanged = nullptr;

			switch (WeaponToReload)
			{
			case EWeaponToUse::Mainhand:
				MainhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor());
				break;
			
			case EWeaponToUse::Offhand:
				OffhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor());
				break;
			
			case EWeaponToUse::Both:
				MainhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor());
				OffhandWeaponRanged = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor());
				break;
			
			default:
				break;
			}

			if (MainhandWeaponRanged)
			{
				MainhandWeaponRanged->ReloadWeapon(ReloadType);
			}

			if (OffhandWeaponRanged)
			{
				OffhandWeaponRanged->ReloadWeapon(ReloadType);
			}
		}
	}
}

void UAnimNotify_ReloadWeapon::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			RPGProjectAnimInstance->OnReloadWeapon.Broadcast(NotifyName, BranchingPointPayload);
		}
	}
}