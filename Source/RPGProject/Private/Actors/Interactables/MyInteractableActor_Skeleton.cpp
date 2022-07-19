// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/MyInteractableActor_Skeleton.h"
#include "Components/CapsuleComponent.h"

AMyInteractableActor_Skeleton::AMyInteractableActor_Skeleton()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	InteractableMesh->DestroyComponent();
	InteractableMesh = NULL;

	InteractableSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("InteractableSkeletonMesh"));
	InteractableSkeletalMesh->SetCollisionProfileName(FName("Interactable"));
	SetRootComponent(InteractableSkeletalMesh);

	InteractionRangeVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	Tags.Add("Interactable");
}

void AMyInteractableActor_Skeleton::BeginPlay()
{
	Super::BeginPlay();
}

void AMyInteractableActor_Skeleton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyInteractableActor_Skeleton::EnableHighlight(bool bActive, int Colour)
{
	if (!bCanHighlight) { return; }
	InteractableSkeletalMesh->SetCustomDepthStencilValue(Colour);
	InteractableSkeletalMesh->SetRenderCustomDepth(bActive);
}

void AMyInteractableActor_Skeleton::InteractionRequested(AActor* InteractingActor)
{
	Super::InteractionRequested(InteractingActor);
}

void AMyInteractableActor_Skeleton::InteractionStart(AActor* InteractingActor)
{
	Super::InteractionStart(InteractingActor);
}

void AMyInteractableActor_Skeleton::ActivateInteractable()
{
	Super::ActivateInteractable();
}

void AMyInteractableActor_Skeleton::DeactivateInteractable()
{
	Super::DeactivateInteractable();
}

void AMyInteractableActor_Skeleton::InteractableActivated()
{
	Super::InteractableActivated();
}

void AMyInteractableActor_Skeleton::InteractableDeactivated()
{
	Super::InteractableDeactivated();
}

bool AMyInteractableActor_Skeleton::GetIsInInteractableRange(AActor* InteractingActor)
{
	return Super::GetIsInInteractableRange(InteractingActor);
}
