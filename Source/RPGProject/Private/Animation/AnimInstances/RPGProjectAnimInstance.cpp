// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimInstances/RPGProjectAnimInstance.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Controllers/RPGProjectPlayerController.h"
#include "AIController.h"

URPGProjectAnimInstance::URPGProjectAnimInstance()
{

}

void URPGProjectAnimInstance::NativeBeginPlay()
{
	PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(GetOwningActor());
	if (PlayerCharacter)
	{
		PC = Cast<ARPGProjectPlayerController>(PlayerCharacter->GetController());
		AIPC = Cast<AAIController>(PlayerCharacter->GetController());
	}
}

void URPGProjectAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	UpdateCurves();


}

void URPGProjectAnimInstance::UpdateCurves()
{
	if (GetCurveValue("DisableAngleBodyToTarget") >= 1.f)
	{
		bDisableAngleBodyToTarget = true;
	}
	else
	{
		bDisableAngleBodyToTarget = false;
	}

	if (GetCurveValue("DisableAngleHeadToTarget") >= 1.f)
	{
		DisableAngleHeadToTarget = true;
	}
	else
	{
		DisableAngleHeadToTarget = false;
	}
}
