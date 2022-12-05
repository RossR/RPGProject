// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Enums/CombatEnums.h"
#include "AnimNotify_DamageStamina.generated.h"

class AItemWeapon;
/**
 * 
 */
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Damage Stamina"))
class RPGPROJECT_API UAnimNotify_DamageStamina : public UAnimNotify
{
	GENERATED_UCLASS_BODY()

public:

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual void BranchingPointNotify(FBranchingPointNotifyPayload& BranchingPointPayload) override;

protected:

	// Name of notify that is passed to the PlayMontage K2Node.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	FName NotifyName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	EWeaponToUse WeaponToUseStaminaCostFrom = EWeaponToUse::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify")
	bool bOverrideWeaponStaminaCost = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Notify", meta = (EditCondition = "bOverrideWeaponStaminaCost", DisplayAfter = "bOverrideWeaponStaminaCost", EditConditionHides))
	float StaminaDamage = 0.f;

	

};
