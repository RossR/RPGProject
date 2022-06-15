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

	// Confirm item can be equipped
	if (!(EquipmentDataEquipmentCast && InventoryDataEquipmentCast))
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemDataWidget::SwapItemBetweenEquipmentAndInventory Item is not equipment."));
		return false;
	}

	const bool bEquippedSuccessfully = EquipmentComponentRef->Equip(InventoryDataEquipmentCast, EquipmentSlot, ItemKey);

	if (!bEquippedSuccessfully)
	{
		InventoryComponentRef->AddItemToInventory(InventoryDataEquipmentCast, ItemKey);
	}

	return bEquippedSuccessfully;
}

void UItemDataWidget::CompareItemStatistics(UItemData FirstItem, UItemData SecondItem)
{

}