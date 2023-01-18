// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/Components/InventoryComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));

	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionProfileName("Item");
	ItemMesh->SetSimulatePhysics(true);

	ItemDataDefault = CreateDefaultSubobject<UItemData>(TEXT("Item Data Default"));
	//ItemData = CreateDefaultSubobject<UItemData>(TEXT("Item Data"));

	Tags.Add("Interactable");
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDataOverride) { SetItemData(ItemDataOverride); }
	else { SetItemData(ItemDataDefault); }

	if (GetItemData()->bUseItemWeightForPhysics)
	{
		ItemMesh->SetMassOverrideInKg(NAME_None, GetItemData()->ItemWeight, true);
	}

	if (ACharacter* OwningCharacterRef = Cast<ACharacter>(GetParentActor()))
	{
		OwnerSkeletalMeshComponentRef = OwningCharacterRef->GetMesh();

		if (OwnerSkeletalMeshComponentRef) { OwnerAnimInstanceRef = Cast<URPGProjectAnimInstance>(OwnerSkeletalMeshComponentRef->GetAnimInstance()); }
	}
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AItemBase::SetItemData(UItemData* NewItemData)
{
	if (NewItemData)
	{
		ItemData = NULL;
		ItemData = NewObject<UItemData>(this, UItemData::StaticClass(), TEXT("Item Data"), EObjectFlags::RF_NoFlags, NewItemData);
	}
}

void AItemBase::EnableHighlight(bool bActive, int Colour)
{
	ItemMesh->SetRenderCustomDepth(bActive);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt(Colour));

	if (Colour == -1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt((int)ItemData->ItemRarity));
		ItemMesh->SetCustomDepthStencilValue((int)GetItemData()->ItemRarity);
	}
	else
	{
		ItemMesh->SetCustomDepthStencilValue(Colour);
	}

}

void AItemBase::InteractionRequested(AActor* InteractingActor)
{
	ARPGProjectPlayerCharacter* PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(InteractingActor);
	if (PlayerCharacter)
	{
		AActor* InteractableActor = this;
		PlayerCharacter->InteractionStarted(InteractableActor);
	}

	
}

void AItemBase::StartInteraction(AActor* InteractingActor)
{
	if (UInventoryComponent* InventoryComponentRef = Cast<UInventoryComponent>(InteractingActor->GetComponentByClass(UInventoryComponent::StaticClass())))
	{
		if (InventoryComponentRef->AddItemToInventory(GetItemData()))
		{
			// Create notification on HUD
			ItemPickupEvent();

			Destroy();
		}
	}
}

void AItemBase::ActivateInteractable()
{

}

void AItemBase::DeactivateInteractable()
{

}

void AItemBase::InteractableHasActivated()
{

}

void AItemBase::InteractableHasDeactivated()
{

}

bool AItemBase::GetIsInInteractableRange(AActor* InteractingActor)
{
	return true;
}

void AItemBase::SetDurability(float NewDurability)
{
	GetItemData()->ItemCurrentDurability = NewDurability;
	GetItemData()->ItemCurrentDurability = UKismetMathLibrary::FClamp(GetItemData()->ItemCurrentDurability, 0.f, GetItemData()->ItemMaxDurability);
}

void AItemBase::UpdateCurves()
{
	if (!OwnerAnimInstanceRef) { return; }


}
