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

bool UInventoryComponent::AddItemToInventory(AActor* ItemActor, int ItemKey)
{
	AItemBase* Item = Cast<AItemBase>(ItemActor);

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
				if (!InventoryItemMap.Find(i))
				{
					// Populate the empty inventory slot with the item
					bIsInventoryFull = false;
					InventoryItemMap.Emplace(i, Item->GetItemInfo());

					switch (Item->GetItemInfo().ItemCategory)
					{
					case EItemCategory::EIC_Default:
						break;
					case EItemCategory::EIC_Consumable:
						break;
					case EItemCategory::EIC_Weapon:
					{
						AWeaponBase* WeaponItem = Cast<AWeaponBase>(ItemActor);

						if (WeaponItem)
						{
							InventoryWeaponInfoMap.Emplace(i, WeaponItem->GetWeaponInfo());
						}
						else
						{
							UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory WeaponItem is NULL."))
						}

						break;
					}
					default:
						break;
					}

					// Item was successfully added to inventory
					return true;
				}
			}

			if (bIsInventoryFull)
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory Could not add item as inventory is full."));
			}

		}
		else
		{
			if (!InventoryItemMap.Find(ItemKey))
			{
				InventoryItemMap.Emplace(ItemKey, Item->GetItemInfo());

				switch (Item->GetItemInfo().ItemCategory)
				{
				case EItemCategory::EIC_Default:
					break;
				case EItemCategory::EIC_Consumable:
					break;
				case EItemCategory::EIC_Weapon:
				{
					AWeaponBase* WeaponItem = Cast<AWeaponBase>(ItemActor);

					if (WeaponItem)
					{
						InventoryWeaponInfoMap.Emplace(ItemKey, WeaponItem->GetWeaponInfo());
					}
					else
					{
						UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory WeaponItem is NULL."))
					}

					break;
				}
				default:
					break;
				}
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
	if (InventoryItemMap.Contains(ItemKey))
	{
		InventoryItemMap.Remove(ItemKey);
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
	else if (InventoryItemMap.Contains(FirstItemKey) && InventoryItemMap.Contains(SecondItemKey))
	{
		// 1. Store info structs of first index
		// 2. Store any other info structs of first index
		// 3. Overwrite first index info structs
		// 4. Overwrite second index info structs
		
		FItemInfo FirstItemInfo = InventoryItemMap[FirstItemKey];
		FWeaponInfo FirstWeaponInfo;
		// Any other info type
		
		InventoryItemMap[FirstItemKey] = InventoryItemMap[SecondItemKey];
		InventoryItemMap[SecondItemKey] = FirstItemInfo;

		if (InventoryWeaponInfoMap.Contains(FirstItemKey) && InventoryWeaponInfoMap.Contains(SecondItemKey))
		{
			FirstWeaponInfo = InventoryWeaponInfoMap[FirstItemKey];

			InventoryWeaponInfoMap[FirstItemKey] = InventoryWeaponInfoMap[SecondItemKey];
			InventoryWeaponInfoMap[SecondItemKey] = FirstWeaponInfo;
		}
		else if (InventoryWeaponInfoMap.Contains(FirstItemKey) || InventoryWeaponInfoMap.Contains(SecondItemKey))
		{
			if (InventoryWeaponInfoMap.Contains(FirstItemKey))
			{
				InventoryWeaponInfoMap.Emplace(SecondItemKey, InventoryWeaponInfoMap[FirstItemKey]);
				InventoryWeaponInfoMap.Remove(FirstItemKey);
			}
			else if (InventoryWeaponInfoMap.Contains(SecondItemKey))
			{
				InventoryWeaponInfoMap.Emplace(FirstItemKey, InventoryWeaponInfoMap[SecondItemKey]);
				InventoryWeaponInfoMap.Remove(SecondItemKey);
			}
		}

		return true;

	}
	else if (!InventoryItemMap.Contains(FirstItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SwapItems FirstItemKey is invalid."));
	}
	else if (!InventoryItemMap.Contains(SecondItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::SwapItems SecondItemKey is invalid."));
	}

	return false;
}

bool UInventoryComponent::MoveItemToIndex(int CurrentItemKey, int NewItemKey)
{

	// Check that the item being moved has a valid index
	if (!InventoryItemMap.Find(CurrentItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::MoveItemToIndex CurrentItemKey is invalid."));
		return false;
	}

	// Move the item info if the new index is not populated
	if (!InventoryItemMap.Find(NewItemKey))
	{
		InventoryItemMap.Emplace(NewItemKey, InventoryItemMap[CurrentItemKey]);
		InventoryItemMap.Remove(CurrentItemKey);

		if (InventoryWeaponInfoMap.Find(CurrentItemKey))
		{
			InventoryWeaponInfoMap.Emplace(NewItemKey, InventoryWeaponInfoMap[CurrentItemKey]);
			InventoryWeaponInfoMap.Remove(CurrentItemKey);
		}

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::MoveItemToIndex NewItemKey is already populated, executing UInventoryComponent::SwapItems function."));

		return SwapItems(CurrentItemKey, NewItemKey);
	}
}