// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/ItemWeapon.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int> CVarDisplayWeaponTrace
(
	TEXT("RPGProject.PlayerCharacter.Debug.DisplayWeaponTrace"),
	0,
	TEXT("		0: Do not display weapon trace.\n")
	TEXT("		1: Display weapon trace for one frame. \n")
	TEXT("		2: Display weapon trace for duration. \n ")
	TEXT("		3: Display weapon trace indefinitely. \n "),
	ECVF_Default
);

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

	IgnoredActorArray.Add(this);
	if (GetParentActor()) { IgnoredActorArray.Add(GetParentActor()); }

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

void AItemWeapon::WeaponTrace(ETraceType InTraceType, int SingleWeaponTraceStart)
{
	EDrawDebugTrace::Type DebugTrace = EDrawDebugTrace::None;
	switch (CVarDisplayWeaponTrace->GetInt())
	{
	case 0:
		DebugTrace = EDrawDebugTrace::None;
		break;

	case 1:
		DebugTrace = EDrawDebugTrace::ForOneFrame;
		break;

	case 2:
		DebugTrace = EDrawDebugTrace::ForDuration;
		break;

	case 3:
		DebugTrace = EDrawDebugTrace::Persistent;
		break;
	}

	// if true, trace along all points in the trace arrays
	if (SingleWeaponTraceStart == -1)
	{
		for (int i = 0; i < WeaponTraceStart.Num(); i++)
		{
			SetTraceOrientation(WeaponTraceStart[i]);
			SetCurrentTraceStart(WeaponTraceStart[i]);
			SetCurrentTraceEnd(WeaponTraceEnd[i]);

			switch (TraceType)
			{
			case ETraceType::Multi_Line:
				bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			case ETraceType::Multi_Box:
				bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			case ETraceType::Multi_Sphere:
				bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			default:
				break;
			}

			CheckIfFirstTrace(i);
			SetLastTraceVariables(i);
		}
	}
	else
	{
		SetTraceOrientation(WeaponTraceStart[SingleWeaponTraceStart]);
		SetCurrentTraceStart(WeaponTraceStart[SingleWeaponTraceStart]);
		SetCurrentTraceEnd(WeaponTraceEnd[SingleWeaponTraceStart]);

		switch (TraceType)
		{
		case ETraceType::Multi_Line:
			bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Box:
			bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Sphere:
			bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		default:
			break;
		}

		CheckIfFirstTrace();
		SetLastTraceVariables();
	}
	SetIsFirstTrace(false);
}

void AItemWeapon::TraceBetweenTraces(int TraceIndex)
{
	EDrawDebugTrace::Type DebugTrace;
	switch (CVarDisplayWeaponTrace->GetInt())
	{
	case 0:
		DebugTrace = EDrawDebugTrace::None;
		break;

	case 1:
		DebugTrace = EDrawDebugTrace::ForOneFrame;
		break;

	case 2:
		DebugTrace = EDrawDebugTrace::ForDuration;
		break;

	case 3:
		DebugTrace = EDrawDebugTrace::Persistent;
		break;
	default:
		DebugTrace = EDrawDebugTrace::None;
		break;
	}

	for (int i = 0; i < TotalTraceSections; i++)
	{
		SetTraceAlpha(i);
		SetLastTraceSection(TraceIndex);
		SetCurrentTraceSection();

		switch (TraceType)
		{
		case ETraceType::Multi_Line:
			bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true,FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Box:
			bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Sphere:
			bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, WeaponTraceHitResult, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		default:
			break;
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

void AItemWeapon::CheckIfFirstTrace(int TraceIndex)
{
	if (!bIsFirstTrace)
	{
		TraceBetweenTraces(TraceIndex);
	}
}

void AItemWeapon::SetLastTraceVariables(int Index)
{
	LastTraceStart.SetNum(WeaponTraceStart.Num(), true);
	LastTraceEnd.SetNum(WeaponTraceEnd.Num(), true);
	LastTraceStart[Index] = CurrentTraceStart;
	LastTraceEnd[Index] =CurrentTraceEnd;
}