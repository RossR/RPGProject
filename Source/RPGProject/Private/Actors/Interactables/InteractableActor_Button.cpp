// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActor_Button.h"

AInteractableActor_Button::AInteractableActor_Button()
{
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	ButtonMesh->SetupAttachment(InteractableMesh);
}

void AInteractableActor_Button::BeginPlay()
{

}

void AInteractableActor_Button::Tick(float DeltaTime)
{
	if (InteractableState == EInteractableState::Activating)
	{
		CurrentRotationTime += DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float LocationAlpha = ButtonPressCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FVector CurrentLocation = FMath::Lerp(StartingVector, PressedVector, LocationAlpha);
		ButtonMesh->SetRelativeLocation(CurrentLocation);
		if (TimeRatio >= 1.0f)
		{
			InteractableActivated();
		}
	}
	else if (InteractableState == EInteractableState::Deactivating)
	{
		CurrentRotationTime -= DeltaTime;
		const float TimeRatio = FMath::Clamp(CurrentRotationTime / TimeToRotate, 0.0f, 1.0f);
		const float LocationAlpha = ButtonPressCurve.GetRichCurveConst()->Eval(TimeRatio);
		const FVector CurrentLocation = FMath::Lerp(StartingVector, PressedVector, LocationAlpha);
		ButtonMesh->SetRelativeLocation(CurrentLocation);
		if (TimeRatio <= 0.0f)
		{
			InteractableDeactivated();
		}
	}
}

void AInteractableActor_Button::EnableHighlight(bool bActive, int Colour)
{
	Super::EnableHighlight(bActive, Colour);

	ButtonMesh->SetCustomDepthStencilValue(Colour);
	ButtonMesh->SetRenderCustomDepth(bActive);
}

void AInteractableActor_Button::InteractionRequested(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::InteractionRequested called."));

	Super::InteractionRequested(InteractingActor);

}

void AInteractableActor_Button::InteractionStart(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::InteractionStart called."));

	Super::InteractionStart(InteractingActor);

}

void AInteractableActor_Button::ActivateInteractable()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::ActivateInteractable called."));

	Super::ActivateInteractable();

	bCanInteract = false;
	InteractableState = EInteractableState::Activating;

}

void AInteractableActor_Button::DeactivateInteractable()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::DeactivateInteractable called."));

	Super::DeactivateInteractable();

	bCanInteract = false;
	InteractableState = EInteractableState::Deactivating;
}

void AInteractableActor_Button::InteractableActivated()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::InteractableActivated called."));

	Super::InteractableActivated();

}

void AInteractableActor_Button::InteractableDeactivated()
{
	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Button::InteractableDeactivated called."));

	Super::InteractableDeactivated();

}
