// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActor_Skeleton.h"
#include "Components/CapsuleComponent.h"

AInteractableActor_Skeleton::AInteractableActor_Skeleton()
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

void AInteractableActor_Skeleton::BeginPlay()
{
	Super::BeginPlay();
}

void AInteractableActor_Skeleton::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractableActor_Skeleton::EnableHighlight(bool bActive, int Colour)
{
	if (!bCanHighlight) { return; }
	InteractableSkeletalMesh->SetCustomDepthStencilValue(Colour);
	InteractableSkeletalMesh->SetRenderCustomDepth(bActive);
}

void AInteractableActor_Skeleton::InteractionRequested(AActor* InteractingActor)
{
	Super::InteractionRequested(InteractingActor);
}

void AInteractableActor_Skeleton::StartInteraction(AActor* InteractingActor)
{
	Super::StartInteraction(InteractingActor);
}

void AInteractableActor_Skeleton::ActivateInteractable()
{
	Super::ActivateInteractable();
}

void AInteractableActor_Skeleton::DeactivateInteractable()
{
	Super::DeactivateInteractable();
}

void AInteractableActor_Skeleton::InteractableHasActivated()
{
	Super::InteractableHasActivated();
}

void AInteractableActor_Skeleton::InteractableHasDeactivated()
{
	Super::InteractableHasDeactivated();
}

bool AInteractableActor_Skeleton::GetIsInInteractableRange(AActor* InteractingActor) const
{
	return Super::GetIsInInteractableRange(InteractingActor);
}
