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

	// React to the interaction request from the interacting actor
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void InteractionRequested(AActor* InteractingActor = nullptr) = 0;

	// Start this actor's interaction with the interacting actor, calling either ActivateInteractable or DeactivateInteractable depending on the actor's interactable state
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void StartInteraction(AActor* InteractingActor = nullptr) = 0;

	// This function should be called by StartInteraction and should set the interactable state of the actor to activating
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void ActivateInteractable() = 0;

	// This function should be called by StartInteraction and should set the interactable state of the actor to deactivating
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void DeactivateInteractable() = 0;

	// This function should be called when the actor's interactable state is set to activated
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void InteractableHasActivated() = 0;

	// This function should be called when the actor's interactable state is set to deactivated
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual void InteractableHasDeactivated() = 0;

	// Returns true if the interacting actor is within range to interact with this actor
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual bool GetIsInInteractableRange(AActor* InteractingActor) const = 0;

	// Returns the actor's interactable state
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual EInteractableState GetInteractableState() const = 0;

	// Returns true if the actor can be interacted with
	UFUNCTION(BlueprintCallable, Category = "Interaction Interface")
	virtual bool CanActorBeInteractedWith() const = 0;
};
