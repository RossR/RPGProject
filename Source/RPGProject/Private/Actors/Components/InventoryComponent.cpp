// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InventorySize = 56;
	InventoryWidth = 8;
	InventoryHeight = InventorySize / InventoryWidth;

}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();



	// ...
	
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::AddItemToInventory(UItemData* Item, int ItemKey)
{
	// AItemBase* Item = Cast<AItemBase>(ItemActor);

	// Check that Item has been successfully casted
	if (Item)
	{
		// Add item to first empty inventory slot
		if (ItemKey == -1 || ItemKey > InventorySize)
		{
			// boolean that states whether or not the inventory is full
			bool bIsInventoryFull = true;

			// Check each inventory slot to find an empty slot
			for (int i = 0; i < InventorySize; i++)
			{
				if (!InventoryItemDataMap.Find(i))
				{
					// Populate the empty inventory slot with the item
					bIsInventoryFull = false;

					InventoryItemDataMap.Emplace(i, Item);

					// Item was successfully added to inventory
					return true;
				}
			}

			if (bIsInventoryFull)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory Could not add item as inventory is full."));
			}

		}
		// Add item to specified inventory slot
		else
		{
			// Check that the inventory slot is empty
			if (!InventoryItemDataMap.Find(ItemKey))
			{

				InventoryItemDataMap.Emplace(ItemKey, Item);

				return true;
			}
			else
			{
				UE_LOG(LogTemp,Warning, TEXT("UInventoryComponent::AddItemToInventory Could not add item as provided index is already in use."))
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory Item is NULL."));
	}

	


	// item failed to be added
	return false;
}

bool UInventoryComponent::RemoveItemFromInventory(int ItemKey)
{
	
	if (InventoryItemDataMap.Contains(ItemKey))
	{
		InventoryItemDataMap.Remove(ItemKey);
		return true;
		// FString RemovedMessage = "UInventoryComponent::RemoveItemFromInventory Item in slot '" + FString::FromInt(ItemKey) + "' has been removed.";
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveItemFromInventory ItemKey is not a valid index."));
		return false;
	}
}

bool UInventoryComponent::SwapItems(int FirstItemKey, int SecondItemKey)
{
	
	if (FirstItemKey == SecondItemKey)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SwapItems FirstItemKey & SecondItemKey have the same value."));

		return false;
	}

	// If both inventory slots are in use
	if (InventoryItemDataMap.Contains(FirstItemKey) && InventoryItemDataMap.Contains(SecondItemKey))
	{
		// 1. Store info structs of first index
		// 2. Store any other info structs of first index
		// 3. Overwrite first index info structs
		// 4. Overwrite second index info structs
		
		UItemData* FirstItemData = InventoryItemDataMap[FirstItemKey];
		
		InventoryItemDataMap[FirstItemKey] = InventoryItemDataMap[SecondItemKey];
		InventoryItemDataMap[SecondItemKey] = FirstItemData;

		return true;
	}
	else if (!InventoryItemDataMap.Contains(FirstItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SwapItems FirstItemKey is invalid."));
	}
	else if (!InventoryItemDataMap.Contains(SecondItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SwapItems SecondItemKey is invalid."));
	}
	
	return false;
}

bool UInventoryComponent::MoveItemToIndex(int CurrentItemKey, int NewItemKey)
{
	
	// Check that the item being moved has a valid index
	if (!InventoryItemDataMap.Contains(CurrentItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::MoveItemToIndex CurrentItemKey is invalid."));
		return false;
	}
	
	// Move the item info if the new index is not populated
	if (!InventoryItemDataMap.Contains(NewItemKey))
	{
		InventoryItemDataMap.Emplace(NewItemKey, InventoryItemDataMap[CurrentItemKey]);

		RemoveItemFromInventory(CurrentItemKey);

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::MoveItemToIndex NewItemKey is already populated, executing UInventoryComponent::SwapItems function."));

		return SwapItems(CurrentItemKey, NewItemKey);
	}
	
	return false;
}