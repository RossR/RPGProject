// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAssets/ItemData.h"
#include "Interfaces/InteractionInterface.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "GameFramework/Character.h"
#include "ItemBase.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class URPGProjectAnimInstance;

DECLARE_DELEGATE_OneParam(FOnItemPickupDelegate, UItemData*);

UCLASS()
class RPGPROJECT_API AItemBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual UItemData* GetItemDataDefault() { return ItemDataDefault; }

	UFUNCTION(BlueprintCallable)
	virtual UItemData* GetItemData() { return ItemData; }

	UFUNCTION(BlueprintCallable)
	virtual void SetItemData(UItemData* NewItemData);

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetItemMesh() { return ItemMesh; }

	void EnableHighlight(bool bActive, int Colour = -1) override;

	void InteractionRequested(AActor* InteractingActor) override;

	void InteractionStart(AActor* InteractingActor) override;

	void ActivateInteractable() override;

	void DeactivateInteractable() override;

	void InteractableActivated() override;

	void InteractableDeactivated() override;

	bool GetIsInInteractableRange(AActor* InteractingActor) override;

	EInteractableState GetInteractableState() override { return EInteractableState::MAX; };

	bool CanBeInteractedWith() { return true; }

	virtual void ItemPickupEvent() { OnItemPickup.ExecuteIfBound(ItemData); }

	UFUNCTION(BlueprintCallable)
	void SetDurability(float NewDurability);

protected:

	virtual void UpdateCurves();

public:

	FOnItemPickupDelegate OnItemPickup;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Mesh")
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Item Data Override")
	UItemData* ItemDataOverride;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Item | Item Data")
	UItemData* ItemDataDefault;

	UPROPERTY(BlueprintReadOnly, Category = "Item | Owner References")
	USkeletalMeshComponent* OwnerSkeletalMeshComponentRef;

	UPROPERTY(BlueprintReadOnly, Category = "Item | Owner References")
	URPGProjectAnimInstance* OwnerAnimInstanceRef;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemData* ItemData;*/

private:

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "TEST")
	UItemData* ItemData;

};
