// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_PlayAttackSFX.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Characters/RPGProjectPlayerCharacter.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_PlayAttackSFX
//////////////////////////////////////////////////////////////////////////

UAnimNotify_PlayAttackSFX::UAnimNotify_PlayAttackSFX(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

//UAnimNotify_PlayAttackSFX::UAnimNotify_PlayAttackSFX()
//{
//	bIsNativeBranchingPoint = true;
//}

FString UAnimNotify_PlayAttackSFX::GetNotifyName_Implementation() const
{
	if (WeaponToPlaySFX != EWeaponToUse::None)
	{
		return ("Play Attack SFX on " + UEnum::GetDisplayValueAsText(WeaponToPlaySFX).ToString());
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_PlayAttackSFX::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(Owner->GetComponentByClass(UEquipmentComponent::StaticClass())))
		{
			AItemWeapon* MainhandWeaponRef = EquipmentComponentRef->GetMainhandWeaponActor();
			AItemWeapon* OffhandWeaponRef = EquipmentComponentRef->GetOffhandWeaponActor();

			switch (WeaponToPlaySFX)
			{
			case EWeaponToUse::Mainhand:
				if (MainhandWeaponRef) { MainhandWeaponRef->PlayAttackSFX(); }
				break;

			case EWeaponToUse::Offhand:
				if (OffhandWeaponRef) { OffhandWeaponRef->PlayAttackSFX(); }
				break;

			case EWeaponToUse::Both:
				if (MainhandWeaponRef) { MainhandWeaponRef->PlayAttackSFX(); }
				if (OffhandWeaponRef) { OffhandWeaponRef->PlayAttackSFX(); }
				break;

			default:
				break;
			}
		}
	}
}

void UAnimNotify_PlayAttackSFX::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			RPGProjectAnimInstance->OnPlayAttackSFX.Broadcast(NotifyName, BranchingPointPayload);
		}
	}
}