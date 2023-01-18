// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Animation/AnimMontage.h"
#include "SkillData.generated.h"


UCLASS(BlueprintType)
class RPGPROJECT_API USkillData : public UDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	FName SkillName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	FText SkillDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Data")
	UAnimMontage* SkillMontage;

};