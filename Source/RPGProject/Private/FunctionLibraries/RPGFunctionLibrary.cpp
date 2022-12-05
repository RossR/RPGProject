// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/RPGFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/PlayerCameraManager.h"

void URPGFunctionLibrary::GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float MinRecentTime, float AllowedViewAngle)
{
	//Empty any previous entries
	CurrentlyRenderedActors.Empty();

	//Iterate Over Actors
	for (TObjectIterator<AActor> Itr; Itr; ++Itr)
	{
		float ViewingActorRotation = 0.f;
		if (APlayerCameraManager* CameraManager = Cast<APlayerCameraManager>(ViewingActor))
		{
			ViewingActorRotation = CameraManager->GetCameraRotation().Yaw;
		}
		else
		{
			ViewingActorRotation = ViewingActor->GetActorRotation().Yaw;
		}
		float LookAtRotation = UKismetMathLibrary::FindLookAtRotation(ViewingActor->GetActorLocation(), Itr->GetActorLocation()).Yaw;
		float LookAtAngle = ViewingActorRotation - LookAtRotation;

		if (LookAtAngle <= AllowedViewAngle || LookAtAngle >= (AllowedViewAngle * -1.f))
		{
			if (ActorTag.IsNone() || Itr->ActorHasTag(ActorTag))
			{
				if (Itr->GetLastRenderTime() > MinRecentTime)
				{
					CurrentlyRenderedActors.Add(*Itr);
				}
			}
		}
	}
}
