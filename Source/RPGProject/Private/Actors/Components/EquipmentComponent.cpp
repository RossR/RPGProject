// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/CharacterStatisticComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CreateEquipmentChildActors();
	UpdateEquipmentChildActors();

	
}

bool UEquipmentComponent::RemoveWornEquipmentDataInSlot(EEquipmentSlot EquipmentSlot)
{
	if (WornEquipmentDataMap.Contains(EquipmentSlot))
	{
		WornEquipmentDataMap.Remove(EquipmentSlot);
		UpdateEquipmentChildActors();
		return true;
	}

	return false;
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());

	OwnerInventoryComponentRef = Cast<UInventoryComponent>(OwningCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));

	EquipStartingEquipment();
	UpdateEquipmentChildActors();
}

// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

bool UEquipmentComponent::Equip(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo, UInventoryComponent* InInventoryComponentRef, int ItemToEquipInventoryKey, bool bRemoveFromInventoryOnSuccessfulEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip called."));

	if (!InInventoryComponentRef)
	{
		if (OwnerInventoryComponentRef)
		{
			InInventoryComponentRef = OwnerInventoryComponentRef;	
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip InventoryComponentRef is nullptr."));
			return false;
		}
	}

	// Check that the item is a piece of equipment
	const UItemEquipmentData* ItemEquipmentCast = Cast<UItemEquipmentData>(ItemToEquip);
	if (!ItemEquipmentCast)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip ItemToEquip is not equipment."));
		return false;
	}

	// Check that the item can be equipped to an equipment slot
	EEquipmentSlot EquipSlot;

	SlotToEquipTo != EEquipmentSlot::ES_None ? EquipSlot = SlotToEquipTo : EquipSlot = GetEquipmentSlotForItem(ItemToEquip);
	if (EquipSlot == EEquipmentSlot::ES_None) { return false; }

	const EEquipmentSlot ValidEquipmentSlot = GetEquipmentSlotForItem(ItemToEquip);
	// Check that SlotToEquipTo is valid for ItemToEquip
	if (SlotToEquipTo != EEquipmentSlot::ES_None && ValidEquipmentSlot != SlotToEquipTo)
	{
		bool IsValidSlot = false;

		switch (SlotToEquipTo)
		{
		case EEquipmentSlot::ES_MainHandOne:
			if (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo)
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::ES_OffHandOne:
			if (ItemEquipmentCast->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo))
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::ES_MainHandTwo:
			if (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo)
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::ES_OffHandTwo:
			if (ItemEquipmentCast->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne))
			{
				IsValidSlot = true;
			}
			break;

		default:
			break;
		}

		if (!IsValidSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip SlotToEquipTo is not a valid slot for ItemToEquip."));
			return false;
		}
	}

	// Check if character has stats required to equip the item
	if (UCharacterStatisticComponent* CharacterStatisticComponent = Cast<UCharacterStatisticComponent>(OwningCharacter->GetComponentByClass(UCharacterStatisticComponent::StaticClass())))
	{
		if (CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Strength) < ItemEquipmentCast->RequiredStrength ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Dexterity) < ItemEquipmentCast->RequiredDexterity ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Vitality) < ItemEquipmentCast->RequiredVitality ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Grit) < ItemEquipmentCast->RequiredGrit ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Intelligence) < ItemEquipmentCast->RequiredIntelligence ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Wisdom) < ItemEquipmentCast->RequiredWisdom ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Charisma) < ItemEquipmentCast->RequiredCharisma ||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Luck) < ItemEquipmentCast->RequiredLuck)
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Character does not meet the stat requirements to equip the item."));
			return false;
		}
	}

	if (EquipSlot == EEquipmentSlot::ES_OffHandOne)
	{
		UItemEquipmentData* MainHandCheck = nullptr;
		WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandOne) ? MainHandCheck = Cast<UItemEquipmentData>(WornEquipmentDataMap[EEquipmentSlot::ES_MainHandOne]) : false;

		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}

	if (EquipSlot == EEquipmentSlot::ES_OffHandTwo)
	{
		UItemEquipmentData* MainHandCheck = nullptr;
		WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandOne) ? MainHandCheck = Cast<UItemEquipmentData>(WornEquipmentDataMap[EEquipmentSlot::ES_MainHandTwo]) : false;

		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}
	
	
	
	// If equipment slot is empty
	if (!WornEquipmentDataMap.Contains(EquipSlot))
	{
		WornEquipmentDataMap.Emplace(EquipSlot, ItemToEquip);

		if (WornEquipmentActorMap[EquipSlot]->GetChildActorClass() == nullptr)
		{
			//WornEquipmentActorMap[EquipSlot]->SetChildActorClass(WornEquipmentDataMap[EquipSlot]->ClassToSpawn.Get());
			WornEquipmentActorMap[EquipSlot]->SetChildActorClass(ItemToEquip->ClassToSpawn.Get());

			AItemEquipment* EquippedItem = Cast<AItemEquipment>(WornEquipmentActorMap[EquipSlot]->GetChildActor());
			if (EquippedItem)
			{
				// Copy ItemData to child actor
				//EquippedItem->SetItemData(WornEquipmentDataMap[EquipSlot]);
				EquippedItem->SetItemData(ItemToEquip);

				WornEquipmentDataMap.Emplace(EquipSlot, EquippedItem->GetItemData());

				AttachEquipmentToSocket(EquipSlot);

				// If weapon, attach to appropriate slot
				/*if (AItemWeapon* EquippedWeapon = Cast<AItemWeapon>(EquippedItem))
				{
					AttachEquipmentToSocket(EquipSlot);
				}*/

				// Change the child actor's collision profile so it does not hit the interaction trace
				EquippedItem->GetItemMesh()->SetCollisionProfileName("EquippedItem");
				if (OwningCharacter)
				{
					OwningCharacter->MoveIgnoreActorAdd(EquippedItem);
					EquippedItem->GetItemMesh()->SetCanEverAffectNavigation(false);
					EquippedItem->GetItemMesh()->SetSimulatePhysics(false);
				}
			}
		}

		if (bRemoveFromInventoryOnSuccessfulEquip && WornEquipmentDataMap.Contains(EquipSlot))
		{
			if (ItemToEquipInventoryKey >= 0 && InInventoryComponentRef->GetInventoryItemDataMap().Contains(ItemToEquipInventoryKey))
			{
				InInventoryComponentRef->RemoveItemFromInventory(ItemToEquipInventoryKey);
			}
		}
		
		return true;
	}
	// If equipment slot is already in use
	else
	{
		// Unequip the current equipment in slot and equip the new item
		if (ItemToEquipInventoryKey >= 0 && InInventoryComponentRef->GetInventoryItemDataMap().Contains(ItemToEquipInventoryKey))
		{
			InInventoryComponentRef->RemoveItemFromInventory(ItemToEquipInventoryKey);
		}

		switch (EquipSlot)
		{
		case EEquipmentSlot::ES_MainHandOne:
			if (WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandOne))
			{
				if (InInventoryComponentRef->GetEmptyInventorySpace() >= 2)
				{
					const bool bUnequippedMain = Unequip(EEquipmentSlot::ES_MainHandOne, InInventoryComponentRef, ItemToEquipInventoryKey);
					const bool bUnequippedOff = Unequip(EEquipmentSlot::ES_OffHandOne, InInventoryComponentRef);

					if (bUnequippedMain && bUnequippedOff)
					{
						return Equip(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, no space in inventory for two items."));
					InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
					return false;
				}
			}
			else
			{
				const bool bUnequipSuccessful = Unequip(EquipSlot, InInventoryComponentRef, ItemToEquipInventoryKey);
				if (bUnequipSuccessful)
				{
					return Equip(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, slot is in use and no space in inventory."));
					InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
					return false;
				}
			}
			break;

		case EEquipmentSlot::ES_MainHandTwo:
			if (WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandTwo))
			{
				if (InInventoryComponentRef->GetEmptyInventorySpace() >= 2)
				{
					const bool bUnequippedMain = Unequip(EEquipmentSlot::ES_MainHandTwo, InInventoryComponentRef, ItemToEquipInventoryKey);
					const bool bUnequippedOff = Unequip(EEquipmentSlot::ES_OffHandTwo, InInventoryComponentRef);

					if (bUnequippedMain && bUnequippedOff)
					{
						return Equip(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, no space in inventory for two items."));
					InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
					return false;
				}
			}
			else
			{
				const bool bUnequipSuccessful = Unequip(EquipSlot, InInventoryComponentRef, ItemToEquipInventoryKey);
				if (bUnequipSuccessful)
				{
					return Equip(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, slot is in use and no space in inventory."));
					InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
					return false;
				}
			}
			break;

		default:
			const bool bUnequipSuccessful = Unequip(EquipSlot, InInventoryComponentRef, ItemToEquipInventoryKey);
			if (bUnequipSuccessful)
			{
				return Equip(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, slot is in use and no space in inventory."));
				InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
				return false;
			}
			break;
		}
		// Equip new item and put old item in inventory (same place new item came from)
		

		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, slot is full."));
		return false;
	}

	return false;
}

bool UEquipmentComponent::Unequip(EEquipmentSlot WornEquipmentSlot, UInventoryComponent* InInventoryComponentRef, int InventoryItemKey, bool bDropOnGround)
{
	if (!InInventoryComponentRef)
	{
		if (OwnerInventoryComponentRef)
		{
			InInventoryComponentRef = OwnerInventoryComponentRef;
		}
		else
		{
			return false;
		}
	}

	if (!bDropOnGround)
	{
		if (!InInventoryComponentRef) { return false; }
	}

	// Unequip item and put in inventory, if there is space
	
	// Check that there is an item to unequip
	if (WornEquipmentDataMap.Contains(WornEquipmentSlot))
	{
		UItemData* ItemData = GetWornEquipmentDataInSlot(WornEquipmentSlot);

		AItemEquipment* DroppedItem = nullptr;
		bool bAddedToInventory = false;

		if (bDropOnGround)
		{
			// Spawn item in front of player
			UClass* ItemClass = ItemData->ClassToSpawn.Get();
			const FTransform Transform = FTransform(OwningCharacter->GetActorRotation(), OwningCharacter->GetActorLocation() + (OwningCharacter->GetActorForwardVector() * 100.f), { 1.f, 1.f, 1.f });
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			
			DroppedItem = GetWorld()->SpawnActor<AItemEquipment>(ItemClass, Transform, SpawnInfo);
		}
		else
		{
			//Check that there is space in the inventory to store the unequipped item
			if (InInventoryComponentRef->GetEmptyInventorySpace() <= 0)
			{
				return false;
			}

			if (InInventoryComponentRef->GetInventoryItemDataMap().Contains(InventoryItemKey))
			{
				bAddedToInventory = InInventoryComponentRef->AddItemToInventory(ItemData);
			}
			else
			{
				bAddedToInventory = InInventoryComponentRef->AddItemToInventory(ItemData, InventoryItemKey);
			}
		}

		if (DroppedItem || bAddedToInventory)
		{
			WornEquipmentDataMap.Remove(WornEquipmentSlot);

			if (WornEquipmentActorMap[WornEquipmentSlot]->GetChildActorClass() != nullptr)
			{
				if (OwningCharacter)
				{
					AItemEquipment* ItemToUnequip = Cast<AItemEquipment>(WornEquipmentActorMap[WornEquipmentSlot]->GetChildActor());
					OwningCharacter->MoveIgnoreActorRemove(ItemToUnequip);
				}
				WornEquipmentActorMap[WornEquipmentSlot]->SetChildActorClass(nullptr);
			}
			return true;
		}	
	}
	return false;
}

bool UEquipmentComponent::EquipCheck(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo, UInventoryComponent* InInventoryComponentRef, int ItemToEquipInventoryKey, bool bPopulateEquipCheckDataMap)
{
	UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip called."));

	if (!InInventoryComponentRef)
	{
		if (OwnerInventoryComponentRef)
		{
			InInventoryComponentRef = OwnerInventoryComponentRef;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck InventoryComponentRef is nullptr."));
			return false;
		}
	}

	if (bPopulateEquipCheckDataMap)
	{
		EquipCheckDataMap = WornEquipmentDataMap;
		EmptyInventorySlots = InInventoryComponentRef->GetEmptyInventorySpace();
		if (EquipCheckDataMap.IsEmpty() && !WornEquipmentDataMap.IsEmpty()) { return false; }
	}

	

	// Check that the item is a piece of equipment
	UItemEquipmentData* ItemEquipmentCast = Cast<UItemEquipmentData>(ItemToEquip);
	if (!ItemEquipmentCast)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck ItemToEquip is not equipment."));
		return false;
	}

	// Check that the item can be equipped to an equipment slot
	EEquipmentSlot EquipSlot = EEquipmentSlot::ES_None;
	if (SlotToEquipTo != EEquipmentSlot::ES_None)
	{
		EquipSlot = SlotToEquipTo;
	}
	else
	{
		EquipSlot = GetEquipmentSlotForItem(ItemToEquip);
	}

	if (EquipSlot == EEquipmentSlot::ES_None) { return false; }

	if (!IsEquipSlotValidForItem(EquipSlot, ItemEquipmentCast))
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck SlotToEquipTo is not a valid slot for ItemToEquip."));

		return false;
	}
	

	// Check if character has stats required to equip the item
	if (!OwningCharacterHasAttributesToEquipItem(ItemEquipmentCast))
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Character does not meet the stat requirements to equip the item."));
		return false;
	}

	if (EquipSlot == EEquipmentSlot::ES_OffHandOne)
	{
		UItemEquipmentData* MainHandCheck = nullptr;
		if (EquipCheckDataMap.Contains(EEquipmentSlot::ES_MainHandOne))
		{
			MainHandCheck = Cast<UItemEquipmentData>(EquipCheckDataMap[EEquipmentSlot::ES_MainHandOne]);
		}

		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}

	if (EquipSlot == EEquipmentSlot::ES_OffHandTwo)
	{
		UItemEquipmentData* MainHandCheck = nullptr;
		if (EquipCheckDataMap.Contains(EEquipmentSlot::ES_MainHandOne))
		{
			MainHandCheck = Cast<UItemEquipmentData>(EquipCheckDataMap[EEquipmentSlot::ES_MainHandTwo]);
		}

		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}

	// If equipment slot is empty
	if (!EquipCheckDataMap.Contains(EquipSlot))
	{
		return true;
	}
	// If equipment slot is already in use
	else
	{
		// Unequip the current equipment in slot and equip the new item
		
		// +1 free inventory slot
		EmptyInventorySlots += 1;

		switch (EquipSlot)
		{
		case EEquipmentSlot::ES_MainHandOne:
			if (EquipCheckDataMap.Contains(EEquipmentSlot::ES_OffHandOne))
			{
				if (EmptyInventorySlots >= 2)
				{
					EquipCheckDataMap.Remove(EEquipmentSlot::ES_MainHandOne);
					const bool bUnequippedMain = !EquipCheckDataMap.Contains(EEquipmentSlot::ES_MainHandOne);

					EquipCheckDataMap.Remove(EEquipmentSlot::ES_OffHandOne);
					const bool bUnequippedOff = !EquipCheckDataMap.Contains(EEquipmentSlot::ES_OffHandOne);

					if (bUnequippedMain && bUnequippedOff)
					{
						return EquipCheck(ItemToEquip, SlotToEquipTo, InInventoryComponentRef,ItemToEquipInventoryKey, false);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, no space in inventory for two items."));
					return false;
				}
			}
			else
			{
				EquipCheckDataMap.Remove(EquipSlot);
				const bool bUnequipSuccessful = !EquipCheckDataMap.Contains(EquipSlot);

				if (bUnequipSuccessful)
				{
					return EquipCheck(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, slot is in use and no space in inventory."));
					return false;
				}
			}
			break;

		case EEquipmentSlot::ES_MainHandTwo:
			if (EquipCheckDataMap.Contains(EEquipmentSlot::ES_OffHandTwo))
			{
				if (EmptyInventorySlots >= 2)
				{
					EquipCheckDataMap.Remove(EEquipmentSlot::ES_MainHandTwo);
					const bool bUnequippedMain = !EquipCheckDataMap.Contains(EEquipmentSlot::ES_MainHandTwo);

					EquipCheckDataMap.Remove(EEquipmentSlot::ES_OffHandTwo);
					const bool bUnequippedOff = !EquipCheckDataMap.Contains(EEquipmentSlot::ES_OffHandTwo);

					if (bUnequippedMain && bUnequippedOff)
					{
						return EquipCheck(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, no space in inventory for two items."));
					InInventoryComponentRef->AddItemToInventory(ItemToEquip, ItemToEquipInventoryKey);
					return false;
				}
			}
			else
			{
				EquipCheckDataMap.Remove(EquipSlot);
				const bool bUnequipSuccessful = !EquipCheckDataMap.Contains(EquipSlot);

				if (bUnequipSuccessful)
				{
					return EquipCheck(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, slot is in use and no space in inventory."));
					return false;
				}
			}
			break;

		default:
			EquipCheckDataMap.Remove(EquipSlot);
			const bool bUnequipSuccessful = !EquipCheckDataMap.Contains(EquipSlot);

			if (bUnequipSuccessful)
			{
				return EquipCheck(ItemToEquip, SlotToEquipTo, InInventoryComponentRef, ItemToEquipInventoryKey, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, slot is in use and no space in inventory."));
				return false;
			}
			break;
		}
		// Equip new item and put old item in inventory (same place new item came from)


		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::EquipCheck Equipment cannot be equipped, slot is full."));
		return false;
	}


	return false;
}

bool UEquipmentComponent::RemoveEquipmentInSlot(EEquipmentSlot EquipmentSlot)
{
	if (WornEquipmentDataMap.Contains(EquipmentSlot))
	{
		WornEquipmentDataMap.Remove(EquipmentSlot);
		return true;
	}

	return false;
}

EEquipmentSlot UEquipmentComponent::GetEquipmentSlotForItem(UItemData* Item)
{
	const UItemEquipmentData* ItemEquipmentCast = Cast<UItemEquipmentData>(Item);

	// Find which slot the item needs to be equipped in
	if (ItemEquipmentCast)
	{
		switch (ItemEquipmentCast->EquipmentType)
		{
		case EEquipmentType::EET_None:
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip ItemToEquip does not have a valid EquipmentType."));
			return EEquipmentSlot::ES_None;
			break;

		case EEquipmentType::EET_1HWeapon:
			if (GetCurrentWeaponLoadout() == EWeaponLoadout::WL_LoadoutOne)
			{
				if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandOne))
				{
					return EEquipmentSlot::ES_MainHandOne;
				}
				else if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandOne))
				{
					return EEquipmentSlot::ES_OffHandOne;
				}
				else
				{
					return EEquipmentSlot::ES_MainHandOne;
				}
			}
			else
			{
				if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandTwo))
				{
					return EEquipmentSlot::ES_MainHandTwo;
				}
				else if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandTwo))
				{
					return EEquipmentSlot::ES_OffHandTwo;
				}
				else
				{
					return EEquipmentSlot::ES_MainHandTwo;
				}
			}
			break;

		case EEquipmentType::EET_2HWeapon:
			if (GetCurrentWeaponLoadout() == EWeaponLoadout::WL_LoadoutOne)
			{
				return EEquipmentSlot::ES_MainHandOne;
			}
			else
			{
				return EEquipmentSlot::ES_MainHandTwo;
			}
			break;

		case EEquipmentType::EET_OffHand:
			if (GetCurrentWeaponLoadout() == EWeaponLoadout::WL_LoadoutOne)
			{
				return EEquipmentSlot::ES_OffHandOne;
			}
			else
			{
				return EEquipmentSlot::ES_OffHandTwo;
			}
			break;

		case EEquipmentType::EET_Head:
			return EEquipmentSlot::ES_Head;
			break;

		case EEquipmentType::EET_Torso:
			return EEquipmentSlot::ES_Torso;
			break;

		case EEquipmentType::EET_Hands:
			return EEquipmentSlot::ES_Hands;
			break;

		case EEquipmentType::EET_Legs:
			return EEquipmentSlot::ES_Legs;
			break;

		case EEquipmentType::EET_Feet:
			return EEquipmentSlot::ES_Feet;
			break;

		case EEquipmentType::EET_Torso_Accessory:
			return EEquipmentSlot::ES_TorsoAccessory;
			break;

		case EEquipmentType::EET_Head_Accessory:
			return EEquipmentSlot::ES_HeadAccessory;
			break;

		case EEquipmentType::EET_Neck_Accessory:
			return EEquipmentSlot::ES_NeckAccessory;
			break;

		case EEquipmentType::EET_Arm_Accessory:
			return EEquipmentSlot::ES_ArmAccessory;
			break;

		case EEquipmentType::EET_Ring_Accessory:
			if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_RingAccessory1))
			{
				return EEquipmentSlot::ES_RingAccessory1;
			}
			else if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_RingAccessory2))
			{
				return EEquipmentSlot::ES_RingAccessory2;
			}
			else
			{
				return EEquipmentSlot::ES_RingAccessory1;
			}
			break;

		default:
			return EEquipmentSlot::ES_None;
			break;
		}
	}
	return EEquipmentSlot::ES_None;
}

void UEquipmentComponent::EquipOffHandIfMainHandIsEmpty()
{
	if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandOne) && WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandOne))
	{
		UItemWeaponData* OffHandData = Cast<UItemWeaponData>(WornEquipmentDataMap[EEquipmentSlot::ES_OffHandOne]);
		if (OffHandData->EquipmentType == EEquipmentType::EET_1HWeapon)
		{
			if (Equip(OffHandData, EEquipmentSlot::ES_MainHandOne, nullptr, -1, false))
			{
				WornEquipmentDataMap.Remove(EEquipmentSlot::ES_OffHandOne);
			}
		}
	}
	if (!WornEquipmentDataMap.Contains(EEquipmentSlot::ES_MainHandTwo) && WornEquipmentDataMap.Contains(EEquipmentSlot::ES_OffHandTwo))
	{
		UItemWeaponData* OffHandData = Cast<UItemWeaponData>(WornEquipmentDataMap[EEquipmentSlot::ES_OffHandTwo]);
		if (OffHandData->EquipmentType == EEquipmentType::EET_1HWeapon)
		{
			if (Equip(OffHandData, EEquipmentSlot::ES_MainHandTwo, nullptr, -1, false))
			{
				WornEquipmentDataMap.Remove(EEquipmentSlot::ES_OffHandTwo);
			}
		}
		//WornEquipmentDataMap.Emplace(EEquipmentSlot::ES_MainHandTwo, WornEquipmentDataMap[EEquipmentSlot::ES_OffHandTwo]);
		//WornEquipmentDataMap.Remove(EEquipmentSlot::ES_OffHandTwo);
	}
}

bool UEquipmentComponent::SwapEquipment(EEquipmentSlot FirstEquipment, EEquipmentSlot SecondEquipment)
{
	if (FirstEquipment == SecondEquipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::SwapEquipment FirstEquipment and SecondEquipment are the same slot."));
		return false;
	}

	UItemEquipmentData* FirstItemEquipmentData = Cast<UItemEquipmentData>(WornEquipmentDataMap[FirstEquipment]);
	UItemEquipmentData* SecondItemEquipmentData = Cast<UItemEquipmentData>(WornEquipmentDataMap[SecondEquipment]);

	if (FirstItemEquipmentData && SecondItemEquipmentData)
	{
		if (FirstItemEquipmentData->EquipmentType == SecondItemEquipmentData->EquipmentType)
		{
			UItemData* FirstItemData = WornEquipmentDataMap[FirstEquipment];
			WornEquipmentDataMap[FirstEquipment] = WornEquipmentDataMap[SecondEquipment];
			WornEquipmentDataMap.Emplace(SecondEquipment, FirstItemData);
			return true;
		}
	}

	return false;
}

void UEquipmentComponent::CreateEquipmentChildActors()
{
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_MainHandOne, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_MainHandOne")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_OffHandOne, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_OffHandOne")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_MainHandTwo, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_MainHandTwo")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_OffHandTwo, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_OffHandTwo")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_Head, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Head")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_Torso, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Torso")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_Hands, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Hands")));
	
	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_Legs, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Legs")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_Feet, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Feet")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_TorsoAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_TorsoAccessory")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_HeadAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_HeadAccessory")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_NeckAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_NeckAccessory")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_ArmAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_ArmAccessory")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_RingAccessory1, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_RingAccessory1")));

	WornEquipmentActorMap.Emplace(EEquipmentSlot::ES_RingAccessory2, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_RingAccessory2")));
}

void UEquipmentComponent::UpdateEquipmentChildActors(bool bForceUpdate)
{
	EEquipmentSlot EquipmentSlot;

	for (uint8 i = 1; i < (uint8)EEquipmentSlot::ES_MAX; i++)
	{
		EquipmentSlot = (EEquipmentSlot)i;
		if (WornEquipmentDataMap.Contains(EquipmentSlot))
		{

			if (bForceUpdate || WornEquipmentActorMap[EquipmentSlot]->GetChildActorClass() == nullptr)
			{
				WornEquipmentActorMap[EquipmentSlot]->SetChildActorClass(WornEquipmentDataMap[EquipmentSlot]->ClassToSpawn.Get());

				AItemEquipment* EquippedItem = Cast<AItemEquipment>(WornEquipmentActorMap[EquipmentSlot]->GetChildActor());
				if (EquippedItem)
				{
					// Copy ItemData to child actor
					EquippedItem->SetItemData(WornEquipmentDataMap[EquipmentSlot]);

					// If weapon, attach to appropriate slot
					if (AItemWeapon* EquippedWeapon = Cast<AItemWeapon>(EquippedItem))
					{
						AttachEquipmentToSocket(EquipmentSlot);
					}


					// Change the child actor's collision profile so it does not hit the interaction trace
					EquippedItem->GetItemMesh()->SetCollisionProfileName("EquippedItem");
					if (OwningCharacter)
					{
						OwningCharacter->MoveIgnoreActorAdd(EquippedItem);
						EquippedItem->GetItemMesh()->SetCanEverAffectNavigation(false);
						EquippedItem->GetItemMesh()->SetSimulatePhysics(false);
					}
				}
			}
		}
		else
		{
			if (WornEquipmentActorMap[EquipmentSlot]->GetChildActorClass() != nullptr)
			{
				if (OwningCharacter)
				{
					AItemEquipment* ItemToUnequip = Cast<AItemEquipment>(WornEquipmentActorMap[EquipmentSlot]->GetChildActor());
					OwningCharacter->MoveIgnoreActorRemove(ItemToUnequip);
				}
				WornEquipmentActorMap[EquipmentSlot]->SetChildActorClass(nullptr);
			}
		}
	}
}

void UEquipmentComponent::EquipStartingEquipment()
{
	if (StartingEquipment.IsEmpty()) { return; }

	EEquipmentSlot EquipmentSlot;
	
	for (uint8 i = 1; i < (uint8)EEquipmentSlot::ES_MAX; i++)
	{
		EquipmentSlot = (EEquipmentSlot)i;

		if (StartingEquipment.Contains(EquipmentSlot))
		{
			if (UItemData* ItemData = StartingEquipment[EquipmentSlot].GetDefaultObject()->GetItemDataDefault())
			{
				Equip(ItemData, EquipmentSlot);
			}
		}
	}
}

void UEquipmentComponent::AttachWeaponToSocket(AItemEquipment* WeaponToAttach, FName SocketName)
{
	if (!OwningCharacter) { return; }
	if (!Cast<AItemWeapon>(WeaponToAttach)) { return; }


	if (OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
	{
		WeaponToAttach->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), SocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::AttackWeaponToSocket SocketName is null."));
	}
}

void UEquipmentComponent::AttachEquipmentToMesh(USkeletalMeshComponent* CharacterMesh)
{
	WornEquipmentActorMap[EEquipmentSlot::ES_MainHandOne]->SetupAttachment(CharacterMesh, FName("Equipment_MainHandOne"));

	WornEquipmentActorMap[EEquipmentSlot::ES_OffHandOne]->SetupAttachment(CharacterMesh, FName("Equipment_OffHandOne"));

	WornEquipmentActorMap[EEquipmentSlot::ES_MainHandTwo]->SetupAttachment(CharacterMesh, FName("Equipment_MainHandTwo"));

	WornEquipmentActorMap[EEquipmentSlot::ES_OffHandTwo]->SetupAttachment(CharacterMesh, FName("Equipment_OffHandTwo"));

	WornEquipmentActorMap[EEquipmentSlot::ES_Head]->SetupAttachment(CharacterMesh, FName("Equipment_Head"));

	WornEquipmentActorMap[EEquipmentSlot::ES_Torso]->SetupAttachment(CharacterMesh, FName("Equipment_Torso"));

	WornEquipmentActorMap[EEquipmentSlot::ES_Hands]->SetupAttachment(CharacterMesh, FName("Equipment_Hands"));

	WornEquipmentActorMap[EEquipmentSlot::ES_Legs]->SetupAttachment(CharacterMesh, FName("Equipment_Legs"));

	WornEquipmentActorMap[EEquipmentSlot::ES_Feet]->SetupAttachment(CharacterMesh, FName("Equipment_Feet"));

	WornEquipmentActorMap[EEquipmentSlot::ES_TorsoAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_TorsoAccessory"));

	WornEquipmentActorMap[EEquipmentSlot::ES_HeadAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_HeadAccessory"));

	WornEquipmentActorMap[EEquipmentSlot::ES_NeckAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_NeckAccessory"));

	WornEquipmentActorMap[EEquipmentSlot::ES_ArmAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_ArmAccessory"));

	WornEquipmentActorMap[EEquipmentSlot::ES_RingAccessory1]->SetupAttachment(CharacterMesh, FName("Equipment_RingAccessory1"));

	WornEquipmentActorMap[EEquipmentSlot::ES_RingAccessory2]->SetupAttachment(CharacterMesh, FName("Equipment_RingAccessory2"));
}

void UEquipmentComponent::AttachEquipmentToSocket(EEquipmentSlot EquipmentSlot)
{
	if (!OwningCharacter) { return; }

	if (!WornEquipmentDataMap.Contains(EquipmentSlot)) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AttachEquipmentToSocket No ItemData stored in EquipmentSlot.")); 
		return; 
	}

	FName SocketName;

	if (UCombatComponent* CombatComponentRef = Cast<UCombatComponent>(OwningCharacter->FindComponentByClass(UCombatComponent::StaticClass())))
	{
		// ToDo - Add universal sheath sockets for weapons for both weapon sets
		if (UItemEquipmentData* EquipmentData = Cast<UItemEquipmentData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
		{
			switch (EquipmentSlot)
			{
			case EEquipmentSlot::ES_MainHandOne:
				//SocketName = "Equipment_MainHandOne";
				if (UItemWeaponData* WeaponData = Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
				{
					SocketName = WeaponData->MainhandSheatheSocket;
				}
				break;

			case EEquipmentSlot::ES_OffHandOne:
				//SocketName = "Equipment_OffHandOne";
				if (UItemWeaponData* WeaponData = Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
				{
					SocketName = WeaponData->OffhandSheatheSocket;
				}
				break;

			case EEquipmentSlot::ES_MainHandTwo:
				//SocketName = "Equipment_MainHandTwo";
				if (UItemWeaponData* WeaponData = Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
				{
					SocketName = WeaponData->MainhandSheatheSocket;
				}
				break;

			case EEquipmentSlot::ES_OffHandTwo:
				//SocketName = "Equipment_OffHandTwo";
				if (UItemWeaponData* WeaponData = Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
				{
					SocketName = WeaponData->OffhandSheatheSocket;
				}
				break;

			case EEquipmentSlot::ES_Head:
				SocketName = "Equipment_Head";
				break;

			case EEquipmentSlot::ES_Torso:
				SocketName = "Equipment_Torso";
				break;

			case EEquipmentSlot::ES_Hands:
				SocketName = "Equipment_Hands";
				break;

			case EEquipmentSlot::ES_Legs:
				SocketName = "Equipment_Legs";
				break;

			case EEquipmentSlot::ES_Feet:
				SocketName = "Equipment_Feet";
				break;

			case EEquipmentSlot::ES_TorsoAccessory:
				SocketName = "Equipment_TorsoAccessory";
				break;

			case EEquipmentSlot::ES_HeadAccessory:
				SocketName = "Equipment_HeadAccessory";
				break;

			case EEquipmentSlot::ES_NeckAccessory:
				SocketName = "Equipment_NeckAccessory";
				break;

			case EEquipmentSlot::ES_ArmAccessory:
				SocketName = "Equipment_ArmAccessory";
				break;

			case EEquipmentSlot::ES_RingAccessory1:
				SocketName = "Equipment_RingAccessory1";
				break;

			case EEquipmentSlot::ES_RingAccessory2:
				SocketName = "Equipment_RingAccessory2";
				break;

			default:
				break;
			}
		}
	}

	if (OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
	{
		AItemEquipment* EquippedItem = Cast<AItemEquipment>(GetWornEquipmentActorInSlot(EquipmentSlot)->GetChildActor());

		if (EquippedItem)
		{
			// ToDo - Change scaling EAttachmentRule when I start to use proper equipment models so they scale with the body they are equipped to
			EquippedItem->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), SocketName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AttackWeaponToSocket SocketName is not valid."));
	}
}

EEquipmentSlot UEquipmentComponent::GetEquipmentSlotForCurrentWeaponLoadout(bool bReturnMainhandSlot)
{
	switch (CurrentWeaponLoadout)
	{
	case EWeaponLoadout::WL_LoadoutOne:
		if (bReturnMainhandSlot)
		{
			return EEquipmentSlot::ES_MainHandOne;
		}
		else
		{
			return EEquipmentSlot::ES_OffHandOne;
		}
		break;
	case EWeaponLoadout::WL_LoadoutTwo:
		if (bReturnMainhandSlot)
		{
			return EEquipmentSlot::ES_MainHandTwo;
		}
		else
		{
			return EEquipmentSlot::ES_OffHandTwo;
		}
		break;
	default:
		return EEquipmentSlot::ES_None;
		break;
	}
}

float UEquipmentComponent::GetEquipmentWeight()
{
	float TotalWeight = 0.0f;

	for (uint8 i = 1; i < (uint8)EEquipmentSlot::ES_MAX; i++)
	{
		if (WornEquipmentDataMap.Contains((EEquipmentSlot)i))
		{
			TotalWeight += WornEquipmentDataMap[(EEquipmentSlot)i]->ItemWeight;
		}
	}
	return TotalWeight;
}

AItemWeapon* UEquipmentComponent::GetMainhandWeaponActor()
{
	return Cast<AItemWeapon>(GetWornEquipmentActorInSlot(GetEquipmentSlotForCurrentWeaponLoadout())->GetChildActor());
}

UItemWeaponData* UEquipmentComponent::GetMainhandWeaponData()
{
	return Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(GetEquipmentSlotForCurrentWeaponLoadout()));
}

AItemWeapon* UEquipmentComponent::GetOffhandWeaponActor()
{
	switch (GetEquipmentSlotForCurrentWeaponLoadout())
	{
	case EEquipmentSlot::ES_MainHandOne:
		return Cast<AItemWeapon>(GetWornEquipmentActorInSlot(EEquipmentSlot::ES_OffHandOne)->GetChildActor());
		break;
	case EEquipmentSlot::ES_MainHandTwo:
		return Cast<AItemWeapon>(GetWornEquipmentActorInSlot(EEquipmentSlot::ES_OffHandTwo)->GetChildActor());
		break;
	default:
		break;
	}
	return nullptr;
}

UItemWeaponData* UEquipmentComponent::GetOffhandWeaponData()
{
	switch (GetEquipmentSlotForCurrentWeaponLoadout())
	{
	case EEquipmentSlot::ES_MainHandOne:
		return Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EEquipmentSlot::ES_OffHandOne));
		break;
	case EEquipmentSlot::ES_MainHandTwo:
		return Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EEquipmentSlot::ES_OffHandTwo));
		break;
	default:
		break;
	}
	return nullptr;
}

bool UEquipmentComponent::IsEquipSlotValidForItem(EEquipmentSlot EquipSlot, UItemEquipmentData* Item)
{
	const EEquipmentSlot ValidEquipmentSlot = GetEquipmentSlotForItem(Item);
	// Check that SlotToEquipTo is valid for ItemToEquip
	if (ValidEquipmentSlot == EquipSlot)
	{
		return true;
	}
	else if (EquipSlot != EEquipmentSlot::ES_None && ValidEquipmentSlot != EquipSlot)
	{
		switch (EquipSlot)
		{
		case EEquipmentSlot::ES_MainHandOne:
			if (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo)
			{
				return true;
			}
			break;

		case EEquipmentSlot::ES_OffHandOne:
			if (Item->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo))
			{
				return true;
			}
			break;

		case EEquipmentSlot::ES_MainHandTwo:
			if (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandTwo)
			{
				return true;
			}
			break;

		case EEquipmentSlot::ES_OffHandTwo:
			if (Item->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::ES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::ES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::ES_OffHandOne))
			{
				return true;
			}
			break;

		default:
			break;
		}
	}
	return false;
}

bool UEquipmentComponent::OwningCharacterHasAttributesToEquipItem(UItemEquipmentData* Item)
{
	if (UCharacterStatisticComponent* CharacterStatisticComponent = Cast<UCharacterStatisticComponent>(OwningCharacter->GetComponentByClass(UCharacterStatisticComponent::StaticClass())))
	{
		if (CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Strength)		>= Item->RequiredStrength &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Dexterity)	>= Item->RequiredDexterity &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Vitality)		>= Item->RequiredVitality &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Grit)			>= Item->RequiredGrit &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Intelligence) >= Item->RequiredIntelligence &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Wisdom)		>= Item->RequiredWisdom &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Charisma)		>= Item->RequiredCharisma &&
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Luck)			>= Item->RequiredLuck)
		{
			return true;
		}
	}
	return false;
}
