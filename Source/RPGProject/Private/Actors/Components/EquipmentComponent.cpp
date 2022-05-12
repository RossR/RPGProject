// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/Components/CharacterStatisticComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsUsingFirstWeaponSet = true;

	CreateEquipmentChildActors();
	UpdateEquipmentChildActors();
}

bool UEquipmentComponent::RemoveWornEquipmentDataInSlot(EEquipmentSlot EquipmentSlot)
{
	if (WornEquipmentData.Contains(EquipmentSlot))
	{
		WornEquipmentData.Remove(EquipmentSlot);
		UpdateEquipmentChildActors();
		return true;
	}

	return false;
}

void UEquipmentComponent::AttachWeaponToSocket(AItemEquipment* WeaponToAttach, FName SocketName)
{

	if (OwningCharacter && OwningCharacter->GetMesh()->DoesSocketExist(SocketName))
	{
		const EEquipmentSlot CurrentMainHandSlot = GetCurrentlyEquippedWeaponSet();
		/*EEquipmentSlot CurrentOffHandSlot;

		if (CurrentMainHandSlot == EEquipmentSlot::EES_MainHandOne)
		{
			CurrentOffHandSlot = EEquipmentSlot::EES_OffHandOne;
		}
		else
		{
			CurrentOffHandSlot = EEquipmentSlot::EES_OffHandTwo;
		}*/

		AItemEquipment* EquippedMainHandItem = Cast<AItemEquipment>(GetWornEquipmentActorInSlot(CurrentMainHandSlot)->GetChildActor());
		//AItemEquipment* EquippedOffHandItem = Cast<AItemEquipment>(EquipmentComponent->GetWornEquipmentActorInSlot(CurrentOffHandSlot)->GetChildActor());

		if (EquippedMainHandItem)
		{
			EquippedMainHandItem->AttachToComponent(OwningCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepRelative, true), SocketName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARPGProjectPlayerCharacter::AttackWeaponToSocket SocketName is null."));
	}
}

// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<ACharacter>(GetOwner());

	EquipStartingEquipment();
	UpdateEquipmentChildActors();
}

// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
}

bool UEquipmentComponent::Equip(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo)
{
	// Check that the item is a piece of equipment
	const UItemEquipmentData* ItemEquipmentCast = Cast<UItemEquipmentData>(ItemToEquip);
	if (!ItemEquipmentCast)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip ItemToEquip is not equipment."));
		return false;
	}

	// Check that the item can be equipped to an equipment slot
	EEquipmentSlot EquipSlot;

	SlotToEquipTo != EEquipmentSlot::EES_None ? EquipSlot = SlotToEquipTo : EquipSlot = GetEquipmentSlotForItem(ItemToEquip);

	if (EquipSlot == EEquipmentSlot::EES_None)
	{
		return false;
	}

	const EEquipmentSlot ValidEquipmentSlot = GetEquipmentSlotForItem(ItemToEquip);
	// Check that SlotToEquipTo is valid for ItemToEquip
	if (SlotToEquipTo != EEquipmentSlot::EES_None && ValidEquipmentSlot != SlotToEquipTo)
	{
		bool IsValidSlot = false;

		switch (SlotToEquipTo)
		{
		case EEquipmentSlot::EES_MainHandOne:
			if (ValidEquipmentSlot == EEquipmentSlot::EES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandTwo)
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::EES_OffHandOne:
			if (ItemEquipmentCast->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::EES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::EES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandTwo))
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::EES_MainHandTwo:
			if (ValidEquipmentSlot == EEquipmentSlot::EES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandOne || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandTwo)
			{
				IsValidSlot = true;
			}
			break;

		case EEquipmentSlot::EES_OffHandTwo:
			if (ItemEquipmentCast->EquipmentType != EEquipmentType::EET_2HWeapon && (ValidEquipmentSlot == EEquipmentSlot::EES_MainHandOne || ValidEquipmentSlot == EEquipmentSlot::EES_MainHandTwo || ValidEquipmentSlot == EEquipmentSlot::EES_OffHandOne))
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

	if (EquipSlot == EEquipmentSlot::EES_OffHandOne)
	{
		UItemEquipmentData* MainHandCheck = Cast<UItemEquipmentData>(WornEquipmentData[EEquipmentSlot::EES_MainHandOne]);
		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}

	if (EquipSlot == EEquipmentSlot::EES_OffHandTwo)
	{
		UItemEquipmentData* MainHandCheck = Cast<UItemEquipmentData>(WornEquipmentData[EEquipmentSlot::EES_MainHandTwo]);
		if (MainHandCheck)
		{
			if (MainHandCheck->EquipmentType == EEquipmentType::EET_2HWeapon)
			{
				UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Mainhand is currently holding a two-handed weapon, cannot equip."));
				return false;
			}
		}
	}
	
	// Check if character has stats required to equip the item
	if (UCharacterStatisticComponent* CharacterStatisticComponent = Cast<UCharacterStatisticComponent>(OwningCharacter->GetComponentByClass(UCharacterStatisticComponent::StaticClass())))
	{
		if (CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Strength)		< ItemEquipmentCast->RequiredStrength		||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Dexterity)	< ItemEquipmentCast->RequiredDexterity		||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Vitality)		< ItemEquipmentCast->RequiredVitality		||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Grit)			< ItemEquipmentCast->RequiredGrit			||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Intelligence) < ItemEquipmentCast->RequiredIntelligence	||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Wisdom)		< ItemEquipmentCast->RequiredWisdom			||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Charisma)		< ItemEquipmentCast->RequiredCharisma		||
			CharacterStatisticComponent->GetAttribute(EAttributeType::EAT_Luck)			< ItemEquipmentCast->RequiredLuck)
		{
			UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Character does not meet the stat requirements to equip the item."));
			return false;
		}
	}
	
	// If equipment slot is empty
	if (!WornEquipmentData.Contains(EquipSlot))
	{
		WornEquipmentData.Emplace(EquipSlot, ItemToEquip);

		if (WornEquipmentActors[EquipSlot]->GetChildActorClass() == nullptr)
		{
			WornEquipmentActors[EquipSlot]->SetChildActorClass(WornEquipmentData[EquipSlot]->ClassToSpawn.Get());

			AItemEquipment* EquippedItem = Cast<AItemEquipment>(WornEquipmentActors[EquipSlot]->GetChildActor());
			if (EquippedItem)
			{
				// Copy ItemData to child actor
				EquippedItem->SetItemData(WornEquipmentData[EquipSlot]);

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

		return true;
	}
	// If equipment slot is already in use
	else
	{
		// Equip new item and put old item in inventory (same place new item came from)

		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::Equip Equipment cannot be equipped, slot is full."));
		return false;
	}

	return false;
}

bool UEquipmentComponent::Unequip(EEquipmentSlot WornEquipmentSlot)
{
	// Unequip item and put in inventory, if there is space
	if (WornEquipmentData.Contains(WornEquipmentSlot))
	{
		WornEquipmentData.Remove(WornEquipmentSlot);

		if (WornEquipmentActors[WornEquipmentSlot]->GetChildActorClass() != nullptr)
		{
			if (OwningCharacter)
			{
				AItemEquipment* ItemToUnequip = Cast<AItemEquipment>(WornEquipmentActors[WornEquipmentSlot]->GetChildActor());
				OwningCharacter->MoveIgnoreActorRemove(ItemToUnequip);
			}
			WornEquipmentActors[WornEquipmentSlot]->SetChildActorClass(nullptr);
		}
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
			return EEquipmentSlot::EES_None;
			break;

		case EEquipmentType::EET_1HWeapon:
			if (bIsUsingFirstWeaponSet)
			{
				if (!WornEquipmentData.Contains(EEquipmentSlot::EES_MainHandOne))
				{
					return EEquipmentSlot::EES_MainHandOne;
				}
				else if (!WornEquipmentData.Contains(EEquipmentSlot::EES_OffHandOne))
				{
					return EEquipmentSlot::EES_OffHandOne;
				}
				else
				{
					return EEquipmentSlot::EES_MainHandOne;
				}
			}
			else
			{
				if (!WornEquipmentData.Contains(EEquipmentSlot::EES_MainHandTwo))
				{
					return EEquipmentSlot::EES_MainHandTwo;
				}
				else if (!WornEquipmentData.Contains(EEquipmentSlot::EES_OffHandTwo))
				{
					return EEquipmentSlot::EES_OffHandTwo;
				}
				else
				{
					return EEquipmentSlot::EES_MainHandTwo;
				}
			}
			break;

		case EEquipmentType::EET_2HWeapon:
			if (bIsUsingFirstWeaponSet)
			{
				return EEquipmentSlot::EES_MainHandOne;
			}
			else
			{
				return EEquipmentSlot::EES_MainHandTwo;
			}
			break;

		case EEquipmentType::EET_OffHand:
			if (bIsUsingFirstWeaponSet)
			{
				return EEquipmentSlot::EES_OffHandOne;
			}
			else
			{
				return EEquipmentSlot::EES_OffHandTwo;
			}
			break;

		case EEquipmentType::EET_Head:
			return EEquipmentSlot::EES_Head;
			break;

		case EEquipmentType::EET_Torso:
			return EEquipmentSlot::EES_Torso;
			break;

		case EEquipmentType::EET_Hands:
			return EEquipmentSlot::EES_Hands;
			break;

		case EEquipmentType::EET_Legs:
			return EEquipmentSlot::EES_Legs;
			break;

		case EEquipmentType::EET_Feet:
			return EEquipmentSlot::EES_Feet;
			break;

		case EEquipmentType::EET_TorsoAccessory:
			return EEquipmentSlot::EES_TorsoAccessory;
			break;

		case EEquipmentType::EET_HeadAccessory:
			return EEquipmentSlot::EES_HeadAccessory;
			break;

		case EEquipmentType::EET_NeckAccessory:
			return EEquipmentSlot::EES_NeckAccessory;
			break;

		case EEquipmentType::EET_ArmAccessory:
			return EEquipmentSlot::EES_ArmAccessory;
			break;

		case EEquipmentType::EET_RingAccessory:
			if (!WornEquipmentData.Contains(EEquipmentSlot::EES_RingAccessory1))
			{
				return EEquipmentSlot::EES_RingAccessory1;
			}
			else if (!WornEquipmentData.Contains(EEquipmentSlot::EES_RingAccessory2))
			{
				return EEquipmentSlot::EES_RingAccessory2;
			}
			else
			{
				return EEquipmentSlot::EES_RingAccessory1;
			}
			break;

		default:
			return EEquipmentSlot::EES_None;
			break;
		}
	}
	return EEquipmentSlot::EES_None;
}

void UEquipmentComponent::EquipOffHandIfMainHandIsEmpty()
{
	if (!WornEquipmentData.Contains(EEquipmentSlot::EES_MainHandOne) && WornEquipmentData.Contains(EEquipmentSlot::EES_OffHandOne))
	{
		UItemData* OffHandData = Cast<UItemData>(WornEquipmentData[EEquipmentSlot::EES_OffHandOne]);
		Equip(OffHandData);
		WornEquipmentData.Remove(EEquipmentSlot::EES_OffHandOne);
	}
	if (!WornEquipmentData.Contains(EEquipmentSlot::EES_MainHandTwo) && WornEquipmentData.Contains(EEquipmentSlot::EES_OffHandTwo))
	{
		WornEquipmentData.Emplace(EEquipmentSlot::EES_MainHandTwo, WornEquipmentData[EEquipmentSlot::EES_OffHandTwo]);
		WornEquipmentData.Remove(EEquipmentSlot::EES_OffHandTwo);
	}
}

bool UEquipmentComponent::SwapEquipment(EEquipmentSlot FirstEquipment, EEquipmentSlot SecondEquipment)
{
	if (FirstEquipment == SecondEquipment)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::SwapEquipment FirstEquipment and SecondEquipment are the same slot."));
		return false;
	}

	UItemEquipmentData* FirstItemEquipmentData = Cast<UItemEquipmentData>(WornEquipmentData[FirstEquipment]);
	UItemEquipmentData* SecondItemEquipmentData = Cast<UItemEquipmentData>(WornEquipmentData[SecondEquipment]);

	if (FirstItemEquipmentData && SecondItemEquipmentData)
	{
		if (FirstItemEquipmentData->EquipmentType == SecondItemEquipmentData->EquipmentType)
		{
			UItemData* FirstItemData = WornEquipmentData[FirstEquipment];
			WornEquipmentData[FirstEquipment] = WornEquipmentData[SecondEquipment];
			WornEquipmentData.Emplace(SecondEquipment, FirstItemData);
			return true;
		}
	}

	return false;
}

void UEquipmentComponent::CreateEquipmentChildActors()
{
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_MainHandOne, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_MainHandOne")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_OffHandOne, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_OffHandOne")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_MainHandTwo, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_MainHandTwo")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_OffHandTwo, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_OffHandTwo")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_Head, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Head")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_Torso, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Torso")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_Hands, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Hands")));
	
	WornEquipmentActors.Emplace(EEquipmentSlot::EES_Legs, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Legs")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_Feet, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_Feet")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_TorsoAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_TorsoAccessory")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_HeadAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_HeadAccessory")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_NeckAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_NeckAccessory")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_ArmAccessory, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_ArmAccessory")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_RingAccessory1, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_RingAccessory1")));

	WornEquipmentActors.Emplace(EEquipmentSlot::EES_RingAccessory2, CreateDefaultSubobject<UChildActorComponent>(TEXT("Equipped_RingAccessory2")));
}

void UEquipmentComponent::UpdateEquipmentChildActors()
{
	EEquipmentSlot EquipmentSlot;

	for (uint8 i = 1; i < (uint8)EEquipmentSlot::EES_MAX; i++)
	{
		EquipmentSlot = (EEquipmentSlot)i;
		if (WornEquipmentData.Contains(EquipmentSlot))
		{
			if (WornEquipmentActors[EquipmentSlot]->GetChildActorClass() == nullptr)
			{
				WornEquipmentActors[EquipmentSlot]->SetChildActorClass(WornEquipmentData[EquipmentSlot]->ClassToSpawn.Get());

				AItemEquipment* EquippedItem = Cast<AItemEquipment>(WornEquipmentActors[EquipmentSlot]->GetChildActor());
				if (EquippedItem)
				{
					// Copy ItemData to child actor
					EquippedItem->SetItemData(WornEquipmentData[EquipmentSlot]);

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
			if (WornEquipmentActors[EquipmentSlot]->GetChildActorClass() != nullptr)
			{
				if (OwningCharacter)
				{
					AItemEquipment* ItemToUnequip = Cast<AItemEquipment>(WornEquipmentActors[EquipmentSlot]->GetChildActor());
					OwningCharacter->MoveIgnoreActorRemove(ItemToUnequip);
				}
				WornEquipmentActors[EquipmentSlot]->SetChildActorClass(nullptr);
			}
		}
	}
}

void UEquipmentComponent::EquipStartingEquipment()
{
	if (StartingEquipment.IsEmpty()) { return; }

	EEquipmentSlot EquipmentSlot;
	
	for (uint8 i = 1; i < (uint8)EEquipmentSlot::EES_MAX; i++)
	{
		EquipmentSlot = (EEquipmentSlot)i;

		if (StartingEquipment.Contains(EquipmentSlot))
		{
			if (UItemData* ItemData = StartingEquipment[EquipmentSlot].GetDefaultObject()->GetItemData())
			{
				Equip(ItemData, EquipmentSlot);
			}
		}
	}
}

void UEquipmentComponent::AttachEquipmentToMesh(USkeletalMeshComponent* CharacterMesh)
{
	WornEquipmentActors[EEquipmentSlot::EES_MainHandOne]->SetupAttachment(CharacterMesh, FName("Equipment_MainHandOne"));

	WornEquipmentActors[EEquipmentSlot::EES_OffHandOne]->SetupAttachment(CharacterMesh, FName("Equipment_OffHandOne"));

	WornEquipmentActors[EEquipmentSlot::EES_MainHandTwo]->SetupAttachment(CharacterMesh, FName("Equipment_MainHandTwo"));

	WornEquipmentActors[EEquipmentSlot::EES_OffHandTwo]->SetupAttachment(CharacterMesh, FName("Equipment_OffHandTwo"));

	WornEquipmentActors[EEquipmentSlot::EES_Head]->SetupAttachment(CharacterMesh, FName("Equipment_Head"));

	WornEquipmentActors[EEquipmentSlot::EES_Torso]->SetupAttachment(CharacterMesh, FName("Equipment_Torso"));

	WornEquipmentActors[EEquipmentSlot::EES_Hands]->SetupAttachment(CharacterMesh, FName("Equipment_Hands"));

	WornEquipmentActors[EEquipmentSlot::EES_Legs]->SetupAttachment(CharacterMesh, FName("Equipment_Legs"));

	WornEquipmentActors[EEquipmentSlot::EES_Feet]->SetupAttachment(CharacterMesh, FName("Equipment_Feet"));

	WornEquipmentActors[EEquipmentSlot::EES_TorsoAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_TorsoAccessory"));

	WornEquipmentActors[EEquipmentSlot::EES_HeadAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_HeadAccessory"));

	WornEquipmentActors[EEquipmentSlot::EES_NeckAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_NeckAccessory"));

	WornEquipmentActors[EEquipmentSlot::EES_ArmAccessory]->SetupAttachment(CharacterMesh, FName("Equipment_ArmAccessory"));

	WornEquipmentActors[EEquipmentSlot::EES_RingAccessory1]->SetupAttachment(CharacterMesh, FName("Equipment_RingAccessory1"));

	WornEquipmentActors[EEquipmentSlot::EES_RingAccessory2]->SetupAttachment(CharacterMesh, FName("Equipment_RingAccessory2"));
}

void UEquipmentComponent::AttachEquipmentToSocket(EEquipmentSlot EquipmentSlot)
{
	if (!OwningCharacter) { return; }

	if (!WornEquipmentData.Contains(EquipmentSlot)) 
	{ 
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentComponent::AttachEquipmentToSocket No ItemData stored in EquipmentSlot.")); 
		return; 
	}

	FName SocketName;

	if (UCombatComponent* CombatComponentRef = Cast<UCombatComponent>(OwningCharacter->FindComponentByClass(UCombatComponent::StaticClass())))
	{
		if (UItemWeaponData* WeaponData = Cast<UItemWeaponData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
		{
			switch (CombatComponentRef->GetCombatState())
			{
			case ECombatState::CS_AtEase:
				SocketName = WeaponData->SheathSocket;
				break;

			case ECombatState::CS_CombatReady:
				SocketName = WeaponData->EquippedSocket;
				break;

			default:
				break;
			}
		}
		// ToDo - Add universal sheath sockets for weapons for both weapon sets
		else if (UItemEquipmentData* EquipmentData = Cast<UItemEquipmentData>(GetWornEquipmentDataInSlot(EquipmentSlot)))
		{
			switch (EquipmentSlot)
			{
			/*case EEquipmentSlot::EES_MainHandOne:
				SocketName = "Equipment_MainHandOne";
				break;

			case EEquipmentSlot::EES_OffHandOne:
				SocketName = "Equipment_OffHandOne";
				break;

			case EEquipmentSlot::EES_MainHandTwo:
				SocketName = "Equipment_MainHandTwo";
				break;

			case EEquipmentSlot::EES_OffHandTwo:
				SocketName = "Equipment_OffHandTwo";
				break;*/

			case EEquipmentSlot::EES_Head:
				SocketName = "Equipment_Head";
				break;

			case EEquipmentSlot::EES_Torso:
				SocketName = "Equipment_Torso";
				break;

			case EEquipmentSlot::EES_Hands:
				SocketName = "Equipment_Hands";
				break;

			case EEquipmentSlot::EES_Legs:
				SocketName = "Equipment_Legs";
				break;

			case EEquipmentSlot::EES_Feet:
				SocketName = "Equipment_Feet";
				break;

			case EEquipmentSlot::EES_TorsoAccessory:
				SocketName = "Equipment_TorsoAccessory";
				break;

			case EEquipmentSlot::EES_HeadAccessory:
				SocketName = "Equipment_HeadAccessory";
				break;

			case EEquipmentSlot::EES_NeckAccessory:
				SocketName = "Equipment_NeckAccessory";
				break;

			case EEquipmentSlot::EES_ArmAccessory:
				SocketName = "Equipment_ArmAccessory";
				break;

			case EEquipmentSlot::EES_RingAccessory1:
				SocketName = "Equipment_RingAccessory1";
				break;

			case EEquipmentSlot::EES_RingAccessory2:
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

EEquipmentSlot UEquipmentComponent::GetCurrentlyEquippedWeaponSet()
{
	if (bIsUsingFirstWeaponSet)
	{
		return EEquipmentSlot::EES_MainHandOne;
	}
	else
	{
		return EEquipmentSlot::EES_MainHandTwo;
	}
	return EEquipmentSlot::EES_None;
}