// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactables/InteractableActor_Base.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Characters/RPGProjectPlayerCharacter.h"

// Sets default values
AInteractableActor_Base::AInteractableActor_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InteractableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractableMesh"));
	InteractableMesh->SetCollisionProfileName(FName("Interactable"));
	SetRootComponent(InteractableMesh);

	InteractionRangeVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionRange"));
	InteractionRangeVolume->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
	InteractionRangeVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor_Base::OnOverlapBegin);
	InteractionRangeVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor_Base::OnOverlapEnd);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("InteractableAudio"));
	AudioComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);

	Tags.Add("Interactable");
}

// Called when the game starts or when spawned
void AInteractableActor_Base::BeginPlay()
{
	Super::BeginPlay();

	if (bStartActive)
	{
		InteractableState = EInteractableState::Active;
	}
	else
	{
		InteractableState = EInteractableState::Inactive;
	}
}

// Called every frame
void AInteractableActor_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableActor_Base::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ACharacter* CharacterCast = Cast<ACharacter>(OtherActor))
	{
		ActorsInInteractionRange.AddUnique(OtherActor);
	}
}

void AInteractableActor_Base::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ACharacter* CharacterCast = Cast<ACharacter>(OtherActor))
	{
		ActorsInInteractionRange.RemoveSingle(OtherActor);
	}
}

void AInteractableActor_Base::EnableHighlight(bool bActive, int Colour)
{
	if (!bCanHighlight) { return; }
	InteractableMesh->SetCustomDepthStencilValue(Colour);
	InteractableMesh->SetRenderCustomDepth(bActive);
}

void AInteractableActor_Base::InteractionRequested(AActor* InteractingActor)
{
	if (!ActorsInInteractionRange.Contains(InteractingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractableActorBase::InteractionStart InteractingActor is not in interaction range."));
		return;
	}

	if (bIsLocked)
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractableActorBase::InteractionStart InteractableActor is locked."));
		return;
	}

	if (!bCanInteract)
	{
		UE_LOG(LogTemp, Warning, TEXT("AInteractableActorBase::InteractionStart InteractableActor cannot be interacted with right now."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("AInteractableActor_Base::InteractionRequested called."));

	ARPGProjectPlayerCharacter* PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(InteractingActor);
	if (PlayerCharacter)
	{
		AActor* InteractableActor = this;
		PlayerCharacter->InteractionStarted(InteractableActor);
	}
}

void AInteractableActor_Base::InteractionStart(AActor* InteractingActor)
{
	if (bActivateSelf)
	{
		switch (GetInteractableState())
		{
		case EInteractableState::Inactive:
			ActivateInteractable();
			break;

		case EInteractableState::Active:
			DeactivateInteractable();
			break;

		default:
			break;
		}
	}

	if (!ActivatableActorsArray.IsEmpty())
	{
		for (int i = 0; i < ActivatableActorsArray.Num(); i++)
		{
			if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(ActivatableActorsArray[i]))
			{
				switch (InteractionInterface->GetInteractableState())
				{
				case EInteractableState::Inactive:
					InteractionInterface->ActivateInteractable();
					break;

				case EInteractableState::Active:
					InteractionInterface->DeactivateInteractable();
					break;

				default:
					break;
				}
			}
		}
	}
}

void AInteractableActor_Base::ActivateInteractable()
{
	BPActivateInteractable();
}

void AInteractableActor_Base::DeactivateInteractable()
{
	BPDeactivateInteractable();
}

void AInteractableActor_Base::InteractableActivated()
{
	switch (InteractionType)
	{
	case EInteractionType::Toggle:
		InteractableState = EInteractableState::Active;
		bCanInteract = true;
		break;

	case EInteractionType::Button:
		if (bStartActive)
		{
			InteractableState = EInteractableState::Active;
			bCanInteract = true;
		}
		else
		{
			DeactivateInteractable();
		}
		break;

	case EInteractionType::Timer:
		InteractableState = EInteractableState::Active;
		if (!bStartActive)
		{
			GetWorld()->GetTimerManager().SetTimer(InteractTimerHandle, this, &AInteractableActor_Base::InteractTimer, TimerDuration, false, -1.f);
		}
		else { bCanInteract = true; }
		break;

	case EInteractionType::SingleUse:
		InteractableState = EInteractableState::Active;
		break;

	default:
		break;
	}

	
}

void AInteractableActor_Base::InteractableDeactivated()
{
	switch (InteractionType)
	{
	case EInteractionType::Toggle:
		InteractableState = EInteractableState::Inactive;
		bCanInteract = true;
		break;

	case EInteractionType::Button:
		if (bStartActive)
		{
			ActivateInteractable();
		}
		else
		{
			InteractableState = EInteractableState::Inactive;
			bCanInteract = true;
		}
		break;

	case EInteractionType::Timer:
		InteractableState = EInteractableState::Inactive;
		if (bStartActive)
		{
			GetWorld()->GetTimerManager().SetTimer(InteractTimerHandle, this, &AInteractableActor_Base::InteractTimer, TimerDuration, false, -1.f);
		}
		else { bCanInteract = true; }
		break;

	case EInteractionType::SingleUse:
		InteractableState = EInteractableState::Inactive;
		break;

	default:
		break;
	}

	
}

bool AInteractableActor_Base::GetIsInInteractableRange(AActor* InteractingActor)
{
	if (ActorsInInteractionRange.Contains(InteractingActor))
	{
		return true;
	}
	return false;
}

void AInteractableActor_Base::InteractTimer()
{
	InteractionStart();
}
