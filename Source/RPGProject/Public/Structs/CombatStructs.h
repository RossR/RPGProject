// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/CombatEnums.h"
#include "CombatStructs.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAttackInfo
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ETraceType AttackTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float AttackDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float StaminaDamage = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float StaminaCost = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bUsesAmmunition = false;
};

USTRUCT(BlueprintType)
struct FCombatActionInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* CombatActionMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActionIsAnAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bActionIsAnAttack", DisplayAfter = "bActionIsAnAttack", EditConditionHides))
	FWeaponAttackInfo ActionAttackInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRepeatIfCombatActionButtonPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequiresAmmo = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AmmoRequired = 0.f;
};