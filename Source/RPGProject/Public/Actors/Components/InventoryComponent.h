// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/ItemWeapon.h"
#include "Actors/ItemTypes/ItemEquipment.h"
#include "Actors/ItemTypes/ItemConsumable.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	// Adds an item to the inventory, set ItemKey to -1 to fill the first empty inventory slot
	bool AddItemToInventory(UItemData* Item, int ItemKey = -1);// FItemInfo ItemInfo);

	UFUNCTION(BlueprintCallable)
	// Removes the item stored in the provided key
	bool RemoveItemFromInventory(int ItemKey);

	UFUNCTION(BlueprintCallable)
	bool SwapItems(int FirstItemKey, int SecondItemKey);

	UFUNCTION(BlueprintCallable)
	bool MoveItemToIndex(int CurrentItemKey, int NewItemKey);

	UFUNCTION(BlueprintCallable)
	UItemData* GetInventoryItemData(int ItemKey) { if (InventoryItemDataMap.Contains(ItemKey)) { return InventoryItemDataMap[ItemKey]; } return nullptr; }

	UFUNCTION(BlueprintCallable)
	TMap<int, UItemData*> GetInventoryItemDataMap() { return InventoryItemDataMap; }

	UFUNCTION(BlueprintPure)
	int GetEmptyInventorySpace() { return (InventorySize - InventoryItemDataMap.Num()); }

	// ToDo - Add functionality to calculate total weight of inventory
	UFUNCTION(BlueprintPure)
	float GetInventoryWeight();

	UFUNCTION(BlueprintPure)
	float GetTotalWeight();

	UFUNCTION(BlueprintCallable)
	void AddCurrency(FCurrency CurrencyToAdd, bool bConvertCurrencies = true);

	UFUNCTION(BlueprintCallable)
	bool RemoveCurrency(FCurrency CurrencyToRemove);

	UFUNCTION(BlueprintCallable)
	void ConvertCurrencies();

	UFUNCTION(BlueprintPure)
	TMap<int, TSubclassOf<AItemBase>> GetStartingInventoryItems() { return StartingInventoryItems; }

	UFUNCTION(BlueprintCallable)
	void AddStartingItems();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<int, UItemData*> InventoryItemDataMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	FCurrency Currency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Starting Inventory");
	TMap<int, TSubclassOf<AItemBase>> StartingInventoryItems;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency Exchange Ratios")
	int CopperToSilverRatio = 10;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency Exchange Ratios")
	int SilverToGoldRatio = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency Exchange Ratios")
	int GoldToPlatinumRatio = 100;
	// Struct of structs / parent structs and child structs
	// pointers to data assets

};
