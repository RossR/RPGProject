// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/StaminaComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;

	// ...
	
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	StaminaRegenInterval = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);

	if (CurrentStamina == MaxStamina)
	{
		IsRegeneratingStamina = false;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	} 
	else if (!IsRegeneratingStamina && CurrentStamina < MaxStamina /* And if didn't take stamina damage this tick */)
	{
		// UE_LOG(LogTemp, Warning, TEXT("UStaminaComponent::TickComponent StaminaRegenTimerHandle Set"));
		if (!GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &UStaminaComponent::RegenerateStamina, StaminaRegenInterval, true, StaminaRegenDelay);
		}
	}
}

void UStaminaComponent::TakeStaminaDamage(float Damage)
{
	CurrentStamina -= Damage;
	if (IsRegeneratingStamina)
	{
		IsRegeneratingStamina = false;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
}

void UStaminaComponent::RegenerateStamina() 
{ 
	IsRegeneratingStamina = true;
	CurrentStamina += StaminaRegenPerInterval; 
}