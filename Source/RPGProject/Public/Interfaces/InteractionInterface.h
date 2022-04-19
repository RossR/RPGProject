// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/HighlightInterface.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractionInterface : public UHighlightInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IInteractionInterface : public IHighlightInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void InteractionStart(AActor* InteractingActor) = 0;

};
