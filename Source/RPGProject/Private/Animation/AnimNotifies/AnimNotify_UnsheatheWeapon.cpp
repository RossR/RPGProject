// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_UnsheatheWeapon.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"

//////////////////////////////////////////////////////////////////////////
// UAnimNotify_UnsheatheWeapon
//////////////////////////////////////////////////////////////////////////

UAnimNotify_UnsheatheWeapon::UAnimNotify_UnsheatheWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsNativeBranchingPoint = true;
}

FString UAnimNotify_UnsheatheWeapon::GetNotifyName_Implementation() const
{
	if (WeaponToUnsheathe != EWeaponToUse::None)
	{
		return ("Unsheathe Weapon in " + UEnum::GetDisplayValueAsText(WeaponToUnsheathe).ToString());
	}
	else
	{
		return Super::GetNotifyName_Implementation();
	}
}

void UAnimNotify_UnsheatheWeapon::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

}

void UAnimNotify_UnsheatheWeapon::BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload)
{
	Super::BranchingPointNotify(BranchingPointPayload);

	if (USkeletalMeshComponent* MeshComp = BranchingPointPayload.SkelMeshComponent)
	{
		if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
		{
			if (RPGProjectAnimInstance->OnUnsheatheWeapon.IsBound())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UAnimNotify_UnsheatheWeapon::BranchingPointNotify Broadcast."));
				RPGProjectAnimInstance->OnUnsheatheWeapon.Broadcast(NotifyName, BranchingPointPayload, WeaponToUnsheathe);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("UAnimNotify_UnsheatheWeapon::BranchingPointNotify No Function Bound."));
			}
		}
	}
}

#if WITH_EDITOR
bool UAnimNotify_UnsheatheWeapon::CanBePlaced(UAnimSequenceBase* Animation) const
{
	return (Animation && Animation->IsA(UAnimMontage::StaticClass()));
}
#endif // WITH_EDITOR