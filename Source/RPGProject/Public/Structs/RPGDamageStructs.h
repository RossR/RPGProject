// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGDamageStructs.generated.h"

/** Damage subclass that handles damage with a single impact location and source direction */
USTRUCT(BlueprintType)
struct FBodyPartDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	UPROPERTY()
	FName BodyPartHit;

	FBodyPartDamageEvent() : BodyPartHit(NAME_None) {}
	FBodyPartDamageEvent(float InDamage, struct FHitResult const& InHitInfo, FVector const& InShotDirection, TSubclassOf<class UDamageType> InDamageTypeClass, FName InBodyPartHit)
		: FDamageEvent(InDamageTypeClass), BodyPartHit(InBodyPartHit)
	{}

	/** ID for this class. NOTE this must be unique for all damage events. */
	static const int32 ClassID = 100;

	virtual int32 GetTypeID() const override { return FBodyPartDamageEvent::ClassID; };
	virtual bool IsOfType(int32 InID) const override { return (FBodyPartDamageEvent::ClassID == InID) || FDamageEvent::IsOfType(InID); };

	/** Simple API for common cases where we are happy to assume a single hit is expected, even though damage event may have multiple hits. */
	//virtual void GetBestHitInfo(AActor const* HitActor, AActor const* HitInstigator, struct FHitResult& OutHitInfo, FVector& OutImpulseDir) const override;
};