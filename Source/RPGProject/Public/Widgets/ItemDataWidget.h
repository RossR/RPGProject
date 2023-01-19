// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DataAssets/ItemData.h"
#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "ItemDataWidget.generated.h"

class ARPGProjectPlayerController;

UCLASS()
class RPGPROJECT_API UItemDataWidget : public UUserWidget
{
	GENERATED_BODY()


public:		// --- FUNCTIONS --- //

	bool Initialize() override;

	virtual void NativeConstruct() override;

	/*
	* Attempts to swap items between an inventory component and an equipment component, returns true is the swap was successful
	* @param TargetInventoryComponentRef Reference to the inventory component
	* @param ItemKey Location of the item to be swapped from the inventory equipment
	* @param TargetEquipmentComponentRef Reference to the equipment component
	* @param EquipmentSlot Location of the item to be swapped from the equipment component
	*/
	UFUNCTION(BlueprintCallable)
	bool SwapItemsBetweenEquipmentAndInventory(UInventoryComponent* TargetInventoryComponentRef, int ItemKey, UEquipmentComponent* TargetEquipmentComponentRef, EEquipmentSlot EquipmentSlot);

	/*
	* Attempts to swap items between two different inventory components, returns true is the swap was successful
	* @param FirstInventoryComponentRef Reference to the first inventory component
	* @param FirstItemKey Location of the item to be swapped from the first inventory component
	* @param SecondInventoryComponentRef Reference to the second inventory component
	* @param SecondItemKey Location of the item to be swapped from the second inventory component
	*/
	UFUNCTION(BlueprintCallable)
	bool SwapItemsBetweenInventoryComponents(UInventoryComponent* FirstInventoryComponentRef, int FirstItemKey, UInventoryComponent* SecondInventoryComponentRef, int SecondItemKey);

	/*
	* Attempts to swap items between two different inventory components, returns true if the swap was successful
	* @param FirstEquipmentComponentRef Reference to the first equipment component
	* @param FirstEquipmentSlot Location of the item to be swapped from the first equipment component
	* @param SecondEquipmentComponentRef Reference to the second equipment component
	* @param SecondEquipmentSlot Location of the item to be swapped from the second equipment component
	*/
	UFUNCTION(BlueprintCallable)
	bool SwapItemsBetweenEquipmentComponents(UEquipmentComponent* FirstEquipmentComponentRef, EEquipmentSlot FirstEquipmentSlot, UEquipmentComponent* SecondEquipmentComponentRef, EEquipmentSlot SecondEquipmentSlot);

	// ToDo - Stat comparison
	//void CompareItemStatistics(UItemData FirstItem, UItemData SecondItem);
	
	UFUNCTION(BlueprintCallable)
	void SetInventoryComponentRef(UInventoryComponent* InInventoryComponentRef) { InventoryComponentRef = InInventoryComponentRef; }

	UFUNCTION(BlueprintCallable)
	void SetEquipmentComponentRef(UEquipmentComponent* InEquipmentComponentRef) { EquipmentComponentRef = InEquipmentComponentRef; }

protected:	// --- VARIABLES --- //

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Data Widget|References")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Data Widget|References")
	UInventoryComponent* InventoryComponentRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item Data Widget|References")
	UEquipmentComponent* EquipmentComponentRef;

};
