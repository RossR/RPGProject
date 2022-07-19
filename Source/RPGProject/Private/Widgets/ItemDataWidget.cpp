// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDataWidget.h"
#include "Controllers/RPGProjectPlayerController.h"

bool UItemDataWidget::Initialize()
{
	bool b = Super::Initialize();

	PC = GetOwningPlayer<ARPGProjectPlayerController>();

	/*if (PC)
	{
		InventoryComponentRef = Cast<UInventoryComponent>(PC->GetPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
		EquipmentComponentRef = Cast<UEquipmentComponent>(PC->GetPawn()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	}*/

	return b;
}

void UItemDataWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UItemDataWidget::SwapItemsBetweenEquipmentAndInventory(UInventoryComponent* TargetInventoryComponentRef, UEquipmentComponent* TargetEquipmentComponentRef, EEquipmentSlot EquipmentSlot, int ItemKey)
{
	if (!TargetInventoryComponentRef) { return false; }
	else if (!TargetInventoryComponentRef->GetInventoryItemData(ItemKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory ItemKey is not valid."));
		return false; 
	}

	if (!TargetEquipmentComponentRef) { return false; }
	else if (!TargetEquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory EquipmentSlot is not valid."));
		return false;
	}

	UItemEquipmentData* EquipmentDataEquipmentCast = Cast<UItemEquipmentData>(TargetEquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot));
	UItemEquipmentData* InventoryDataEquipmentCast = Cast<UItemEquipmentData>(TargetInventoryComponentRef->GetInventoryItemData(ItemKey));

	// Confirm item can be equipped
	if (!(EquipmentDataEquipmentCast && InventoryDataEquipmentCast))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory Item is not equipment."));
		return false;
	}

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

		if (FirstItemData && SecondItemData)
		{
			if (FirstInventoryComponentRef->RemoveItemFromInventory(FirstItemKey))
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
			}
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
			if (FirstEquipmentComponentRef->RemoveEquipmentInSlot(FirstEquipmentSlot))
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
			}
		}
	}
	return false;
}

void UItemDataWidget::CompareItemStatistics(UItemData FirstItem, UItemData SecondItem)
{

}