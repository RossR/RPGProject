// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
// #include "InventoryComponent.h"
#include "RPGProjectPlayerController.h"
// #include "PlayerCharacter.h"
// #include "Weapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Creating the trigger capsule
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	
	TriggerCapsule->SetCapsuleHalfHeight(120.0f);
	TriggerCapsule->SetCapsuleRadius(90.0);
	TriggerCapsule->SetRelativeLocation({ -50.0f, 0.0f, 100.0f });

	// Binding to events
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnOverlapEnd);

	InteractingActor = nullptr;

}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	ARPGProjectPlayerController* PlayerController = Cast<ARPGProjectPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	
	if (PlayerController)
	{
		// Bind to player input
		PlayerController->OnInteractionStart.AddUObject(this, &UInteractionComponent::InteractionStart);
	}
	
}


// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (InteractingActor)
	{
		FVector Offset( 0.0f, 50.0f, 100.0f);
		FVector StartLocation = GetOwner()->GetActorLocation() + Offset;
		DrawDebugString(GetWorld(), Offset, InteractionPrompt.ToString(), GetOwner(), FColor::Blue, 0.0f);
	}
}

void UInteractionComponent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UInteractionComponent::OnOverlapBegin"));

	// InventoryInteractionComponent..make this say press x to equip
	if (OtherActor->ActorHasTag("Player"))
	{
		InteractingActor = OtherActor;
	}
	else if (!OtherActor->ActorHasTag("Player"))
	{
		UE_LOG(LogTemp, Warning, TEXT("UInteractionComponent::OnOverlapBegin Actor is not Player"));
	}
}

void UInteractionComponent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("UInteractionComponent::OnOverlapEnd"));

	InteractingActor = nullptr;
}

// BeginPlay bind
// EndPlay unbind 
// Broadcast
void UInteractionComponent::InteractionStart()
{

}
