// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_FireProjectile.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Actors/Components/CombatComponent.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_FireProjectile
//////////////////////////////////////////////////////////////////////////

UAnimNotify_FireProjectile::UAnimNotify_FireProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_FireProjectile::GetNotifyName_Implementation() const
{
	if (WeaponToFire != EWeaponToUse::None)
	{
		return ("Fire projectile from " + UEnum::GetDisplayValueAsText(WeaponToFire).ToString() + (bAimAtCrosshair ? " towards Crosshair" : ""));
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_FireProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(Owner->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			AItemWeaponRanged* MainhandWeaponRanged = nullptr;
			AItemWeaponRanged* OffhandWeaponRanged = nullptr;

			switch (WeaponToFire)
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
				MainhandWeaponRanged->FireProjectile(bAimAtCrosshair);
			}

			if (OffhandWeaponRanged)
			{
				OffhandWeaponRanged->FireProjectile(bAimAtCrosshair);
			}
		}
	}
}

void UAnimNotify_FireProjectile::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			RPGProjectAnimInstance->OnFireProjectile.Broadcast(NotifyName, BranchingPointPayload);
		}
	}
}
