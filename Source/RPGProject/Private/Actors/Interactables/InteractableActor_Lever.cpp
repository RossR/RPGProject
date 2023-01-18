// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActor_Lever.h"

AInteractableActor_Lever::AInteractableActor_Lever()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	LeverHandleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeverHandleMesh"));
	LeverHandleMesh->SetupAttachment(InteractableMesh);
}

void AInteractableActor_Lever::BeginPlay()
{
	Super::BeginPlay();

	if (bStartActive)
	{
		LeverHandleMesh->SetRelativeRotation(ActiveRotation);
	}
	else
	{
		LeverHandleMesh->SetRelativeRotation(InactiveRotation);
	}
}

void AInteractableActor_Lever::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (InteractableState == EInteractableState::Activating)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = LeverRotationCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(InactiveRotation, ActiveRotation, RotationAlpha);
		LeverHandleMesh->SetRelativeRotation(CurrentRotation);
		if (TimeRatio >= 1.0f)
		{
			InteractableHasActivated();
		}
	}
	else if (InteractableState == EInteractableState::Deactivating)
	{
		CurrentRotationTime -= DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float RotationAlpha = LeverRotationCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FRotator CurrentRotation = FMath::Lerp(InactiveRotation, ActiveRotation, RotationAlpha);
		LeverHandleMesh->SetRelativeRotation(CurrentRotation);
		if (TimeRatio <= 0.0f)
		{
			InteractableHasDeactivated();
		}
	}
}

void AInteractableActor_Lever::EnableHighlight(bool bActive, int Colour)
{
	Super::EnableHighlight(bActive, Colour);

	LeverHandleMesh->SetCustomDepthStencilValue(Colour);
	LeverHandleMesh->SetRenderCustomDepth(bActive);
}

void AInteractableActor_Lever::InteractionRequested(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::InteractionRequested called."));

	Super::InteractionRequested(InteractingActor);
}

void AInteractableActor_Lever::StartInteraction(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::InteractionStart called."));

	Super::StartInteraction(InteractingActor);

}

void AInteractableActor_Lever::ActivateInteractable()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::ActivateInteractable called."));

	Super::ActivateInteractable();

	bCanInteract = false;
	InteractableState = EInteractableState::Activating;
}

void AInteractableActor_Lever::DeactivateInteractable()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::DeactivateInteractable called."));

	Super::DeactivateInteractable();

	bCanInteract = false;
	InteractableState = EInteractableState::Deactivating;
}

void AInteractableActor_Lever::InteractableHasActivated()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::InteractableActivated called."));

	Super::InteractableHasActivated();

}

void AInteractableActor_Lever::InteractableHasDeactivated()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Lever::InteractableDeactivated called."));

	Super::InteractableHasDeactivated();

}