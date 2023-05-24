// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActor_Door.h"
#include "Characters/RPGProjectPlayerCharacter.h"

AInteractableActor_Door::AInteractableActor_Door()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void AInteractableActor_Door::BeginPlay()
{
	Super::BeginPlay();

	StartRotation = GetActorRotation();
	FinalRotation = GetActorRotation() + DesiredRotation;

	// Ensure TimeToRotate is greater than EPSILON
	if (InteractableState == EInteractableState::Active)
	{
		CurrentRotationTime = TimeToRotate;
		SetActorRotation(FinalRotation);
	}
	else
	{
		CurrentRotationTime = 0.0f;
		SetActorRotation(StartRotation);
	}
	
}

void AInteractableActor_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InteractableState == EInteractableState::Activating)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		SetActorRotation(CurrentRotation);
		if (TimeRatio >= 1.0f)
		{
			InteractableHasActivated();
		}
	}
	else if (InteractableState == EInteractableState::Deactivating)
	{
		CurrentRotationTime -= DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = OpenCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(StartRotation, FinalRotation, RotationAlpha);
		SetActorRotation(CurrentRotation);
		if (TimeRatio <= 0.0f)
		{
			InteractableHasDeactivated();
		}
	}
}

void AInteractableActor_Door::EnableHighlight(bool bActive, int Colour)
{
	Super::EnableHighlight(bActive, Colour);
}

void AInteractableActor_Door::InteractionRequested(AActor* InteractingActor)
{
	Super::InteractionRequested(InteractingActor);
	
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Door::InteractionRequested called."));
}

void AInteractableActor_Door::StartInteraction(AActor* InteractingActor)
{
	Super::StartInteraction(InteractingActor);

	if (bActivateSelf)
	{
		switch (InteractableState)
		{
		case EInteractableState::Inactive:
			ActivateInteractable();
			break;

		case EInteractableState::Active:
			DeactivateInteractable();
			break;

		default:
			return;
			break;
		}
	}
}

void AInteractableActor_Door::ActivateInteractable()
{
	Super::ActivateInteractable();

	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Door::ActivateInteractable called"));

	if (InteractableState == EInteractableState::Active || InteractableState == EInteractableState::Activating)
	{
		return;
	}

	if (AudioComponent)
	{
		AudioComponent->Play();
	}

	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Door::ActivateInteractable InteractableActor is activating."));

	InteractableState = EInteractableState::Activating;
	bCanInteract = false;
}

void AInteractableActor_Door::DeactivateInteractable()
{
	Super::DeactivateInteractable();

	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Door::DeactivateInteractable called"));

	if (InteractableState == EInteractableState::Inactive || InteractableState == EInteractableState::Deactivating)
	{
		return;
	}

	if (AudioComponent)
	{
		AudioComponent->Play();
	}

	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Door::ActivateInteractable InteractableActor is deactivating."));

	InteractableState = EInteractableState::Deactivating;
	bCanInteract = false;

}

void AInteractableActor_Door::InteractableHasActivated()
{
	Super::InteractableHasActivated();
}

void AInteractableActor_Door::InteractableHasDeactivated()
{
	Super::InteractableHasDeactivated();
}

bool AInteractableActor_Door::GetIsInInteractableRange(AActor* InteractingActor) const
{
	if (ActorsInInteractionRange.Contains(InteractingActor))
	{
		return true;
	}
	return false;
}
