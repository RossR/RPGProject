// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/WeaponBase.h"
#include "InventoryComponent.generated.h"

//class AItemBase;

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
	FItemInfo GetInventoryItemInfo(int ItemKey) { return InventoryItemMap[ItemKey]; }

	UFUNCTION(BlueprintCallable)
	FWeaponInfo GetInventoryWeaponInfo(int ItemKey) { return InventoryWeaponInfoMap[ItemKey]; }

	UFUNCTION(BlueprintCallable)
	TMap<int, FItemInfo> GetInventoryItemMap() { return InventoryItemMap; }

	UFUNCTION(BlueprintCallable)
	TMap<int, FWeaponInfo> GetInventoryWeaponInfoMap() { return InventoryWeaponInfoMap; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventorySize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryWidth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int InventoryHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<int, FItemInfo> InventoryItemMap;
	//TMap<int, TSubclassOf<AItemBase>> CharacterInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TMap<int, FWeaponInfo> InventoryWeaponInfoMap;
	
};
