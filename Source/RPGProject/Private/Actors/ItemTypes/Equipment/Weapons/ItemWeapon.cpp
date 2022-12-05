// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Actors/Components/StaminaComponent.h"
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

	WeaponTraceStartMap.Add(0);
	WeaponTraceStartMap[0] = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponTraceStart_1"));

	WeaponTraceEndMap.Add(0);
	WeaponTraceEndMap[0] = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponTraceEnd_1"));

	//WeaponTraceEndMap.Emplace(0, CreateDefaultSubobject<USceneComponent>(TEXT("WeaponTraceEnd_1")));
	WeaponVFXMap.Add(0);
	WeaponVFXMap[0] = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponVFX_1"));

	WeaponTraceStartMap[0]->SetupAttachment(RootComponent);
	WeaponTraceEndMap[0]->SetupAttachment(RootComponent);
	WeaponVFXMap[0]->SetupAttachment(RootComponent);

	ItemDataDefault = CreateDefaultSubobject<UItemWeaponData>(TEXT("Weapon Data Default"));
	//WeaponData = CreateDefaultSubobject<UItemWeaponData>(TEXT("Weapon Data"));

	AttackBlockerBoxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Attack Blocker Box Volume"));
	AttackBlockerBoxVolume->SetupAttachment(ItemMesh);
	AttackBlockerBoxVolume->SetCollisionProfileName("AttackBlocker");
	AttackBlockerBoxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttackBlockerCapsuleVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Attack Blocker Capsule Volume"));
	AttackBlockerCapsuleVolume->SetupAttachment(ItemMesh);
	AttackBlockerCapsuleVolume->SetCollisionProfileName("AttackBlocker");
	AttackBlockerCapsuleVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AItemWeapon::PostInitProperties()
{
	Super::PostInitProperties();

	switch (AttackBlockerType)
	{
	case EAttackBlockerType::None:
		AttackBlockerBoxVolume->SetVisibility(false);
		AttackBlockerCapsuleVolume->SetVisibility(false);
		break;


	case EAttackBlockerType::Box:
		AttackBlockerCapsuleVolume->SetVisibility(false);
		AttackBlockerBoxVolume->SetVisibility(true);
		break;

	case EAttackBlockerType::Capsule:
		AttackBlockerBoxVolume->SetVisibility(false);
		AttackBlockerCapsuleVolume->SetVisibility(true);
		break;

	default:
		break;
	}

	//AttackBlockerVolume = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), TEXT("Attack Blocker Volume"));

	/*if (AttackBlockerVolumeClass == UCapsuleComponent::StaticClass())
	{
		AttackBlockerVolume = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), TEXT("Attack Blocker Volume"), EObjectFlags::RF_NoFlags);
	}*/

	//AttackBlockerVolume = NewObject<UCapsuleComponent>(this, UCapsuleComponent::StaticClass(), TEXT("Attack Blocker Volume"), EObjectFlags::RF_NoFlags);

	//AttackBlockerVolume = Cast<UCapsuleComponent>(CreateDefaultSubobject(TEXT("Attack Blocker Volume"), /**AttackBlockerVolumeClass*/UCapsuleComponent::StaticClass(), /**AttackBlockerVolumeClass*/UCapsuleComponent::StaticClass(), true, false, false));

	//if (AttackBlockerVolumeClass)
	//{
	//	AttackBlockerVolume = NewObject<UPrimitiveComponent>(this, AttackBlockerVolumeClass->StaticClass(), TEXT("Item Data"), EObjectFlags::RF_NoFlags);
	//	//AttackBlockerVolume = Cast<UShapeComponent>(CreateDefaultSubobject(TEXT("InitialWeaponComponent"), *AttackBlockerVolumeClass, *AttackBlockerVolumeClass, true, false, false));
	//	//AttackBlockerBoxVolume = CreateDefaultSubobject(FName SubobjectFName, UClass * ReturnType, UClass * ClassToCreateByDefault, bool bIsRequired, bool bAbstract, bool bIsTransient);
	//}
}

// Called when the game starts or when spawned
void AItemWeapon::BeginPlay()
{
	Super::BeginPlay();

	SetItemData(ItemDataDefault);

	IgnoredActorArray.Add(this);
	if (GetParentActor()) { IgnoredActorArray.Add(GetParentActor()); }

	if (bAutoSetMeleeTrailVFXWidth)
	{
		SetMeleeTrailVFXWidth();
	}

}

// Called every frame
void AItemWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemWeapon::SetItemData(UItemData* NewItemData)
{
	if (UItemWeaponData* NewItemWeaponData = Cast<UItemWeaponData>(NewItemData))
	{
		WeaponData = NULL;
		WeaponData = NewObject<UItemWeaponData>(this, UItemWeaponData::StaticClass(), TEXT("Equipment Data"), EObjectFlags::RF_NoFlags, NewItemWeaponData);
	}
}

void AItemWeapon::PlayAttackSFX()
{
	if (WeaponData)
	{
		if (WeaponData->AttackSFX)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponData->AttackSFX, GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AItemWeapon::PlayAttackSound AttackSoundCue is nullptr."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AItemWeapon::PlayAttackSound WeaponData is nullptr."));
	}
}

void AItemWeapon::WeaponTrace(int SingleWeaponTraceStart)
{
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

	TArray<FHitResult> InitialOutHits;
	if (CurrentAttackType == EAttackType::AT_LightAttack || CurrentAttackType == EAttackType::AT_LightFinisher)
	{
		if (LightAttackInfo.Find(AttackComboCount))
		{
			CurrentTraceType = LightAttackInfo[AttackComboCount].AttackTraceType;
		}
		else if (AttackComboCount >= 1)
		{
			if (LightAttackInfo.Find(0))
			{
				CurrentTraceType = LightAttackInfo[0].AttackTraceType;
			}
		}
		else
		{
			CurrentTraceType = ETraceType::None;
		}
	}
	else if (CurrentAttackType == EAttackType::AT_HeavyAttack || CurrentAttackType == EAttackType::AT_HeavyFinisher)
	{
		if (HeavyAttackInfo.Find(AttackComboCount))
		{
			CurrentTraceType = HeavyAttackInfo[AttackComboCount].AttackTraceType;
		}
		else if (AttackComboCount >= 1)
		{
			if (HeavyAttackInfo.Find(0))
			{
				CurrentTraceType = HeavyAttackInfo[0].AttackTraceType;
			}
		}
		else
		{
			CurrentTraceType = ETraceType::None;
		}
	}

	// if true, trace along all points in the trace arrays
	if (SingleWeaponTraceStart == -1)
	{
		for (int i = 0; i < WeaponTraceStartMap.Num(); i++)
		{
			SetTraceOrientation(WeaponTraceStartMap[i]);
			SetCurrentTraceStart(WeaponTraceStartMap[i]);
			SetCurrentTraceEnd(WeaponTraceEndMap[i]);

			switch (CurrentTraceType)
			{
			case ETraceType::Multi_Line:
				bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			case ETraceType::Multi_Box:
				bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			case ETraceType::Multi_Sphere:
				bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
				break;

			default:
				break;
			}

			for (int Counter = 0; Counter < InitialOutHits.Num(); Counter++)
			{
				FString HitActorString = InitialOutHits[Counter].GetActor()->GetName();
				FString HitBoneString = InitialOutHits[Counter].BoneName.ToString();
				bool bBlockingHit = InitialOutHits[Counter].bBlockingHit;
				FString BlockingHitString = bBlockingHit ? "true" : "false";
				//UE_LOG(LogTemp, Warning, TEXT("SWTS InitialOutHits[%s]: bBlockingHit = %s, HitActor = %s, HitBone = %s"), *FString::FromInt(Counter), *BlockingHitString, *HitActorString, *HitBoneString);
				if (Counter == (InitialOutHits.Num() - 1))
				{
					//UE_LOG(LogTemp, Warning, TEXT("-----"));
				}
			}

			AddToWeaponTraceHitResults(InitialOutHits);

			CheckIfFirstTrace(i);
			SetLastTraceVariables(WeaponTraceStartMap.Num(), WeaponTraceEndMap.Num(), i);
		}
	}
	else
	{
		SetTraceOrientation(WeaponTraceStartMap[SingleWeaponTraceStart]);
		SetCurrentTraceStart(WeaponTraceStartMap[SingleWeaponTraceStart]);
		SetCurrentTraceEnd(WeaponTraceEndMap[SingleWeaponTraceStart]);

		switch (CurrentTraceType)
		{
		case ETraceType::Multi_Line:
			bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Box:
			bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Sphere:
			bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), CurrentTraceStart, CurrentTraceEnd, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, InitialOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		default:
			break;
		}

		for (int Counter = 0; Counter < InitialOutHits.Num(); Counter++)
		{
			FString HitActorString = InitialOutHits[Counter].GetActor()->GetName();
			FString HitBoneString = InitialOutHits[Counter].BoneName.ToString();
			bool bBlockingHit = InitialOutHits[Counter].bBlockingHit;
			FString BlockingHitString = bBlockingHit ? "true" : "false";
			UE_LOG(LogTemp, Warning, TEXT("InitialOutHits[%s]: bBlockingHit = %s, HitActor = %s, HitBone = %s"), *FString::FromInt(Counter), *BlockingHitString, *HitActorString, *HitBoneString);
			if (Counter == (InitialOutHits.Num() - 1))
			{
				UE_LOG(LogTemp, Warning, TEXT("-----"));
			}
		}

		AddToWeaponTraceHitResults(InitialOutHits);

		CheckIfFirstTrace();
		SetLastTraceVariables(WeaponTraceStartMap.Num(), WeaponTraceEndMap.Num());
	}
	SetIsFirstTrace(false);
}

void AItemWeapon::TraceBetweenTraces(int TraceIndex)
{
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

	TArray<FHitResult> BetweenTraceOutHits;

	for (int i = 0; i < TotalTraceSections; i++)
	{
		SetTraceAlpha(i);
		SetLastTraceSection(TraceIndex);
		SetCurrentTraceSection();

		switch (CurrentTraceType)
		{
		case ETraceType::Multi_Line:
			bWeaponTraceHasHit = UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, BetweenTraceOutHits, true,FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Box:
			bWeaponTraceHasHit = UKismetSystemLibrary::BoxTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, TraceBoxHalfSize, TraceOrientation, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, BetweenTraceOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		case ETraceType::Multi_Sphere:
			bWeaponTraceHasHit = UKismetSystemLibrary::SphereTraceMultiForObjects(GetWorld(), LastTraceSection, CurrentTraceSection, TraceRadius, ObjectTypesArray, bTraceComplex, IgnoredActorArray, DebugTrace, BetweenTraceOutHits, true, FLinearColor::Red, FLinearColor::Green, 3.0f);
			break;

		default:
			break;
		}

		for (int Counter = 0; Counter < BetweenTraceOutHits.Num(); Counter++)
		{
			FString HitActorString = BetweenTraceOutHits[Counter].GetActor()->GetName();
			FString HitBoneString = BetweenTraceOutHits[Counter].BoneName.ToString();
			bool bBlockingHit = BetweenTraceOutHits[Counter].bBlockingHit;
			FString BlockingHitString = bBlockingHit ? "true" : "false";
			UE_LOG(LogTemp, Warning, TEXT("BetweenTraceOutHits[%s]: bBlockingHit = %s, HitActor = %s, HitBone = %s"), *FString::FromInt(Counter), *BlockingHitString, *HitActorString, *HitBoneString);
			if (Counter == (BetweenTraceOutHits.Num() - 1))
			{
				UE_LOG(LogTemp, Warning, TEXT("-----"));
			}
		}

		AddToWeaponTraceHitResults(BetweenTraceOutHits);
	}
}

void AItemWeapon::EnableWeaponVFX(int Index)
{
	if (Index == -1)
	{
		TArray<UNiagaraComponent*> WeaponVFXArray;
		WeaponVFXMap.GenerateValueArray(WeaponVFXArray);
		
		for (int i = 0; i < WeaponVFXArray.Num(); i++)
		{
			if (WeaponVFXArray[i] != nullptr) { WeaponVFXArray[i]->SetNiagaraVariableBool("User.bEnableVFX", true); }
		}
	}
	else if (WeaponVFXMap.Contains(Index))
	{
		if (WeaponVFXMap.Contains(Index)) { WeaponVFXMap[Index]->SetNiagaraVariableBool("User.bEnableVFX", true); }
	}
}

void AItemWeapon::DisableWeaponVFX(int Index)
{
	if (Index == -1)
	{
		TArray<UNiagaraComponent*> WeaponVFXArray;
		WeaponVFXMap.GenerateValueArray(WeaponVFXArray);

		for (int i = 0; i < WeaponVFXArray.Num(); i++)
		{
			if (WeaponVFXArray[i] != nullptr) { WeaponVFXArray[i]->SetNiagaraVariableBool("User.bEnableVFX", false); }
		}
	}
	else if (WeaponVFXMap.Contains(Index))
	{
		if (WeaponVFXMap.Contains(Index)) { WeaponVFXMap[Index]->SetNiagaraVariableBool("User.bEnableVFX", false); }
	}
}

UShapeComponent* AItemWeapon::GetAttackBlockerVolume()
{
	switch (AttackBlockerType)
	{
	case EAttackBlockerType::Box:
		return AttackBlockerBoxVolume;
		break;

	case EAttackBlockerType::Capsule:
		return AttackBlockerCapsuleVolume;
		break;

	default:
		break;
	}
	return nullptr;
}

void AItemWeapon::SetAttackBlockerCollisionEnabled(ECollisionEnabled::Type NewType)
{
	switch (AttackBlockerType)
	{
	case EAttackBlockerType::Box:
		AttackBlockerBoxVolume->SetCollisionEnabled(NewType);
		break;

	case EAttackBlockerType::Capsule:
		AttackBlockerCapsuleVolume->SetCollisionEnabled(NewType);
		break;

	default:
		break;
	}
}

void AItemWeapon::AttackBlocked(FWeaponAttackInfo& BlockedAttackInfo, AItemWeapon* InItemWeapon, EAttackType BlockedAttackType, AProjectileActor* InProjectileActor)
{
	UItemWeaponData* InWeaponData = GetItemWeaponData();

	float NewDurability = FLT_MAX;
	float BlockStaminaDamage = 0.f;

	if (InWeaponData)
	{
		NewDurability = InWeaponData->ItemCurrentDurability;
		
		if (InItemWeapon)
		{
			BlockStaminaDamage = BlockedAttackInfo.StaminaDamage;

			/*switch (BlockedAttackType)
			{
			case EAttackType::AT_LightAttack:
				BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * 1.f;
				break;

			case EAttackType::AT_HeavyAttack:
				BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * 1.25f;
				break;

			case EAttackType::AT_LightFinisher:
				BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * 1.f;
				break;

			case EAttackType::AT_HeavyFinisher:
				BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * 1.25f;
				break;

			default:
				BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * 1.f;
				break;
			}*/
		}
		else if (InProjectileActor)
		{
			BlockStaminaDamage = InWeaponData->WeaponBlockStaminaDamage * .25f;
		}
	}


	if (InWeaponData)
	{
		switch (InWeaponData->WeaponType)
		{
		case EWeaponType::WT_Unarmed:
			break;
		case EWeaponType::WT_Item:
			break;
		case EWeaponType::WT_1HandDagger:
			break;
		case EWeaponType::WT_1HandMace:
			break;
		case EWeaponType::WT_1HandPistol:
			break;
		case EWeaponType::WT_1HandSpear:
			break;
		case EWeaponType::WT_1HandSword:
			break;
		case EWeaponType::WT_2HandAxe:
			break;
		case EWeaponType::WT_2HandBow:
			break;
		case EWeaponType::WT_2HandCrossbow:
			break;
		case EWeaponType::WT_2HandShooter:
			break;
		case EWeaponType::WT_2HandSpear:
			break;
		case EWeaponType::WT_2HandStaff:
			break;
		case EWeaponType::WT_2HandSword:
			break;
		case EWeaponType::WT_1HandShield:
			NewDurability -= 0.5f;
			break;
		default:
			break;
		}
	}

	if (NewDurability != FLT_MAX)
	{
		SetDurability(NewDurability);
	}

	if (AActor* ParentRef = GetParentActor())
	{
		if (UStaminaComponent* ParentStaminaComponentRef = Cast<UStaminaComponent>(ParentRef->GetComponentByClass(UStaminaComponent::StaticClass())))
		{
			ParentStaminaComponentRef->ReduceCurrentStamina(BlockStaminaDamage);
		}
	}
}

void AItemWeapon::UpdateCurves()
{
	Super::UpdateCurves();
}

void AItemWeapon::AddToWeaponTraceHitResults(TArray<FHitResult> OutHits)
{
	for (int i = 0; i < OutHits.Num(); i++)
	{
		if (OutHits[i].bBlockingHit)
		{
			WeaponTraceHitResult.Add(OutHits[i]);

			bWeaponTraceHasHit = true;
		}
	}
}

void AItemWeapon::SetLastTraceVariables(int LastTraceStartArraySize, int LastTraceEndArraySize, int Index)
{
	LastTraceStart.SetNum(LastTraceStartArraySize, true);
	LastTraceEnd.SetNum(LastTraceEndArraySize, true);
	LastTraceStart[Index] = CurrentTraceStart;
	LastTraceEnd[Index] =CurrentTraceEnd;
}

void AItemWeapon::SetMeleeTrailVFXWidth()
{
	if (!WeaponVFXMap.Contains(0)) { return; }
	if (!WeaponTraceStartMap.Contains(0)) { return; }
	if (!WeaponTraceEndMap.Contains(0)) { return; }

	const float TrailWidth = UKismetMathLibrary::Vector_Distance(WeaponTraceStartMap[0]->GetRelativeLocation(), WeaponTraceEndMap[0]->GetRelativeLocation()) + TraceBoxHalfSize.X + TraceRadius;
	WeaponVFXMap[0]->SetVariableFloat("TrailWidth", TrailWidth);
}

