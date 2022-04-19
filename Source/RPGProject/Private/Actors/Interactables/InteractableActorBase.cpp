// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActorBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

// Sets default values
AInteractableActorBase::AInteractableActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	SetRootComponent(InteractableMesh);

	InteractionRangeVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionRange"));
	InteractionRangeVolume->AttachToComponent(InteractableMesh, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	InteractionRangeVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActorBase::OnOverlapBegin);
	InteractionRangeVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractableActorBase::OnOverlapEnd);

	Tags.Add("Interactable");
}

// Called when the game starts or when spawned
void AInteractableActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableActorBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* CharacterCast = Cast<ACharacter>(OtherActor))
	{
		ActorsInInteractionRange.AddUnique(OtherActor);
	}
}

void AInteractableActorBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* CharacterCast = Cast<ACharacter>(OtherActor))
	{
		ActorsInInteractionRange.RemoveSingle(OtherActor);
	}
}

void AInteractableActorBase::EnableHighlight(bool bActive, int Colour)
{
	InteractableMesh->SetCustomDepthStencilValue(Colour);
	InteractableMesh->SetRenderCustomDepth(bActive);
}

void AInteractableActorBase::InteractionStart(AActor* InteractingActor)
{
	if (!ActorsInInteractionRange.Contains(InteractingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractableActorBase::InteractionStart InteractingActor is not in interaction range."));
		return;
	}
}

