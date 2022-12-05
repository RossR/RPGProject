// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Enums/CombatEnums.h"
#include "AnimNotify_SheatheWeapon.generated.h"

/**
 *
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Sheathe Weapon"))
class RPGPROJECT_API UAnimNotify_SheatheWeapon : public UAnimNotify
{
	GENERATED_UCLASS_BODY()

public:

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

#if WITH_EDITOR
	virtual bool CanBePlaced(UAnimSequenceBase* Animation) const override;
#endif

protected:

	// Name of notify that is passed to the PlayMontage K2Node.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	EWeaponToUse WeaponToSheathe;

};
