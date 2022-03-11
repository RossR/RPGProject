// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/ItemWeapon.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AItemWeapon::AItemWeapon()
{
	// Super::AItemBase();

	// WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	// SetRootComponent(WeaponMesh);

	WeaponTraceStart.Add(CreateDefaultSubobject<USceneComponent>(TEXT("WeaponTraceStart1")));

	WeaponTraceEnd.Add(CreateDefaultSubobject<USceneComponent>(TEXT("WeaponTraceEnd1")));

	WeaponTraceStart[0]->SetupAttachment(RootComponent);
	WeaponTraceEnd[0]->SetupAttachment(RootComponent);

	ItemData = CreateDefaultSubobject<UItemWeaponData>(TEXT("Weapon Data Instance"));

	WeaponData = Cast<UItemWeaponData>(ItemData);
}

// Called when the game starts or when spawned
void AItemWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AItemWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemWeapon::PlayRandomAttackSound()
{
	if (WeaponData)
	{
		if (WeaponData->RandomAttackSoundArray.Num() != 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, FString::FromInt(WeaponInfo.RandomAttackSoundArray.Num()));
			const int32 RandomItem = UKismetMathLibrary::RandomIntegerInRange(0, (WeaponData->RandomAttackSoundArray.Num() - 1));
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Emerald, FString::FromInt(RandomItem));
			if (USoundWave* RandomSound = WeaponData->RandomAttackSoundArray[RandomItem])
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), RandomSound, GetActorLocation());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AItemWeapon::PlayRandomAttackSound Random sound is nullptr"));
			}
		}
	}
}

void AItemWeapon::SetCurrentTraceStart(USceneComponent* _WeaponTraceStart)
{
	CurrentTraceStart = _WeaponTraceStart->GetComponentLocation();
}

void AItemWeapon::SetCurrentTraceEnd(USceneComponent* _WeaponTraceEnd)
{
	CurrentTraceEnd = _WeaponTraceEnd->GetComponentLocation();
}

void AItemWeapon::SetTraceAlpha(int CurrentTraceSectionIndex)
{
	TraceAlpha = (float)CurrentTraceSectionIndex / (float)TotalTraceSections;
}

void AItemWeapon::SetCurrentTraceSection()
{
	CurrentTraceSection = (CurrentTraceStart * TraceAlpha) + (CurrentTraceEnd * (1.0f - TraceAlpha));
}

void AItemWeapon::SetLastTraceSection(int Index)
{
	LastTraceSection = (LastTraceStart[Index] * TraceAlpha) + (LastTraceEnd[Index] * (1 - TraceAlpha));
}

void AItemWeapon::SetTraceOrientation(USceneComponent* _WeaponTraceStart)
{
	TraceOrientation = _WeaponTraceStart->GetRelativeRotation() + ItemMesh->GetComponentRotation();
}

void AItemWeapon::SetTraceResults(TArray<FHitResult> OutHits, bool TraceReturnValue)
{
	if (TraceReturnValue)
	{
		for (int i = 0; i < OutHits.Num(); i++)
		{
			WeaponTraceHitResult.Add(OutHits[i]);

			bWeaponTraceHasHit = true;
		}
	}
}

void AItemWeapon::CheckIfFirstTrace()
{
	if (!bIsFirstTrace)
	{
		TraceBetweenTraces();
	}
}

void AItemWeapon::SetLastTraceVariables(int Index)
{
	LastTraceStart.SetNum(WeaponTraceStart.Num(), true);
	LastTraceEnd.SetNum(WeaponTraceEnd.Num(), true);
	LastTraceStart[Index] = CurrentTraceStart;
	LastTraceEnd[Index] =CurrentTraceEnd;
}