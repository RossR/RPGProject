// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Weapons/WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	SetRootComponent(WeaponMesh);

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::PlayRandomAttackSound()
{
	if (WeaponInfo.RandomAttackSoundArray.Num() != 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, FString::FromInt(WeaponInfo.RandomAttackSoundArray.Num()));
		const int32 RandomItem = UKismetMathLibrary::RandomIntegerInRange(0, (WeaponInfo.RandomAttackSoundArray.Num() - 1));
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, FString::FromInt(RandomItem));
		if (USoundWave* RandomSound = WeaponInfo.RandomAttackSoundArray[RandomItem])
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), RandomSound, GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AWeaponBase::PlayRandomAttackSound Random sound is nullptr"));
		}
	}
}