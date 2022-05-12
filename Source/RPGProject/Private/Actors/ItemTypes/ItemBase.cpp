// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/Components/InventoryComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Item Mesh"));

	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionProfileName("Item");
	ItemMesh->SetSimulatePhysics(true);

	ItemData = CreateDefaultSubobject<UItemData>(TEXT("Item Data"));

	Tags.Add("Interactable");
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	if (ItemDataOverride) { ItemData = ItemDataOverride; }

	if (ItemData->bUseItemWeightForPhysics)
	{
		ItemMesh->SetMassOverrideInKg(NAME_None, ItemData->ItemWeight, true);
	}
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AItemBase::EnableHighlight(bool bActive, int Colour)
{
	ItemMesh->SetRenderCustomDepth(bActive);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt(Colour));

	if (Colour == -1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt((int)ItemData->ItemRarity));
		ItemMesh->SetCustomDepthStencilValue((int)ItemData->ItemRarity);
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

void AItemBase::InteractionStart(AActor* InteractingActor)
{
	if (UInventoryComponent* InventoryComponentRef = Cast<UInventoryComponent>(InteractingActor->GetComponentByClass(UInventoryComponent::StaticClass())))
	{
		if (InventoryComponentRef->AddItemToInventory(ItemData))
		{
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

void AItemBase::InteractableActivated()
{

}

void AItemBase::InteractableDeactivated()
{

}

bool AItemBase::GetIsInInteractableRange(AActor* InteractingActor)
{
	return true;
}
