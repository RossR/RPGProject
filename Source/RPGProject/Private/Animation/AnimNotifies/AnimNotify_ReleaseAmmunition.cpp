// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_ReleaseAmmunition.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Actors/ProjectileActor.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_ReleaseAmmunition
//////////////////////////////////////////////////////////////////////////

UAnimNotify_ReleaseAmmunition::UAnimNotify_ReleaseAmmunition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_ReleaseAmmunition::GetNotifyName_Implementation() const
{
	if (AmmunitionFromWeapon != EWeaponToUse::None)
	{
		return ("Release Ammunition for " + UEnum::GetDisplayValueAsText(AmmunitionFromWeapon).ToString());
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_ReleaseAmmunition::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("UAnimNotify_ReleaseAmmunition::Notify called."));

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(Owner->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			AItemWeaponRanged* WeaponRangedRef = nullptr;
			UItemWeaponRangedData* WeaponRangedData = nullptr;

			switch (AmmunitionFromWeapon)
			{
			case EWeaponToUse::Mainhand:
				WeaponRangedRef = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetMainhandWeaponActor());
				WeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetMainhandWeaponData());
				break;

			case EWeaponToUse::Offhand:
				WeaponRangedRef = Cast<AItemWeaponRanged>(EquipmentComponentRef->GetOffhandWeaponActor());
				WeaponRangedData = Cast<UItemWeaponRangedData>(EquipmentComponentRef->GetOffhandWeaponData());
				break;

			default:
				break;
			}

			if (WeaponRangedRef)
			{
				WeaponRangedRef->AttachAmmunitionToWeapon();
			}
		}
	}
}

void UAnimNotify_ReleaseAmmunition::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			RPGProjectAnimInstance->OnReleaseAmmunition.Broadcast(NotifyName, BranchingPointPayload);
		}
	}
}