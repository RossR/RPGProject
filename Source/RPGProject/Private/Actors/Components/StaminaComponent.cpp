// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/StaminaComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstances/RPGProjectAnimInstance.h"

// Sets default values for this component's properties
UStaminaComponent::UStaminaComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	StaminaDrainTypeMap.Add(EStaminaDrainType::Sprint, 1.f);
}


// Called when the game starts
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;

	if (USkeletalMeshComponent* MeshComponentRef = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass())))
	{
		OwnerAnimInstance = Cast<URPGProjectAnimInstance>(MeshComponentRef->GetAnimInstance());
	}

	// ...
	
}


// Called every frame
void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	StaminaRegenPerFrame = (MaxStamina / TimeToFullyRegenStamina) * DeltaTime;

	if (OwnerAnimInstance)
	{
		if (OwnerAnimInstance->GetCurveValue("DisableStaminaRegen") >= 1.f) { StopStaminaRegeneration(); }
	}

	if (IsStaminaRegenerating() && GetCurrentStamina() < GetMaxStamina())
	{
		RegenerateStamina();
	}
	else
	{
		StartStaminaRegeneration();
	}
}

void UStaminaComponent::ReduceCurrentStamina(float Damage)
{
	CurrentStamina -= Damage;
	if (true)//bIsStaminaRegenerating)
	{
		bIsStaminaRegenerating = false;
		StopStaminaRegeneration();
	}
}

void UStaminaComponent::DrainStaminaPerSecond(EStaminaDrainType StaminaDrainType)
{
	if (StaminaDrainTypeMap.Contains(StaminaDrainType))
	{
		CurrentStamina -= StaminaDrainTypeMap[StaminaDrainType];
		bIsStaminaRegenerating = false;
		StopStaminaRegeneration();
	}
}

bool UStaminaComponent::IsStaminaExhausted()
{
	if (!bHasStaminaBeenExhausted)
	{
		bHasStaminaBeenExhausted = CurrentStamina <= FLT_EPSILON;
	}
	else if (bHasStaminaBeenExhausted && (CurrentStamina / MaxStamina >= ExhaustionRecoveryPercentage))
	{
		bHasStaminaBeenExhausted = false;
	}

	return bHasStaminaBeenExhausted;
}

void UStaminaComponent::StaminaRegenTimerFinished()
{
	bIsStaminaRegenerating = true;
}

void UStaminaComponent::RegenerateStamina()
{ 
	CurrentStamina += StaminaRegenPerFrame * StaminaRegenMultiplier;
}

void UStaminaComponent::StartStaminaRegeneration()
{
	if (bIsStaminaRegenerating && CurrentStamina == MaxStamina)
	{
		bIsStaminaRegenerating = false;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
	}
	else if (!bIsStaminaRegenerating && GetCurrentStamina() < GetMaxStamina())
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(StaminaRegenTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimerHandle, this, &UStaminaComponent::StaminaRegenTimerFinished, GetWorld()->GetDeltaSeconds(), false, StaminaRegenDelay * StaminaRegenDelayMultiplier);
		}
	}
}

void UStaminaComponent::StopStaminaRegeneration()
{
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimerHandle);
}
