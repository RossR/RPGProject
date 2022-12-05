// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RPGFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPGFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*> &CurrentlyRenderedActors, FName ActorTag, float MinRecentTime = 0.01f, float ViewAngle = 20.f);
	
};
