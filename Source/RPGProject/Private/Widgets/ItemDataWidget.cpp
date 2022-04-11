// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDataWidget.h"
#include "Controllers/RPGProjectPlayerController.h"

bool UItemDataWidget::Initialize()
{
	bool b = Super::Initialize();

	PC = GetOwningPlayer<ARPGProjectPlayerController>();

	if (PC)
	{
		InventoryComponentRef = Cast<UInventoryComponent>(PC->GetPawn()->GetComponentByClass(UInventoryComponent::StaticClass()));
		EquipmentComponentRef = Cast<UEquipmentComponent>(PC->GetPawn()->GetComponentByClass(UEquipmentComponent::StaticClass()));
	}

	return b;
}

void UItemDataWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

bool UItemDataWidget::SwapItemsBetweenEquipmentAndInventory(EEquipmentSlot EquipmentSlot, int ItemKey)
{
	if (!InventoryComponentRef) { return false; }
	else if (!InventoryComponentRef->GetInventoryItemData(ItemKey)) 
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory ItemKey is not valid."));
		return false; 
	}

	if (!EquipmentComponentRef) { return false; }
	else if (!EquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory EquipmentSlot is not valid."));
		return false;
	}

	UItemEquipmentData* EquipmentDataEquipmentCast = Cast<UItemEquipmentData>(EquipmentComponentRef->GetWornEquipmentDataInSlot(EquipmentSlot));
	UItemEquipmentData* InventoryDataEquipmentCast = Cast<UItemEquipmentData>(InventoryComponentRef->GetInventoryItemData(ItemKey));

	bool bCanSwap = false;
	// Confirm item can be equipped
	if (EquipmentDataEquipmentCast && InventoryDataEquipmentCast)
	{
		
		if (InventoryDataEquipmentCast->EquipmentType == EEquipmentType::EET_1HWeapon)
		{
			// Can swap if Equipment is main-hand or off-hand
			if (EquipmentDataEquipmentCast->EquipmentType == EEquipmentType::EET_1HWeapon || EquipmentDataEquipmentCast->EquipmentType == EEquipmentType::EET_OffHand)
			{
				bCanSwap = true;
			}
		}
		else if (InventoryDataEquipmentCast->EquipmentType == EquipmentDataEquipmentCast->EquipmentType)
		{
			bCanSwap = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory Item is not equipment."));
		return false;
	}

	if (!bCanSwap) 
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory Items cannot be swapped."));
		return false;
	}

	// Remove equipment, then equip new item
	
	EquipmentComponentRef->RemoveWornEquipmentDataInSlot(EquipmentSlot);

	if (!EquipmentComponentRef->GetWornEquipmentData().Contains(EquipmentSlot))
	{
		const bool EquipSuccessful = EquipmentComponentRef->Equip(InventoryDataEquipmentCast, EquipmentSlot);

		if (!EquipSuccessful)
		{
			if (!EquipmentComponentRef->GetWornEquipmentData().Contains(EquipmentSlot))
			{
				EquipmentComponentRef->Equip(EquipmentDataEquipmentCast, EquipmentSlot);
			}
			
			return false;
		}
	}

	// Remove item from inventory, then add equipment to inventory (same slot if possible)

	InventoryComponentRef->RemoveItemFromInventory(ItemKey);

	if (!InventoryComponentRef->GetInventoryItemDataMap().Contains(ItemKey))
	{
		const bool AddSuccessful = InventoryComponentRef->AddItemToInventory(EquipmentDataEquipmentCast, ItemKey);

		if (!AddSuccessful)
		{
			if (!InventoryComponentRef->GetInventoryItemDataMap().Contains(ItemKey))
			{
				InventoryComponentRef->AddItemToInventory(InventoryDataEquipmentCast, ItemKey);
			}

			return false;
		}
	}
	
	return true;
}

void UItemDataWidget::CompareItemStatistics(UItemData FirstItem, UItemData SecondItem)
{

}