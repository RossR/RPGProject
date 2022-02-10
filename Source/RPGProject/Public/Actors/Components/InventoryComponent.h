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
	bool AddItemToInventory(AActor* ItemActor, int ItemKey = -1);// FItemInfo ItemInfo);

	UFUNCTION(BlueprintCallable)
	// Removes the item stored in the provided key
	bool RemoveItemFromInventory(int ItemKey);

	UFUNCTION(BlueprintCallable)
	bool SwapItems(int FirstItemKey, int SecondItemKey);

	UFUNCTION(BlueprintCallable)
	bool MoveItemToIndex(int CurrentItemKey, int NewItemKey);

	UFUNCTION(BlueprintCallable)
	UItemData* GetInventoryItemData(int ItemKey) { return InventoryItemDataMap[ItemKey]; }

	UFUNCTION(BlueprintCallable)
	TMap<int, UItemData*> GetInventoryItemDataMap() { return InventoryItemDataMap; }


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<int, UItemData*> InventoryItemDataMap;

	
	// Struct of structs / parent structs and child structs
	// pointers to data assets

};
