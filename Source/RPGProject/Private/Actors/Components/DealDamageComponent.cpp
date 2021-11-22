// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/DealDamageComponent.h"
#include "Components/CapsuleComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "GameFramework/DamageType.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UDealDamageComponent::UDealDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapBegin);
	TriggerCapsule->OnComponentEndOverlap.AddDynamic(this, &UDealDamageComponent::OnOverlapEnd);

	bWasActiveLastTick = false;
	
}


// Called when the game starts
void UDealDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	
	// ...
	
}

void UDealDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	

	//If the component is disabled, end the overlap with all currently overlapping actors
	/*
	if (!bActive)
	{

		if (bWasActiveLastTick)
		{
			// int ArraySize = OverlappedComponentArray.Num();
			for (int i = 0; i < OverlappedComponentArray.Num(); i++)
			{
				UE_LOG(LogTemp, Warning, TEXT("Ending Overlap with overlapped actors"));
				FOverlapInfo DealDamageOverlapInfo(OverlappedComponentArray[i]);
				TriggerCapsule->EndComponentOverlap(DealDamageOverlapInfo, true, false);
				// OverlappedComponentArray.RemoveSingle(OverlappedComponentArray[i]);
			}
			OverlappedComponentArray.Empty();

			// TriggerCapsule->SetGenerateOverlapEvents(false);

			bWasActiveLastTick = false;
		}
		
	}
	*/
	
	if (bActive)
	{
		//if (!bWasActiveLastTick)
		//{
		//	bWasActiveLastTick = true;
		//}

		//Renew fire on all overlapping ARPGProjectPlayerCharacter actors
		for (int i = 0; i < OverlappedActorArray.Num(); i++)
		{
			PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(OverlappedActorArray[i]);
			if (PlayerCharacter)
			{
				//TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
				// FDamageEvent DamageEvent(ValidDamageTypeClass);

				PlayerCharacter->SetOnFire(BaseDamage, DamageTotalTime, TakeDamageInterval);

			}
		}
	}
}


void UDealDamageComponent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UDealDamageComponent::OnOverlapBegin"));
	if (OtherActor == GetOwner())
	{
		return;
	}

	//if (!bActive)
	//{
	//	return;
	//}

	OtherComponent = OtherComp;

	// Add the overlapped actor into the array
	OverlappedActorArray.Emplace(OtherActor);

	// OverlappedComponentArray.Emplace(OtherComp);

	//ARPGProjectPlayerCharacter* 
	/*
	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		//TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		// FDamageEvent DamageEvent(ValidDamageTypeClass);

		PlayerCharacter->SetOnFire(BaseDamage, DamageTotalTime, TakeDamageInterval);

	}
	*/
}

void UDealDamageComponent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("UDealDamageComponent::OnOverlapEnd"));
	
	// int32 ActorIndex = OverlappedActorArray.Find(OtherActor);
	// OverlappedComponentArray.RemoveAt(ActorIndex);
	OverlappedActorArray.RemoveSingle(OtherActor);

	PlayerCharacter = nullptr;
}