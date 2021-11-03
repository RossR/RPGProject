// Fill out your copyright notice in the Description page of Project Settings.


#include "DealDamageComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCharacter.h"
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

	if (!bActive && GetWorld()->GetTimerManager().IsTimerActive(DamageOverTime))
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageOverTime);
		// TriggerCapsule->Deactivate();
		// FOverlapInfo DealDamageOverlapInfo (SweepResults);
		// TriggerCapsule->EndComponentOverlap(DealDamageOverlapInfo, true, false);
	}
	
	if (bActive && !TriggerCapsule->IsActive())
	{
		//TriggerCapsule->Activate();
	}
}


void UDealDamageComponent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("UDealDamageComponent::OnOverlapBegin"));
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (!bActive)
	{
		return;
	}

	//APlayerCharacter* 
	PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		/*TSubclassOf<UDamageType> const */ ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		// FDamageEvent DamageEvent(ValidDamageTypeClass);



		if (bIsDamageOverTime)
		{
			
			if (!GetWorld()->GetTimerManager().IsTimerActive(DamageOverTime))
			{
				DealDamage();
			}
			GetWorld()->GetTimerManager().SetTimer(DamageOverTime, [&]() {this->DealDamage(/*DamageEvent, PlayerCharacter*/); }, SecondsBetweenDamage, true, -1.0f);
		}
		else
		{
			DealDamage(/*DamageEvent, PlayerCharacter*/ );
			
		}
	}
}

void UDealDamageComponent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("UDealDamageComponent::OnOverlapEnd"));
	if (bIsDamageOverTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageOverTime);
	}
	PlayerCharacter = nullptr;
}

void UDealDamageComponent::DealDamage()// FDamageEvent DamageEvent, APlayerCharacter* PlayerCharacter)
{
	// APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		PlayerCharacter->TakeDamage(BaseDamage, DamageEvent, nullptr, GetOwner());
	}
}