// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/EquipmentComponent.h"

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

	AddStartingItems();

	// ...
	
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

}

bool UInventoryComponent::AddItemToInventory(UItemData* Item, int ItemKey)
{
	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory called."));
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
					bool bAdded = InventoryItemDataMap.Contains(i);

					FString BoolString;
					bAdded ? BoolString = TEXT("True") : BoolString = TEXT("False");
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory return %s"), *BoolString);
					
					return bAdded;
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

				// Item was successfully added to inventory
				bool bAdded = InventoryItemDataMap.Contains(ItemKey);

				FString BoolString;
				bAdded ? BoolString = TEXT("True") : BoolString = TEXT("False") ;
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory return %s"), *BoolString);

				return bAdded;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::AddItemToInventory Could not add item as provided index is already in use."));
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
	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveItemFromInventory called."));

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

float UInventoryComponent::GetInventoryWeight()
{
	float InventoryWeight = 0.0f;

	for (int i = 0; i < InventorySize; i++)
	{
		if (InventoryItemDataMap.Contains(i))
		{
			InventoryWeight += InventoryItemDataMap[i]->ItemWeight;
		}
	}

	return InventoryWeight;
}

float UInventoryComponent::GetTotalWeight()
{
	if (UEquipmentComponent* EquipmentComponentRef = Cast<UEquipmentComponent>(GetOwner()->GetComponentByClass(UEquipmentComponent::StaticClass())))
	{
		return (GetInventoryWeight() + ((Currency.Platinum + Currency.Gold + Currency.Silver + Currency.Copper) * .0025f) + EquipmentComponentRef->GetEquipmentWeight());
	}
	else
	{
		return (GetInventoryWeight() + ((Currency.Platinum + Currency.Gold + Currency.Silver + Currency.Copper) * .0025f));
	}
	

	return 0.0f;
}

void UInventoryComponent::AddCurrency(FCurrency CurrencyToAdd, bool bConvertCurrencies)
{
	if (CurrencyToAdd.Platinum > 0) { Currency.Platinum += CurrencyToAdd.Platinum; }
	if (CurrencyToAdd.Gold > 0) { Currency.Gold += CurrencyToAdd.Gold; }
	if (CurrencyToAdd.Silver > 0) { Currency.Silver += CurrencyToAdd.Silver; }
	if (CurrencyToAdd.Copper > 0) { Currency.Copper += CurrencyToAdd.Copper; }

	if (bConvertCurrencies) { ConvertCurrencies(); }
}

bool UInventoryComponent::RemoveCurrency(FCurrency CurrencyToRemove)
{
	// Check that there is enough existing currency to remove
	const int TotalCoinValue = (Currency.Platinum * (CopperToSilverRatio * SilverToGoldRatio * GoldToPlatinumRatio)) + (Currency.Gold * (CopperToSilverRatio * SilverToGoldRatio)) + (Currency.Silver * CopperToSilverRatio) + Currency.Copper;
	const int TotalCost = (CurrencyToRemove.Platinum * (CopperToSilverRatio * SilverToGoldRatio * GoldToPlatinumRatio)) + (CurrencyToRemove.Gold * (CopperToSilverRatio * SilverToGoldRatio)) + (CurrencyToRemove.Silver * CopperToSilverRatio) + CurrencyToRemove.Copper;

	if (TotalCoinValue >= TotalCost)
	{
		float CoinDifference = 0.f;

		// Remove copper currency, if any
		if (CurrencyToRemove.Copper > 0)
		{
			CoinDifference = Currency.Copper - CurrencyToRemove.Copper;

			if (CoinDifference < 0.f)
			{
				int CoinsToConvert = ceilf((FMath::Abs(CoinDifference / CopperToSilverRatio)));

				// Convert silver to copper, if any
				if (Currency.Silver >= CoinsToConvert)
				{
					Currency.Silver -= CoinsToConvert;
					Currency.Copper += CoinsToConvert * CopperToSilverRatio;
				}
				// Else convert gold to copper, if any
				else if (Currency.Gold >= (CoinsToConvert = ceilf((FMath::Abs(CoinDifference / (CopperToSilverRatio * SilverToGoldRatio))))))
				{
					Currency.Gold -= CoinsToConvert;
					Currency.Copper += CoinsToConvert * (CopperToSilverRatio * SilverToGoldRatio);
				}
				// Else convert platinum to copper, if any
				else if (Currency.Platinum >= (CoinsToConvert = ceilf((FMath::Abs(CoinDifference / (CopperToSilverRatio * SilverToGoldRatio * GoldToPlatinumRatio))))))
				{
					Currency.Platinum -= CoinsToConvert;
					Currency.Copper += CoinsToConvert * (CopperToSilverRatio * SilverToGoldRatio * GoldToPlatinumRatio);
				}
				else
				{
					// In theory, this should never be called
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Cannot convert coins to required amount of copper."));
					return false;
				}

				CoinDifference = Currency.Copper - CurrencyToRemove.Copper;
			}

			if (CoinDifference >= 0.f)
			{
				Currency.Copper -= CurrencyToRemove.Copper;
				CoinDifference = 0.f;
				ConvertCurrencies();
			}
			else
			{
				ConvertCurrencies();

				// In theory, this should never be called
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Not enough copper."));
				return false;
			}

			
		}

		// Remove silver currency, if any
		if (CurrencyToRemove.Silver > 0)
		{
			CoinDifference = Currency.Silver - CurrencyToRemove.Silver;

			if (CoinDifference < 0.f)
			{
				int CoinsToConvert = ceilf((FMath::Abs(CoinDifference / SilverToGoldRatio)));

				// Convert gold to silver, if any
				if (Currency.Gold >= CoinsToConvert)
				{
					Currency.Gold -= CoinsToConvert;
					Currency.Silver += CoinsToConvert * SilverToGoldRatio;
				}
				// Else convert platinum to silver, if any
				else if (Currency.Platinum >= (CoinsToConvert = ceilf((FMath::Abs(CoinDifference / (SilverToGoldRatio * GoldToPlatinumRatio))))))
				{
					Currency.Platinum -= CoinsToConvert;
					Currency.Silver += CoinsToConvert * (SilverToGoldRatio * GoldToPlatinumRatio);
				}
				else
				{
					// In theory, this should never be called
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Cannot convert coins to required amount of silver."));
					return false;
				}

				CoinDifference = Currency.Silver - CurrencyToRemove.Silver;
			}

			if (CoinDifference >= 0.f)
			{
				Currency.Silver -= CurrencyToRemove.Silver;
				CoinDifference = 0.f;
				ConvertCurrencies();
			}
			else
			{
				ConvertCurrencies();

				// In theory, this should never be called
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Not enough silver."));
				return false;
			}
		}

		// Remove gold currency, if any
		if (CurrencyToRemove.Gold > 0)
		{
			CoinDifference = Currency.Gold - CurrencyToRemove.Gold;

			if (CoinDifference < 0.f)
			{
				int CoinsToConvert = ceilf((FMath::Abs(CoinDifference / GoldToPlatinumRatio)));

				// Convert platinum to gold, if any
				if (Currency.Gold >= CoinsToConvert)
				{
					Currency.Platinum -= CoinsToConvert;
					Currency.Gold += CoinsToConvert * GoldToPlatinumRatio;
				}
				else
				{
					// In theory, this should never be called
					UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Cannot convert coins to required amount of silver."));
					return false;
				}

				CoinDifference = Currency.Gold - CurrencyToRemove.Gold;
			}

			if (CoinDifference >= 0.f)
			{
				Currency.Gold -= CurrencyToRemove.Gold;
				CoinDifference = 0.f;
				ConvertCurrencies();
			}
			else
			{
				ConvertCurrencies();

				// In theory, this should never be called
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Not enough silver."));
				return false;
			}
		}

		// Remove platinum currency, if any
		if (CurrencyToRemove.Gold > 0)
		{
			CoinDifference = Currency.Platinum - CurrencyToRemove.Platinum;

			if (CoinDifference >= 0.f)
			{
				Currency.Platinum -= CurrencyToRemove.Platinum;
				CoinDifference = 0.f;
				ConvertCurrencies();
			}
			else
			{
				ConvertCurrencies();

				// In theory, this should never be called
				UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency Not enough silver."));
				return false;
			}
		}

		return true;
	}
	

	UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::RemoveCurrency TotalCoinValue is less that the TotalCost."));

	return false;
}

void UInventoryComponent::ConvertCurrencies()
{
	do
	{
		if (Currency.Copper >= CopperToSilverRatio)
		{
			Currency.Silver += 1;
			Currency.Copper -= CopperToSilverRatio;
		}
	} while (Currency.Copper >= CopperToSilverRatio);
	
	do
	{
		if (Currency.Silver >= SilverToGoldRatio)
		{
			Currency.Gold += 1;
			Currency.Silver -= SilverToGoldRatio;
		}
	} while (Currency.Silver >= SilverToGoldRatio);

	do
	{
		if (Currency.Gold >= GoldToPlatinumRatio)
		{
			Currency.Platinum += 1;
			Currency.Gold -= GoldToPlatinumRatio;
		}
	} while (Currency.Gold >= GoldToPlatinumRatio);

	
}

void UInventoryComponent::AddStartingItems()
{
	if (StartingInventoryItems.IsEmpty()) { return; }

	// ToDo - last key instead of num, as num could be smaller then the last key in the map

	for (int i = 0; i < StartingInventoryItems.Num(); i++)
	{
		if (StartingInventoryItems.Contains(i))
		{
			//AItemBase* Item = StartingInventoryItems[i].GetDefaultObject();
			//Item->SetItemDataOnBeginPlay(Item->GetItemDataDefault());
			if (UItemData* ItemData = StartingInventoryItems[i].GetDefaultObject()->GetItemDataDefault())
			{
				AddItemToInventory(ItemData, i);
			}
		}
	}
}
