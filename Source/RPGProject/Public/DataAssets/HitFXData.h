// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "HitFXData.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USoundCue;

USTRUCT(BlueprintType)
struct FHitFXInfo : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* SoundCue;

};

UCLASS()
class RPGPROJECT_API UHitFXData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit FX Info")
	TMap<TEnumAsByte<EPhysicalSurface>, FHitFXInfo> HitVFXMap;

};

