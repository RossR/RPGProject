// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDataWidget.h"
#include "Controllers/RPGProjectPlayerController.h"

bool UItemDataWidget::Initialize()
{
	bool b = Super::Initialize();

	PC = GetOwningPlayer<ARPGProjectPlayerController>();

	return b;
}

void UItemDataWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UItemDataWidget::SwapItemsBetweenEquipmentAndInventory(UInventoryComponent* TargetInventoryComponentRef, int ItemKey, UEquipmentComponent* TargetEquipmentComponentRef, EEquipmentSlot EquipmentSlot)
{
	// Validate the inventory component and inventory item 
	if (!TargetInventoryComponentRef) { return false; }
	else if (!TargetInventoryComponentRef->GetInventoryItemData(ItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory ItemKey is not valid."));
		return false; 
	}

	// Validate the equipment component and the equipment item 
	if (!TargetEquipmentComponentRef) { return false; }
	else if (!TargetEquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory EquipmentSlot is not valid."));
		return false;
	}

	UItemEquipmentData* EquipmentDataEquipmentCast = Cast<UItemEquipmentData>(TargetEquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot));
	UItemEquipmentData* InventoryDataEquipmentCast = Cast<UItemEquipmentData>(TargetInventoryComponentRef->GetInventoryItemData(ItemKey));

	// Confirm that inventory item can be equipped
	if (!InventoryDataEquipmentCast)
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory Item is not equipment."));
		return false;
	}

	// Try to equip the inventory item and put the equipment item in the inventory
	const bool bEquippedSuccessfully = TargetEquipmentComponentRef->Equip(InventoryDataEquipmentCast, EquipmentSlot, TargetInventoryComponentRef, ItemKey);

	if (!bEquippedSuccessfully)
	{
		TargetInventoryComponentRef->AddItemToInventory(InventoryDataEquipmentCast, ItemKey);
	}

	return bEquippedSuccessfully;
}

bool UItemDataWidget::SwapItemsBetweenInventoryComponents(UInventoryComponent* FirstInventoryComponentRef, int FirstItemKey, UInventoryComponent* SecondInventoryComponentRef, int SecondItemKey)
{
	const bool bSameInventoryComponentRef = FirstInventoryComponentRef == SecondInventoryComponentRef;

	if (bSameInventoryComponentRef)
	{
		// Will return false if the item keys are the same
		return FirstInventoryComponentRef->SwapItems(FirstItemKey, SecondItemKey);
	}
	else
	{
		UItemData* FirstItemData = FirstInventoryComponentRef->GetInventoryItemData(FirstItemKey);
		UItemData* SecondItemData = SecondInventoryComponentRef->GetInventoryItemData(SecondItemKey);

		// Validate both items
		if (FirstItemData && SecondItemData)
		{
			// Remove items from both inventory components
			if (FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey) && SecondInventoryComponentRef->RemoveItemFromInventory(SecondItemKey))
			{
				// Swap the items between inventory components
				if (FirstInventoryComponentRef->AddItemToInventory(SecondItemData, FirstItemKey) && SecondInventoryComponentRef->AddItemToInventory(SecondItemData, SecondItemKey))
				{
					return true;
				}
				// If the swap fails, put everything back where it came from
				else
				{
					FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey);
					SecondInventoryComponentRef->RemoveItemFromInventory(SecondItemKey);

					FirstInventoryComponentRef->AddItemToInventory(FirstItemData, FirstItemKey);
					SecondInventoryComponentRef->AddItemToInventory(SecondItemData, SecondItemKey);
				}
			}
			/*if (FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey))
			{
				if (FirstInventoryComponentRef->AddItemToInventory(SecondItemData, FirstItemKey))
				{
					if (SecondInventoryComponentRef->RemoveItemFromInventory(SecondItemKey))
					{
						if (SecondInventoryComponentRef->AddItemToInventory(SecondItemData, SecondItemKey))
						{
							return true;
						}
						else
						{
							FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey);
							FirstInventoryComponentRef->AddItemToInventory(FirstItemData, FirstItemKey);

							SecondInventoryComponentRef->RemoveItemFromInventory(SecondItemKey);
							SecondInventoryComponentRef->AddItemToInventory(SecondItemData, SecondItemKey);
						}
					}
					else
					{
						FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey);
						FirstInventoryComponentRef->AddItemToInventory(FirstItemData, FirstItemKey);
					}
				}
				else
				{
					FirstInventoryComponentRef->AddItemToInventory(FirstItemData, FirstItemKey);
				}
			}*/
		}
	}
	return false;
}

bool UItemDataWidget::SwapItemsBetweenEquipmentComponents(UEquipmentComponent* FirstEquipmentComponentRef, EEquipmentSlot FirstEquipmentSlot, UEquipmentComponent* SecondEquipmentComponentRef, EEquipmentSlot SecondEquipmentSlot)
{
	const bool bSameEquipmentComponentRef = FirstEquipmentComponentRef == SecondEquipmentComponentRef;

	if (bSameEquipmentComponentRef)
	{
		// Will return false if the item keys are the same
		return FirstEquipmentComponentRef->SwapEquipment(FirstEquipmentSlot, SecondEquipmentSlot);
	}
	else
	{
		UItemData* FirstItemData = FirstEquipmentComponentRef->GetWornEquipmentDataInSlot(FirstEquipmentSlot);
		UItemData* SecondItemData = SecondEquipmentComponentRef->GetWornEquipmentDataInSlot(SecondEquipmentSlot);

		if (FirstItemData && SecondItemData)
		{
			// Remove items from both equipment components
			if (FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot) && SecondEquipmentComponentRef->RemoveEquipmentInSlot(SecondEquipmentSlot))
			{
				// Swap the items between equipment components
				if (FirstEquipmentComponentRef->Equip(SecondItemData, FirstEquipmentSlot, nullptr, -1, false) && SecondEquipmentComponentRef->Equip(FirstItemData, SecondEquipmentSlot, nullptr, -1, false))
				{
					return true;
				}
				// If the swap fails, put everything back where it came from
				else
				{
					FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot);
					SecondEquipmentComponentRef->RemoveEquipmentInSlot(SecondEquipmentSlot);

					FirstEquipmentComponentRef->Equip(FirstItemData, FirstEquipmentSlot, nullptr, -1, false);
					SecondEquipmentComponentRef->Equip(SecondItemData, SecondEquipmentSlot, nullptr, -1, false);
				}
			}
			/*if (FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot))
			{
				if (FirstEquipmentComponentRef->Equip(SecondItemData, FirstEquipmentSlot, nullptr, -1, false))
				{
					if (SecondEquipmentComponentRef->RemoveEquipmentInSlot(SecondEquipmentSlot))
					{
						if (SecondEquipmentComponentRef->Equip(FirstItemData, SecondEquipmentSlot, nullptr, -1, false))
						{
							return true;
						}
						else
						{
							FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot);
							FirstEquipmentComponentRef->Equip(FirstItemData, FirstEquipmentSlot, nullptr, -1, false);
							
							SecondEquipmentComponentRef->RemoveEquipmentInSlot(SecondEquipmentSlot);
							SecondEquipmentComponentRef->Equip(SecondItemData, SecondEquipmentSlot, nullptr, -1, false);
						}
					}
					else
					{
						FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot);
						FirstEquipmentComponentRef->Equip(FirstItemData, FirstEquipmentSlot, nullptr, -1, false);
					}
				}
				else
				{
					FirstEquipmentComponentRef->Equip(FirstItemData, FirstEquipmentSlot, nullptr, -1, false);
				}
			}*/
		}
	}
	return false;
}

//void UItemDataWidget::CompareItemStatistics(UItemData FirstItem, UItemData SecondItem)
//{
//
//}