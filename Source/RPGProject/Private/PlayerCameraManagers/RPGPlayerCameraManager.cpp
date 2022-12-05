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

#include "Actors/Components/HealthComponent.h"

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
	/*PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.SetTickFunctionEnable(true);*/

	ViewPitchMin = -70.f;
	ViewPitchMax = 70.f;

	InterpSpeedMap.Emplace(ECameraView::CV_Exploration, 5.f);
	InterpSpeedMap.Emplace(ECameraView::CV_Action, 5.f);
	InterpSpeedMap.Emplace(ECameraView::CV_Aim, 10.f);
	InterpSpeedMap.Emplace(ECameraView::CV_LockOn, 5.f);
	InterpSpeedMap.Emplace(ECameraView::CV_Skill, 5.f);
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
		}
	}

	if (GetCameraView() == ECameraView::CV_None)
	{
		SetCameraView(ECameraView::CV_Exploration);
	}

	CameraViewLastUpdate = GetCameraView();
}

void ARPGPlayerCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PC)
	{
		if (CurrentCameraView == ECameraView::CV_LockOn )
		{
			if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnTargetActor->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				if (HealthComponentRef->IsDead()) { LockOnTargetActor = nullptr; }
			}

			// Disable lock-on if target dies
			if (LockOnTargetActor)
			{
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					TargetActorAngle = UKismetMathLibrary::FindLookAtRotation(CameraSpringArmMap[ECameraView::CV_LockOn]->GetComponentLocation(), ActorMeshComponent->GetSocketLocation(TargetSocket));

				}
				else
				{
					TargetActorAngle = UKismetMathLibrary::FindLookAtRotation(CameraSpringArmMap[ECameraView::CV_LockOn]->GetComponentLocation(), LockOnTargetActor->GetActorLocation());
				}

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
}

void ARPGPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	if (!PlayerCharacter) { return; }
	if (!ViewTargetCameraRef) { return; }
	if (GetCameraView() == CameraViewLastUpdate) { return; }
	if (GetCameraView() == ECameraView::CV_None) { return; }

	CameraViewLastUpdate = GetCameraView();

	if (CameraSpringArmMap.Contains(CameraViewLastUpdate))
	{
		const bool bAttachedSuccessfully = ViewTargetCameraRef->AttachToComponent(GetValueFromCameraSpringArmMap(CameraViewLastUpdate), FAttachmentTransformRules::KeepWorldTransform);

		if (bAttachedSuccessfully)
		{
			CurrentCameraArrow = GetValueFromCameraArrowMap(CameraViewLastUpdate);

			if (!GetWorldTimerManager().IsTimerActive(InterpToViewTimerHandle))
			{
				GetWorldTimerManager().SetTimer(InterpToViewTimerHandle, this, &ARPGPlayerCameraManager::InterpToView, DeltaTime, true);
			}
		}
	}

}

bool ARPGPlayerCameraManager::SetCameraView(ECameraView NewCameraView, bool bOverrideLockOn)
{
	if (CurrentCameraView == ECameraView::CV_LockOn && !bOverrideLockOn) 
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

bool ARPGPlayerCameraManager::SetCameraSpringArmMap(ECameraView CameraViewIndex, USpringArmComponent* SpringArmComp)
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

USpringArmComponent* ARPGPlayerCameraManager::GetValueFromCameraSpringArmMap(ECameraView CameraViewIndex)
{
	if (CameraSpringArmMap.Contains(CameraViewIndex))
	{
		return CameraSpringArmMap[CameraViewIndex];
	}
	return nullptr;
}

bool ARPGPlayerCameraManager::SetCameraArrowMap(ECameraView CameraViewIndex, UArrowComponent* ArrowComp)
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

UArrowComponent* ARPGPlayerCameraManager::GetValueFromCameraArrowMap(ECameraView CameraViewIndex)
{
	if (CameraArrowMap.Contains(CameraViewIndex))
	{
		return CameraArrowMap[CameraViewIndex];
	}
	return nullptr;
}

void ARPGPlayerCameraManager::GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float MinRecentTime, FRotator MinAllowedViewAngle, FRotator MaxAllowedViewAngle)
{

	//Empty any previous entries
	CurrentlyRenderedActors.Empty();

	TArray<AActor*> ActorArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorArray);

	//Iterate Over Actors
	for (int i = 0; i < ActorArray.Num(); ++i) // TObjectIterator Itr | 
	{
		if (PlayerCharacter)
		{
			if (ActorArray[i] == PlayerCharacter)
			{
				continue;
			}
		}

		if (ActorArray[i] == LockOnTargetActor) { continue; }

		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(ActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { continue; }
		}

		if (!CurrentlyRenderedActors.Contains(ActorArray[i]))
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
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(ActorArray[i]->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				ActorLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget");
			}
			else
			{
				ActorLocation = ActorArray[i]->GetActorLocation();
			}

			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ViewingActorLocation, ActorLocation);
			const FRotator LookAtAngle = UKismetMathLibrary::ComposeRotators(ViewingActorRotation, UKismetMathLibrary::NegateRotator(LookAtRotation));

			bool bAcceptablePitch = false;
			bool bAcceptableYaw = false;
			bool bAcceptableRoll = false;


			if (GetCameraView() != ECameraView::CV_LockOn)
			{
				bAcceptablePitch = (LookAtAngle.Pitch <= FirstLockAllowedViewAngle.Pitch && LookAtAngle.Pitch >= -FirstLockAllowedViewAngle.Pitch);
				bAcceptableYaw = (LookAtAngle.Yaw <= FirstLockAllowedViewAngle.Yaw && LookAtAngle.Yaw >= -FirstLockAllowedViewAngle.Yaw);
				bAcceptableRoll = (LookAtAngle.Roll <= FirstLockAllowedViewAngle.Roll && LookAtAngle.Roll >= -FirstLockAllowedViewAngle.Roll);
			}
			else
			{
				bAcceptablePitch = (LookAtAngle.Pitch <= MaxAllowedViewAngle.Pitch && LookAtAngle.Pitch >= MinAllowedViewAngle.Pitch);
				bAcceptableYaw = (LookAtAngle.Yaw <= MaxAllowedViewAngle.Yaw && LookAtAngle.Yaw >= MinAllowedViewAngle.Yaw);
				bAcceptableRoll = (LookAtAngle.Roll <= MaxAllowedViewAngle.Roll && LookAtAngle.Roll >= MinAllowedViewAngle.Roll);
				
			}

			if (bAcceptablePitch && bAcceptableYaw && bAcceptableRoll)
			{
				if (ActorTag.IsNone() || ActorArray[i]->ActorHasTag(ActorTag))
				{
					if (ActorArray[i]->GetLastRenderTime() > MinRecentTime)
					{
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
							if (HitResult.GetActor() == ActorArray[i])
							{
								CurrentlyRenderedActors.AddUnique(ActorArray[i]);
							}
						}
					}
				}
			}
		}
	}
}

void ARPGPlayerCameraManager::GetRenderedActorsInViewportCircularSector(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float TargetAngle, float SearchAngleRange, float MinRecentTime)
{

	//Empty any previous entries
	CurrentlyRenderedActors.Empty();

	TArray<AActor*> ActorArray;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), ActorArray);

	//Iterate Over Actors
	for (int i = 0; i < ActorArray.Num(); ++i) // TObjectIterator Itr | 
	{
		if (PlayerCharacter)
		{
			if (ActorArray[i] == PlayerCharacter)
			{
				continue;
			}
		}

		if (ActorArray[i] == LockOnTargetActor) { continue; }

		if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(ActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
		{
			if (HealthComponentRef->IsDead()) { continue; }
		}

		if (!CurrentlyRenderedActors.Contains(ActorArray[i]))
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
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(ActorArray[i]->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				ActorLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget");
			}
			else
			{
				ActorLocation = ActorArray[i]->GetActorLocation();
			}

			const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ViewingActorLocation, ActorLocation);
			const FRotator LookAtAngle = UKismetMathLibrary::ComposeRotators(ViewingActorRotation, UKismetMathLibrary::NegateRotator(LookAtRotation));

			const bool bAcceptablePitch = (LookAtAngle.Pitch <= SwapTargetSearchRange && LookAtAngle.Pitch >= -SwapTargetSearchRange);
			const bool bAcceptableYaw = (LookAtAngle.Yaw <= SwapTargetSearchRange && LookAtAngle.Yaw >= -SwapTargetSearchRange);
			const bool bAcceptableRoll = (LookAtAngle.Roll <= SwapTargetSearchRange && LookAtAngle.Roll >= -SwapTargetSearchRange);

			if (bAcceptablePitch && bAcceptableYaw && bAcceptableRoll)
			{
				const float ActorsAngleFromCentre = GetViewportAngleFromVector2D({-LookAtAngle.Yaw, -LookAtAngle.Pitch});

				if (ActorsAngleFromCentre >= (TargetAngle - (SearchAngleRange * .5f)) && ActorsAngleFromCentre <= (TargetAngle + (SearchAngleRange * .5f)))
				{
					if (ActorTag.IsNone() || ActorArray[i]->ActorHasTag(ActorTag))
					{
						if (ActorArray[i]->GetLastRenderTime() > MinRecentTime)
						{
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
								if (HitResult.GetActor() == ActorArray[i])
								{
									CurrentlyRenderedActors.AddUnique(ActorArray[i]);
								}
							}
						}
					}
				}
			}
		}
	}
}

void ARPGPlayerCameraManager::EnableLockOn()
{
	GetRenderedActorsInView(this, LockOnActorArray, "Targetable");
	if (!LockOnActorArray.IsEmpty())
	{
		FVector StartPoint;
		PlayerCharacter ? StartPoint = PlayerCharacter->GetActorLocation() : StartPoint = GetCameraLocation();

		float ShortestActorDistance = FLT_MAX;

		for (int i = 0; i < LockOnActorArray.Num(); i++)
		{
			if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				if (HealthComponentRef->IsDead()) { continue; }
			}

			float ActorDistance = UKismetMathLibrary::VSize(StartPoint - LockOnActorArray[i]->GetActorLocation());
			if (ActorDistance < ShortestActorDistance)
			{
				ShortestActorDistance = ActorDistance;
				LockOnTargetActor = LockOnActorArray[i];
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
				}
			}
		}
		SetCameraView(ECameraView::CV_LockOn);
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

void ARPGPlayerCameraManager::SwapLockOnTarget(float InYawValue, float InPitchValue)
{
	if (!ViewTargetCameraRef) { return; }
	if (GetCameraView() != ECameraView::CV_LockOn) { return; }
	if (bSwapTargetOnCooldown) { return; }

	const float PitchAndYawSum = FMath::Abs(InYawValue) + FMath::Abs(InPitchValue);

	const float YawSearch = 1.f * (InYawValue / PitchAndYawSum);
	const float PitchSearch = 1.f * (-InPitchValue / PitchAndYawSum);

	const float SearchAngle = GetViewportAngleFromVector2D({YawSearch, PitchSearch});

	GetRenderedActorsInViewportCircularSector(this, LockOnActorArray, "Targetable", SearchAngle, SwapTargetSearchAngleRange, .01f);

	if (!LockOnActorArray.IsEmpty())
	{
		FVector StartPoint;
		PlayerCharacter ? StartPoint = ViewTargetCameraRef->GetComponentLocation() : StartPoint = GetCameraLocation();


		// Closest to target angle
		// Closest to centre of screen

		float ClosestToSearchAngle = FLT_MAX;
		//float ClosestToViewportCentre = FLT_MAX;

		float ShortestActorDistance = FLT_MAX;
		float ClosestToViewportCentre = FLT_MAX;

		for (int i = 0; i < LockOnActorArray.Num(); i++)
		{
			// Don't target dead actors
			if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
			{
				if (HealthComponentRef->IsDead()) { continue; }
			}

			// Get distance between current targeted actor and potential new target
			FVector LockOnActorArrayLocation = FVector::ZeroVector;
			if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
			{
				if (ActorMeshComponent->DoesSocketExist("LockOn_MainTarget")) { LockOnActorArrayLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget"); }
				else { LockOnActorArrayLocation = LockOnActorArray[i]->GetActorLocation(); }
			}
			else
			{
				LockOnActorArrayLocation = LockOnActorArray[i]->GetActorLocation();
			}
			float ActorDistance = UKismetMathLibrary::VSize(StartPoint - LockOnActorArrayLocation);

			FRotator ActorAngle = (UKismetMathLibrary::FindLookAtRotation(ViewTargetCameraRef->GetComponentLocation(), LockOnActorArrayLocation));
			ActorAngle = UKismetMathLibrary::ComposeRotators(PC->GetControlRotation(), UKismetMathLibrary::NegateRotator(ActorAngle));

			float DistanceToSearchAngle = FMath::Abs(SearchAngle - GetViewportAngleFromVector2D({ -ActorAngle.Yaw, -ActorAngle.Pitch }));

			ActorAngle = { FMath::Abs(ActorAngle.Pitch), FMath::Abs(ActorAngle.Yaw), 0.f };
			float ActorAngleSum = FMath::Abs(ActorAngle.Pitch) + FMath::Abs(ActorAngle.Yaw) + FMath::Abs(ActorAngle.Roll);


			
			if (ActorAngleSum < ClosestToViewportCentre)
			{
				ClosestToViewportCentre = ActorAngleSum;
				// ShortestActorDistance = ActorDistance;

				LockOnTargetActor = LockOnActorArray[i];
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
				}
				bSwapTargetOnCooldown = true;

				continue;

			}
			else if (DistanceToSearchAngle <= (SwapTargetPreciseSearchAngleRange * .5f) && DistanceToSearchAngle < ClosestToSearchAngle)
			{
				ClosestToSearchAngle = DistanceToSearchAngle;

				LockOnTargetActor = LockOnActorArray[i];
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
				}
				bSwapTargetOnCooldown = true;

				continue;
			}
		}
		if (bSwapTargetOnCooldown)
		{
			GetWorldTimerManager().SetTimer(SwapTargetCooldownTimerHandle, this, &ARPGPlayerCameraManager::ResetSwapTargetCooldown, GetWorld()->GetDeltaSeconds(), false, SwapCooldown);
			bCanSwapTarget = false;
		}
	}
	else
	{
		GetRenderedActorsInViewportCircularSector(this, LockOnActorArray, "Targetable", SearchAngle, SwapTargetSecondSearchAngleRange, .01f);
		if (!LockOnActorArray.IsEmpty())
		{
			FVector StartPoint;

			if (PlayerCharacter)
			{
				if (UMeshComponent* PlayerCharacterMeshComponent = Cast<UMeshComponent>(PlayerCharacter->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					if (PlayerCharacterMeshComponent->DoesSocketExist("LockOn_MainTarget")) { StartPoint = PlayerCharacterMeshComponent->GetSocketLocation("LockOn_MainTarget"); }
					else { StartPoint = PlayerCharacter->GetActorLocation(); }
				}
				else { StartPoint = PlayerCharacter->GetActorLocation(); }
			}
			else { StartPoint = GetCameraLocation(); }

			float ShortestActorDistance = FLT_MAX;
			float ClosestToViewportCentre = FLT_MAX;

			for (int i = 0; i < LockOnActorArray.Num(); i++)
			{
				// Don't target dead actors
				if (UHealthComponent* HealthComponentRef = Cast<UHealthComponent>(LockOnActorArray[i]->GetComponentByClass(UHealthComponent::StaticClass())))
				{
					if (HealthComponentRef->IsDead()) { continue; }
				}

				// Get distance between current targeted actor and potential new target
				FVector LockOnActorArrayLocation = FVector::ZeroVector;
				if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
				{
					if (ActorMeshComponent->DoesSocketExist("LockOn_MainTarget")) { LockOnActorArrayLocation = ActorMeshComponent->GetSocketLocation("LockOn_MainTarget"); }
					else { LockOnActorArrayLocation = LockOnActorArray[i]->GetActorLocation(); }
				}
				else
				{
					LockOnActorArrayLocation = LockOnActorArray[i]->GetActorLocation();
				}
				float ActorDistance = UKismetMathLibrary::VSize(StartPoint - LockOnActorArrayLocation);

				FRotator ActorAngle = (UKismetMathLibrary::FindLookAtRotation(ViewTargetCameraRef->GetComponentLocation(), LockOnActorArrayLocation));
				ActorAngle = UKismetMathLibrary::ComposeRotators(PC->GetControlRotation(), UKismetMathLibrary::NegateRotator(ActorAngle));
				ActorAngle = { FMath::Abs(ActorAngle.Pitch), FMath::Abs(ActorAngle.Yaw), 0.f };
				float ActorAngleSum = FMath::Abs(ActorAngle.Pitch) + FMath::Abs(ActorAngle.Yaw) + FMath::Abs(ActorAngle.Roll);

				if (ActorDistance < ShortestActorDistance)
				{
					ShortestActorDistance = ActorDistance;

					LockOnTargetActor = LockOnActorArray[i];
					if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
					{
						ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
					}
					bSwapTargetOnCooldown = true;

					continue;
				}
				else if (ActorAngleSum < ClosestToViewportCentre)
				{
					ClosestToViewportCentre = ActorAngleSum;

					LockOnTargetActor = LockOnActorArray[i];
					if (UMeshComponent* ActorMeshComponent = Cast<UMeshComponent>(LockOnTargetActor->GetComponentByClass(UMeshComponent::StaticClass())))
					{
						ActorMeshComponent->DoesSocketExist("LockOn_MainTarget") ? TargetSocket = "LockOn_MainTarget" : TargetSocket;
					}
					bSwapTargetOnCooldown = true;

					continue;
				}
			}
			if (bSwapTargetOnCooldown)
			{
				GetWorldTimerManager().SetTimer(SwapTargetCooldownTimerHandle, this, &ARPGPlayerCameraManager::ResetSwapTargetCooldown, GetWorld()->GetDeltaSeconds(), false, SwapCooldown);
				bCanSwapTarget = false;
			}
		}
	}

	
}

bool ARPGPlayerCameraManager::GetCrosshairTarget(FHitResult& CrosshairResult)
{
	const FVector EndPos = GetCameraLocation() + (GetActorForwardVector() * 10000.f);
	
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

void ARPGPlayerCameraManager::InterpToView()
{
	if (!PlayerCharacter || !ViewTargetCameraRef || !CurrentCameraArrow) { return; }
	
	if (ViewTargetCameraRef->GetRelativeLocation() != CurrentCameraArrow->GetRelativeLocation())
	{
		if (InterpSpeedMap.Contains(CameraViewLastUpdate))
		{
			const FVector InterpVector = UKismetMathLibrary::VInterpTo(ViewTargetCameraRef->GetRelativeLocation(), CurrentCameraArrow->GetRelativeLocation(), GetWorld()->GetDeltaSeconds(), InterpSpeedMap[CameraViewLastUpdate]);
			const FRotator InterpRotator = UKismetMathLibrary::RInterpTo(ViewTargetCameraRef->GetRelativeRotation(), CurrentCameraArrow->GetRelativeRotation(), GetWorld()->GetDeltaSeconds(), InterpSpeedMap[CameraViewLastUpdate]);

			ViewTargetCameraRef->SetRelativeLocationAndRotation(InterpVector, InterpRotator);
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(InterpToViewTimerHandle);
	}
}

float ARPGPlayerCameraManager::GetViewportAngleFromVector2D(FVector2D InVector2D)
{
	FVector2D CentreVector2D { 0.f, 1.f };
	CentreVector2D = UKismetMathLibrary::Normal2D(CentreVector2D);
	FVector2D TargetVector2D = UKismetMathLibrary::Normal2D(InVector2D);
	if (TargetVector2D.X < 0.f) 
	{
		return (360.f - UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D))));
	}
	return UKismetMathLibrary::DegAcos(UKismetMathLibrary::DotProduct2D(CentreVector2D, TargetVector2D) / (UKismetMathLibrary::VSize2D(CentreVector2D) * UKismetMathLibrary::VSize2D(TargetVector2D)));
}


