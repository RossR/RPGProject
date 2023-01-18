// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraManagers/RPGPlayerCameraManager.h"
#include "Controllers/RPGProjectPlayerController.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/CombatComponent.h"

static TAutoConsoleVariable<bool> CVarDisplayActorInViewTrace
(
	TEXT("RPGProject.RPGPlayerCameraManager.Debug.DisplayTrace"),
	false,
	TEXT("Display Trace"),
	ECVF_Default
);

ARPGPlayerCameraManager::ARPGPlayerCameraManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ViewPitchMin = -70.f;
	ViewPitchMax = 70.f;

	InterpSpeedMap.Emplace(ECameraView::Exploration, 5.f);
	InterpSpeedMap.Emplace(ECameraView::Action, 5.f);
	InterpSpeedMap.Emplace(ECameraView::Aim, 10.f);
	InterpSpeedMap.Emplace(ECameraView::LockOn, 5.f);
	InterpSpeedMap.Emplace(ECameraView::Skill, 5.f);
}

void ARPGPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwningPlayerController())
	{
		PC = Cast<ARPGProjectPlayerController>(GetOwningPlayerController());

		PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetOwningPlayerController()->GetCharacter());

		if (PlayerCharacter)
		{
			ViewTargetCameraRef = PlayerCharacter->GetPlayerCamera();
			CombatComponentRef = Cast<UCombatComponent>(PlayerCharacter->GetComponentByClass(UCombatComponent::StaticClass()));
		}
	}

	if (GetCameraView() == ECameraView::None)
	{
		SetCameraView(ECameraView::Exploration);
		if (ViewTargetCameraRef) { ViewTargetCameraRef->AttachToComponent(GetSpringArmFromCameraView(ECameraView::Exploration), FAttachmentTransformRules::KeepWorldTransform); }
	}

	CameraViewOnLastUpdate = GetCameraView();
}

void ARPGPlayerCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void ARPGPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	// If the camera view has changed since the last update, slowly interpolate the camera to the new camera view
	if (!ViewTargetCameraRef) { return; }
	if (GetCameraView() == CameraViewOnLastUpdate) { return; }
	if (GetCameraView() == ECameraView::None) { return; }

	CameraViewOnLastUpdate = GetCameraView();

	if (CameraSpringArmMap.Contains(CameraViewOnLastUpdate))
	{
		USpringArmComponent* InSpringArm = GetSpringArmFromCameraView(CameraViewOnLastUpdate);
		bool bAttachedSuccessfully = ViewTargetCameraRef->AttachToComponent(InSpringArm, FAttachmentTransformRules::KeepWorldTransform);

		if (bAttachedSuccessfully)
		{
			CurrentCameraArrow = GetArrowFromCameraView(CameraViewOnLastUpdate);

			if (!GetWorldTimerManager().IsTimerActive(InterpToViewTimerHandle))
			{
				GetWorldTimerManager().SetTimer(InterpToViewTimerHandle, this, &ARPGPlayerCameraManager::InterpToView, DeltaTime, true);
			}
		}
	}

	LockOnUpdate();
}

bool ARPGPlayerCameraManager::GetCrosshairTarget(FHitResult& CrosshairResult)
{
	const FVector EndPos = GetCameraLocation() + (GetActorForwardVector() * 10000.f);

	// Ignore all actors attached to the player character
	TArray<AActor*> ActorIgnoreArray;
	if (PlayerCharacter)
	{
		PlayerCharacter->GetAllChildActors(ActorIgnoreArray, true);
		ActorIgnoreArray.Emplace(PlayerCharacter);
	}

	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;
	if (bShowTrace)
	{
		DebugTrace = EDrawDebugTrace::ForOneFrame;
	}

	bool bTraceBlocked = false;
	if (bUseSphereTrace)
	{
		bTraceBlocked = UKismetSystemLibrary::SphereTraceSingleByProfile(GetWorld(), GetCameraLocation(), EndPos, TargetTraceRadius, "Projectile", false, ActorIgnoreArray, DebugTrace, CrosshairResult, true);
	}
	else
	{
		bTraceBlocked = UKismetSystemLibrary::LineTraceSingleByProfile(GetWorld(), GetCameraLocation(), EndPos, "Projectile", false, ActorIgnoreArray, DebugTrace, CrosshairResult, true);
	}

	if (!bTraceBlocked) { CrosshairResult.Location = EndPos; }

	return bTraceBlocked;
}

bool ARPGPlayerCameraManager::SetCameraView(ECameraView NewCameraView, bool bOverrideLockOn)
{
	if (CurrentCameraView == ECameraView::LockOn && bOverrideLockOn)
	{
		TargetActorAngle = FRotator::ZeroRotator;
		LockOnTargetActor = nullptr;
		TargetSocket = "";
		LastCameraView = ECameraView::LockOn;
		CurrentCameraView = NewCameraView;

		return true;
	}

	if (CurrentCameraView == ECameraView::LockOn && !bOverrideLockOn)
	{
		LastCameraView = NewCameraView;
		return false;
	}

	if (NewCameraView != CurrentCameraView)
	{
		LastCameraView = CurrentCameraView;
		CurrentCameraView = NewCameraView;

		return true;
	}
	return false;
}

UArrowComponent* ARPGPlayerCameraManager::GetArrowFromCameraView(ECameraView CameraViewIndex)
{
	if (CameraArrowMap.Contains(CameraViewIndex))
	{
		return CameraArrowMap[CameraViewIndex];
	}
	return nullptr;
}

bool ARPGPlayerCameraManager::SetArrowForCameraView(ECameraView CameraViewIndex, UArrowComponent* ArrowComp)
{
	if (!CameraArrowMap.Contains(CameraViewIndex))
	{
		CameraArrowMap.Emplace(CameraViewIndex, ArrowComp);
		return true;
	}
	else if (ArrowComp != CameraArrowMap[CameraViewIndex])
	{
		CameraArrowMap.Emplace(CameraViewIndex, ArrowComp);
		return true;
	}

	return false;
}

USpringArmComponent* ARPGPlayerCameraManager::GetSpringArmFromCameraView(ECameraView CameraViewIndex)
{
	if (CameraSpringArmMap.Contains(CameraViewIndex))
	{
		return CameraSpringArmMap[CameraViewIndex];
	}
	return nullptr;
}

bool ARPGPlayerCameraManager::SetSpringArmForCameraView(ECameraView CameraViewIndex, USpringArmComponent* SpringArmComp)
{
	if (!CameraSpringArmMap.Contains(CameraViewIndex))
	{
		CameraSpringArmMap.Emplace(CameraViewIndex, SpringArmComp);
		return true;
	}
	else if (SpringArmComp != CameraSpringArmMap[CameraViewIndex])
	{
		CameraSpringArmMap.Emplace(CameraViewIndex, SpringArmComp);
		return true;
	}

	return false;
}

void ARPGPlayerCameraManager::EnableLockOn()
{
	GetRenderedActorsInView(this, LockOnActorArray, "Targetable");
	if (!LockOnActorArray.IsEmpty())
	{
		FVector2D ViewportCentre = FVector2D::ZeroVector;
		ViewportCentre = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) * FVector2D(.5f, .5f);

		float ShortestViewportDistance = FLT_MAX;

		// Remove actors from the array if that have a health component and are not alive
		for (int i = 0; i < LockOnActorArray.Num(); i++)
		{
			if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				if (HealthComponentRef->IsDead()) { LockOnActorArray.RemoveAt(i); }
			}
		}

		// Lock on to actor closest to the centre of the screen
		for (int i = 0; i < LockOnActorArray.Num(); i++)
		{
			bool bReturnedViewportPosition = false;
			FVector2D LockOnActorViewportPosition = FVector2D::ZeroVector;

			UMeshComponent* LockOnActorMeshComponent = Cast<UMeshComponent>(LockOnActorArray[i]->GetComponentByClass(UMeshComponent::StaticClass()));
			if (LockOnActorMeshComponent)
			{
				bReturnedViewportPosition = UGameplayStatics::ProjectWorldToScreen(PC, LockOnActorMeshComponent->GetSocketLocation("LockOn_MainTarget"), LockOnActorViewportPosition, true);
			}
			else
			{
				bReturnedViewportPosition = UGameplayStatics::ProjectWorldToScreen(PC, LockOnActorArray[i]->GetActorLocation(), LockOnActorViewportPosition, true);
			}

			if (bReturnedViewportPosition)
			{
				float DistanceFromCentreOfViewport = fabsf(UKismetMathLibrary::VSize2D(LockOnActorViewportPosition - ViewportCentre));

				if (DistanceFromCentreOfViewport < ShortestViewportDistance)
				{
					ShortestViewportDistance = DistanceFromCentreOfViewport;
					LockOnTargetActor = LockOnActorArray[i];
					if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
					{
						ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
					}
				}
			}
			else { LockOnActorArray.RemoveAt(i); }
		}

		SetCameraView(ECameraView::LockOn);
		bSwapTargetOnCooldown = false;
	}
}

void ARPGPlayerCameraManager::DisableLockOn()
{
	SetCameraView(LastCameraView, true);
	TargetActorAngle = FRotator::ZeroRotator;
	LockOnTargetActor = nullptr;
	TargetSocket = "";
}

FVector ARPGPlayerCameraManager::GetLocationOfLockOnTargetActorsMainTarget()
{
	if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
	{
		if (ActorMeshComponent->DoesSocketExist("LockOn_MainTarget"))
		{
			return ActorMeshComponent->GetSocketLocation("LockOn_MainTarget");
		}
	}
	return LockOnTargetActor->GetActorLocation();
}

void ARPGPlayerCameraManager::GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float MinRecentTime)
{
	//Empty any previous entries
	CurrentlyRenderedActors.Empty();

	TArray<AActor*> AllActorsInWorldArray;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActorsInWorldArray);

	// Check every actor and add it to the array if it meets all the criteria
	for (int i = 0; i < AllActorsInWorldArray.Num(); ++i)
	{
		// Skip the actor if it is the character possessed by the camera manager's owner
		if (PlayerCharacter)
		{
			if (AllActorsInWorldArray[i] == PlayerCharacter)
			{
				continue;
			}
		}

		// Skip the actor if it is the current lock-on target
		if (AllActorsInWorldArray[i] == LockOnTargetActor) { continue; }

		// Skip the actor if they have have a health component and are dead
		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(AllActorsInWorldArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { continue; }
		}

		// Skip the actor if it does not have the specified tag
		if (!AllActorsInWorldArray[i]->ActorHasTag(ActorTag) && !ActorTag.IsNone())
		{
			continue;
		}

		// Skip the actor if they were rendered too recently
		if (AllActorsInWorldArray[i]->GetLastRenderTime() < MinRecentTime)
		{
			continue;
		}

		// If the actor is not already in the array
		if (!CurrentlyRenderedActors.Contains(AllActorsInWorldArray[i]))
		{
			FRotator ViewingActorRotation = FRotator::ZeroRotator;
			FVector ViewingActorLocation = FVector::ZeroVector;
			if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(ViewingActor))
			{
				ViewingActorRotation = CameraManager->GetCameraRotation();
				ViewingActorLocation = CameraManager->GetCameraLocation();
			}
			else
			{
				ViewingActorRotation = ViewingActor->GetActorRotation();
				ViewingActorLocation = ViewingActor->GetActorLocation();
			}

			FVector ActorLocation = FVector::ZeroVector;
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(AllActorsInWorldArray[i]->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				ActorLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget");
			}
			else
			{
				ActorLocation = AllActorsInWorldArray[i]->GetActorLocation();
			}

			const FRotator RotationFromViewingActor = UKismetMathLibrary::FindLookAtRotation(ViewingActorLocation, ActorLocation);
			const FRotator AngleFromViewingActor = UKismetMathLibrary::ComposeRotators(ViewingActorRotation, UKismetMathLibrary::NegateRotator(RotationFromViewingActor));

			const bool bAcceptablePitch = (AngleFromViewingActor.Pitch <= FirstLockAllowedViewAngle.Pitch && AngleFromViewingActor.Pitch >= -FirstLockAllowedViewAngle.Pitch);
			const bool bAcceptableYaw = (AngleFromViewingActor.Yaw <= FirstLockAllowedViewAngle.Yaw && AngleFromViewingActor.Yaw >= -FirstLockAllowedViewAngle.Yaw);
			const bool bAcceptableRoll = (AngleFromViewingActor.Roll <= FirstLockAllowedViewAngle.Roll && AngleFromViewingActor.Roll >= -FirstLockAllowedViewAngle.Roll);

			// If the actor has an acceptable angle from the perspective of the viewing actor (default is 15°)
			if (bAcceptablePitch && bAcceptableYaw && bAcceptableRoll)
			{
				// Check that the actor is within the ViewingActor's line of sight 

				TArray<AActor*> ActorsToIgnore;
				if (PlayerCharacter)
				{
					ActorsToIgnore.Emplace(PlayerCharacter);
				}
				if (LockOnTargetActor)
				{
					ActorsToIgnore.Emplace(LockOnTargetActor);
				}

				const EDrawDebugTrace::Type DebugTrace = CVarDisplayActorInViewTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
				FHitResult HitResult;

				UKismetSystemLibrary::LineTraceSingle(GetWorld(), ViewingActor->GetActorLocation(), ActorLocation, ActorInViewTraceCollisionChannel, false, ActorsToIgnore, DebugTrace, HitResult, true);

				if (HitResult.bBlockingHit)
				{
					if (HitResult.GetActor() == AllActorsInWorldArray[i])
					{
						CurrentlyRenderedActors.AddUnique(AllActorsInWorldArray[i]);
					}
				}
			}
		}
	}
}

void ARPGPlayerCameraManager::GetRenderedActorsInViewportCircularSector(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float TargetAngle, float SearchSectorSize, float MinRecentTime)
{
	//Empty any previous entries
	CurrentlyRenderedActors.Empty();

	TArray<AActor*> AllActorsInWorldArray;

	FVector2D ViewportCentre = FVector2D::ZeroVector;
	ViewportCentre = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) * FVector2D(.5f, .5f);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActorsInWorldArray);

	// Check every actor and add it to the array if it meets all the criteria
	for (int i = 0; i < AllActorsInWorldArray.Num(); ++i) // TObjectIterator Itr | 
	{
		// Skip the actor if it is the character possessed by the camera manager's owner
		if (PlayerCharacter)
		{
			if (AllActorsInWorldArray[i] == PlayerCharacter)
			{
				continue;
			}
		}

		// Skip the actor if it is the current lock-on target
		if (AllActorsInWorldArray[i] == LockOnTargetActor) { continue; }

		// Skip the actor if they have have a health component and are dead
		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(AllActorsInWorldArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { continue; }
		}

		// Skip the actor if it does not have the specified tag
		if (!AllActorsInWorldArray[i]->ActorHasTag(ActorTag) && !ActorTag.IsNone())
		{
			continue;
		}

		// Skip the actor if they were rendered too recently
		if (AllActorsInWorldArray[i]->GetLastRenderTime() < MinRecentTime)
		{
			continue;
		}

		// If the actor is not already in the array
		if (!CurrentlyRenderedActors.Contains(AllActorsInWorldArray[i]))
		{
			bool bActorIsInViewport = false;
			FVector2D ActorViewportPosition = FVector2D::ZeroVector;

			FVector ActorLocation = FVector::ZeroVector;
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(AllActorsInWorldArray[i]->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				ActorLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget");
				bActorIsInViewport = UGameplayStatics::ProjectWorldToScreen(PC, ActorMeshComponent->GetSocketLocation("LockOn_MainTarget"), ActorViewportPosition, true);
			}
			else
			{
				ActorLocation = AllActorsInWorldArray[i]->GetActorLocation();
				bActorIsInViewport = UGameplayStatics::ProjectWorldToScreen(PC, AllActorsInWorldArray[i]->GetActorLocation(), ActorViewportPosition, true);
			}

			if (bActorIsInViewport)
			{
				const FVector2D ViewportSizeVector = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
				if ((ActorViewportPosition.X < 0.f || ActorViewportPosition.X > ViewportSizeVector.X) || (ActorViewportPosition.Y < 0.f || ActorViewportPosition.Y > ViewportSizeVector.Y))
				{
					continue;
				}

				const float ActorsAngleFromViewportCentre = GetViewportAngleFromScreenPosition(ActorViewportPosition);

				// If the actor is inside the search sector
				if (ActorsAngleFromViewportCentre >= (TargetAngle - (SearchSectorSize * .5f)) && ActorsAngleFromViewportCentre <= (TargetAngle + (SearchSectorSize * .5f)))
				{
					// Check that the actor is within the ViewingActor's line of sight 

					TArray<AActor*> ActorsToIgnore;
					if (PlayerCharacter)
					{
						ActorsToIgnore.Emplace(PlayerCharacter);
					}
					if (LockOnTargetActor)
					{
						ActorsToIgnore.Emplace(LockOnTargetActor);
					}

					EDrawDebugTrace::Type DebugTrace = CVarDisplayActorInViewTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
					FHitResult HitResult;
					UKismetSystemLibrary::LineTraceSingle(GetWorld(), ViewingActor->GetActorLocation(), ActorLocation, ActorInViewTraceCollisionChannel, false, ActorsToIgnore, DebugTrace, HitResult, true);

					if (HitResult.bBlockingHit)
					{
						if (HitResult.GetActor() == AllActorsInWorldArray[i])
						{
							CurrentlyRenderedActors.AddUnique(AllActorsInWorldArray[i]);
						}
					}
				}
			}
		}
	}
}

void ARPGPlayerCameraManager::SwapLockOnTarget(float InYawValue, float InPitchValue)
{
	if (!ViewTargetCameraRef) { return; }
	if (GetCameraView() != ECameraView::LockOn) { return; }
	if (bSwapTargetOnCooldown) { return; }

	const float PitchAndYawSum = FMath::Abs(InYawValue) + FMath::Abs(InPitchValue);
	const float NormalisedYaw = 1.f * (InYawValue / PitchAndYawSum);
	const float NormalisedPitch = 1.f * (-InPitchValue / PitchAndYawSum);

	const float AngleToSearch = GetViewportAngleFromVector2D({ NormalisedYaw, NormalisedPitch });

	// Search for actors with the first search sector size
	GetRenderedActorsInViewportCircularSector(this, LockOnActorArray, "Targetable", AngleToSearch, SwapTargetSearchSectorSize, .01f);

	if (!LockOnActorArray.IsEmpty())
	{
		EvaluateSwapLockOnTargets();
	}
	else
	{
		// Search for actors with the second search sector size
		GetRenderedActorsInViewportCircularSector(this, LockOnActorArray, "Targetable", AngleToSearch, SwapTargetSecondSearchSectorSize, .01f);
		if (!LockOnActorArray.IsEmpty())
		{
			EvaluateSwapLockOnTargets();
		}
	}
}

void ARPGPlayerCameraManager::InterpToView()
{
	if (!PlayerCharacter || !ViewTargetCameraRef || !CurrentCameraArrow) { return; }

	if (ViewTargetCameraRef->GetRelativeLocation() != CurrentCameraArrow->GetRelativeLocation())
	{
		if (InterpSpeedMap.Contains(CameraViewOnLastUpdate))
		{
			const FVector InterpVector = UKismetMathLibrary::VInterpTo(ViewTargetCameraRef->GetRelativeLocation(), CurrentCameraArrow->GetRelativeLocation(), GetWorld()->GetDeltaSeconds(), InterpSpeedMap[CameraViewOnLastUpdate]);
			const FRotator InterpRotator = UKismetMathLibrary::RInterpTo(ViewTargetCameraRef->GetRelativeRotation(), CurrentCameraArrow->GetRelativeRotation(), GetWorld()->GetDeltaSeconds(), InterpSpeedMap[CameraViewOnLastUpdate]);

			ViewTargetCameraRef->SetRelativeLocationAndRotation(InterpVector, InterpRotator);
		}

		/*float LocationInterpSpeed = -1.f;
		float RotationInterpSpeed = -1.f;

		if (InterpSpeedMap.Contains(CameraViewOnLastUpdate))
		{
			LocationInterpSpeed = InterpSpeedMap[CameraViewOnLastUpdate];
			RotationInterpSpeed = InterpSpeedMap[CameraViewOnLastUpdate];
		}

		const FVector InterpVector = UKismetMathLibrary::VInterpTo(ViewTargetCameraRef->GetRelativeLocation(), CurrentCameraArrow->GetRelativeLocation(), GetWorld()->GetDeltaSeconds(), LocationInterpSpeed);
		const FRotator InterpRotator = UKismetMathLibrary::RInterpTo(ViewTargetCameraRef->GetRelativeRotation(), CurrentCameraArrow->GetRelativeRotation(), GetWorld()->GetDeltaSeconds(), RotationInterpSpeed);

		ViewTargetCameraRef->SetRelativeLocationAndRotation(InterpVector, InterpRotator);*/
	}
	else
	{
		GetWorldTimerManager().ClearTimer(InterpToViewTimerHandle);
	}
}

float ARPGPlayerCameraManager::GetViewportAngleFromScreenPosition(FVector2D ScreenPosition)
{
	FVector2D CentreVector2D{ 0.f, 1.f };
	FVector2D ViewportCentre = UWidgetLayoutLibrary::GetViewportSize(GetWorld()) * FVector2D(.5f, .5f);

	ScreenPosition = ScreenPosition - ViewportCentre;
	ScreenPosition.Y = -ScreenPosition.Y;

	CentreVector2D = UKismetMathLibrary::Normal2D(CentreVector2D);
	FVector2D TargetVector2D = UKismetMathLibrary::Normal2D(ScreenPosition);
	if (TargetVector2D.X < 0.f)
	{
		return (360.f - UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D))));
	}
	return UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D)));
}

float ARPGPlayerCameraManager::GetViewportAngleFromVector2D(FVector2D InVector2D)
{
	FVector2D CentreVector2D{ 0.f, 1.f };
	CentreVector2D = UKismetMathLibrary::Normal2D(CentreVector2D);
	FVector2D TargetVector2D = UKismetMathLibrary::Normal2D(InVector2D);
	if (TargetVector2D.X < 0.f)
	{
		return (360.f - UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D))));
	}
	return UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D)));
}

void ARPGPlayerCameraManager::EvaluateSwapLockOnTargets()
{
	FVector2D LockOnTargetActorScreenPosition = FVector2D::ZeroVector;
	UGameplayStatics::ProjectWorldToScreen(PC, LockOnTargetActor->GetActorLocation(), LockOnTargetActorScreenPosition, true);

	float ClosestToSearchAngle = FLT_MAX;

	float ShortestActorDistance = FLT_MAX;
	float ShortestViewportDistance = FLT_MAX;

	// Remove actors that have a health component and are not alive
	for (int i = 0; i < LockOnActorArray.Num(); i++)
	{
		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { LockOnActorArray.RemoveAt(i); }
		}
	}

	// Lock on to actor closest to the centre of the viewport
	for (int i = 0; i < LockOnActorArray.Num(); i++)
	{
		bool bReturnedViewportPosition = false;
		FVector2D LockOnActorViewportPosition = FVector2D::ZeroVector;

		UMeshComponent* LockOnActorMeshComponent = Cast<UMeshComponent>(LockOnActorArray[i]->GetComponentByClass(UMeshComponent::StaticClass()));
		if (LockOnActorMeshComponent)
		{
			bReturnedViewportPosition = UGameplayStatics::ProjectWorldToScreen(PC, LockOnActorMeshComponent->GetSocketLocation("LockOn_MainTarget"), LockOnActorViewportPosition, true);
		}
		else
		{
			bReturnedViewportPosition = UGameplayStatics::ProjectWorldToScreen(PC, LockOnActorArray[i]->GetActorLocation(), LockOnActorViewportPosition, true);
		}

		// If the actor is currently in the viewport
		if (bReturnedViewportPosition)
		{
			float DistanceFromCentreOfViewport = fabsf(UKismetMathLibrary::VSize2D(LockOnActorViewportPosition - LockOnTargetActorScreenPosition));

			// If the actor is the actor closest to the centre of the viewport, set it as the lock-on target
			if (DistanceFromCentreOfViewport < ShortestViewportDistance)
			{
				ShortestViewportDistance = DistanceFromCentreOfViewport;
				LockOnTargetActor = LockOnActorArray[i];
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
				}

				bSwapTargetOnCooldown = true;
			}
		}
		else { LockOnActorArray.RemoveAt(i); }
	}
	// Prevent target swapping until the cooldown period has ended
	if (bSwapTargetOnCooldown)
	{
		GetWorldTimerManager().SetTimer(SwapTargetCooldownTimerHandle, this, &ARPGPlayerCameraManager::ResetSwapTargetCooldown, GetWorld()->GetDeltaSeconds(), false, SwapCooldown);
		bCanSwapTarget = false;
	}
}

void ARPGPlayerCameraManager::LockOnUpdate()
{
	if (PC && CurrentCameraView == ECameraView::LockOn)
	{
		// Disable lock-on if target dies
		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnTargetActor->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { LockOnTargetActor = nullptr; }
		}

		// Disable lock-on if LockOnTargetActor is nullptr
		if (LockOnTargetActor)
		{
			TArray<AActor*> ActorsToIgnore;
			if (PlayerCharacter) { ActorsToIgnore.Add(PlayerCharacter); }

			EDrawDebugTrace::Type DebugTrace = CVarDisplayActorInViewTrace->GetBool() ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
			FHitResult InHitResult;
			UKismetSystemLibrary::LineTraceSingle(GetWorld(), GetCameraLocation(), LockOnTargetActor->GetActorLocation(), ActorInViewTraceCollisionChannel, false, ActorsToIgnore, DebugTrace, InHitResult, true);

			// Start the timer to disable lock-on if the target cannot be seen and is too far away from the player character
			if (InHitResult.GetActor() != LockOnTargetActor && (PlayerCharacter->GetActorLocation() - LockOnTargetActor->GetActorLocation()).Length() > 750.f)
			{
				if (!GetWorldTimerManager().IsTimerActive(NoLineOfSightOnTargetTimerHandle))
				{
					GetWorldTimerManager().SetTimer(NoLineOfSightOnTargetTimerHandle, this, &ARPGPlayerCameraManager::DisableLockOn, GetWorld()->GetDeltaSeconds(), false, 1.f);
				}
			}
			// Cancel the timer if the target is in the player character's line of sight, or is close enough to the player character
			else
			{
				if (GetWorldTimerManager().IsTimerActive(NoLineOfSightOnTargetTimerHandle))
				{
					GetWorldTimerManager().ClearTimer(NoLineOfSightOnTargetTimerHandle);
				}
			}

			// Get the angle to look at the target actor
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				TargetActorAngle = UKismetMathLibrary::FindLookAtRotation(CameraSpringArmMap[ECameraView::LockOn]->GetComponentLocation(), ActorMeshComponent->GetSocketLocation(TargetSocket));

			}
			else
			{
				TargetActorAngle = UKismetMathLibrary::FindLookAtRotation(CameraSpringArmMap[ECameraView::LockOn]->GetComponentLocation(), LockOnTargetActor->GetActorLocation());
			}

			// Look towards the target actor
			if (TargetActorAngle != FRotator::ZeroRotator)
			{
				PC->SetControlRotation({ TargetActorAngle.Pitch, TargetActorAngle.Yaw, PC->GetControlRotation().Roll });
			}
		}
		else
		{
			DisableLockOn();
		}
	}
}