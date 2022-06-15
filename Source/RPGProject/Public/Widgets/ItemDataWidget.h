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


public:

	// --- FUNCTIONS --- //

	bool Initialize() override;

	virtual void NativeConstruct() override;

	// ToDo - Swap Equipment & inventory
	UFUNCTION(BlueprintCallable)
	bool SwapItemsBetweenEquipmentAndInventory(EEquipmentSlot EquipmentSlot, int ItemKey);

	// ToDo - Stat comparison
	void CompareItemStatistics(UItemData FirstItem, UItemData SecondItem);
	
protected:

	// --- VARIABLES --- //
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PC")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory Component")
	UInventoryComponent* InventoryComponentRef;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory Component")
	UEquipmentComponent* EquipmentComponentRef;

};
