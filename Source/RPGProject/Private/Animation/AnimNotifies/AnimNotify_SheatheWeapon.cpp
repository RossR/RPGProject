// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_SheatheWeapon.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Actors/Components/CombatComponent.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_SheatheWeapon
//////////////////////////////////////////////////////////////////////////

UAnimNotify_SheatheWeapon::UAnimNotify_SheatheWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_SheatheWeapon::GetNotifyName_Implementation() const
{
	if (WeaponToSheathe != EWeaponToUse::None)
	{
		return ("Sheathe Weapon in " + UEnum::GetDisplayValueAsText(WeaponToSheathe).ToString());
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_SheatheWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

}

void UAnimNotify_SheatheWeapon::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			if (RPGProjectAnimInstance->OnSheatheWeapon.IsBound())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UAnimNotify_SheatheWeapon::BranchingPointNotify Broadcast."));
				RPGProjectAnimInstance->OnSheatheWeapon.Broadcast(NotifyName, BranchingPointPayload, WeaponToSheathe);
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UAnimNotify_SheatheWeapon::BranchingPointNotify No Function Bound."));
			}
		}
	}
}

#if WITH_EDITOR
bool UAnimNotify_SheatheWeapon::CanBePlaced(UAnimSequenceBase* Animation) const
{
	return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}
#endif // WITH_EDITOR