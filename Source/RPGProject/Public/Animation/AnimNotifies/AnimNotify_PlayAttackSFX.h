// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Enums/CombatEnums.h"
#include "AnimNotify_PlayAttackSFX.generated.h"

/**
 * 
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play Attack SFX"))
//UCLASS(meta = (DisplayName = "Play Attack SFX"))
class RPGPROJECT_API UAnimNotify_PlayAttackSFX : public UAnimNotify
{
	GENERATED_UCLASS_BODY()
	//GENERATED_BODY()

public:

	//UAnimNotify_PlayAttackSFX();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

protected:

	// Name of notify that is passed to the PlayMontage K2Node.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	EWeaponToUse WeaponToPlaySFX = EWeaponToUse::Mainhand;

};