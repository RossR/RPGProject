// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/HighlightInterface.h"
#include "InteractionInterface.generated.h"

UENUM(BlueprintType)
enum class EInteractableState : uint8
{
	Inactive		UMETA(DisplayName = "Inactive"),
	Activating		UMETA(DisplayName = "Activating"),
	Deactivating	UMETA(DisplayName = "Deactivating"),
	Active			UMETA(DisplayName = "Active"),
	MAX				UMETA(Hidden)
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInteractionInterface : public UHighlightInterface
{
	GENERATED_BODY()
	//GENERATED_UINTERFACE_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IInteractionInterface : public IHighlightInterface
{
	GENERATED_BODY()
	//GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable)
	virtual void InteractionRequested(AActor* InteractingActor) = 0;
	//virtual void InteractionRequested_Implementation(AActor* InteractingActor) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void InteractionStart(AActor* InteractingActor) = 0;
	//virtual void InteractionStart_Implementation(AActor* InteractingActor) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ActivateInteractable() = 0;
	//virtual void ActivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void DeactivateInteractable() = 0;
	//virtual void ActivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void InteractableActivated() = 0;
	//virtual void ActivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void InteractableDeactivated() = 0;
	//virtual void ActivateInteractable_Implementation() = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool GetIsInInteractableRange(AActor* InteractingActor) = 0;

	UFUNCTION(BlueprintCallable)
	virtual EInteractableState GetInteractableState() = 0;
};
