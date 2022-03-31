// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/InteractableDoor.h"
#include "Actors/Components/DoorInteractionComponent.h"
#include "Components/CapsuleComponent.h"

AInteractableDoor::AInteractableDoor() : Super()
{
	DoorInteractionComponent = CreateDefaultSubobject<UDoorInteractionComponent>(TEXT("DoorInteractionComponent"));
	
	if (DoorInteractionComponent->GetTriggerCapsule())
	{
		DoorInteractionComponent->GetTriggerCapsule()->SetupAttachment(RootComponent);
	}

	Tags.Add("Interactable");
}

void AInteractableDoor::BeginPlay()
{
	Super::BeginPlay();

	DoorInteractionComponent->InteractionSuccess.AddDynamic(this, &AInteractableDoor::OnInteractionSuccess);
}

void AInteractableDoor::OpenDoor()
{
	DoorInteractionComponent->OpenDoor();
}

void AInteractableDoor::EnableHighlight(bool bActive, int Colour)
{
	UStaticMeshComponent* DoorMesh = Cast<UStaticMeshComponent>(GetRootComponent());
	DoorMesh->SetRenderCustomDepth(bActive);

	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt(Colour));

	if (Colour == -1)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::FromInt((int)ItemData->ItemRarity));
		DoorMesh->SetCustomDepthStencilValue(-1);
	}
	else
	{
		DoorMesh->SetCustomDepthStencilValue(Colour);
	}

}

void AInteractableDoor::OnInteractionSuccess()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableDoor::OnInteractionSuccess called"));
	OnDoorOpen.Broadcast();
}