// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Actors/Components/ProjectileSpawnerComponent.h"
#include "Actors/ProjectileActor.h"
#include "CableComponent.h"

AItemWeaponRanged::AItemWeaponRanged()
{
	ItemDataDefault = CreateDefaultSubobject<UItemWeaponRangedData>(TEXT("Weapon Ranged Data Default"));
	//WeaponRangedData = CreateDefaultSubobject<UItemWeaponRangedData>(TEXT("Weapon Ranged Data")); 

	BowstringOriginPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Bowstring Origin Position"));
	BowstringOriginPosition->SetupAttachment(RootComponent);

	BowstringPullPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Bowstring Pull Point"));
	BowstringPullPoint->SetupAttachment(BowstringOriginPosition);

	BowstringBottom = CreateDefaultSubobject<UCableComponent>(TEXT("Bowstring Bottom"));
	BowstringBottom->SetupAttachment(ItemMesh, "Bowstring_Bottom");
	BowstringBottom->AttachEndTo.ComponentProperty = "BowstringPullPoint";
	BowstringBottom->EndLocation = { 0.f, 0.f, 0.f };
	BowstringBottom->CableLength = 50.f;
	BowstringBottom->NumSegments = 1.f;
	BowstringBottom->CableWidth = 0.5f;
	BowstringBottom->NumSides = 16.f;
	BowstringBottom->SetGenerateOverlapEvents(false);

	BowstringTop = CreateDefaultSubobject<UCableComponent>(TEXT("Bowstring Top"));
	BowstringTop->SetupAttachment(ItemMesh, "Bowstring_Top");
	BowstringTop->AttachEndTo.ComponentProperty = "BowstringPullPoint";
	BowstringTop->EndLocation = { 0.f, 0.f, 0.f };
	BowstringTop->CableLength = 50.f;
	BowstringTop->NumSegments = 1.f;
	BowstringTop->CableWidth = 0.5f;
	BowstringTop->NumSides = 16.f;
	BowstringTop->SetGenerateOverlapEvents(false);

	ProjectileAimPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Aim Point"));
	ProjectileAimPoint->SetupAttachment(BowstringOriginPosition);

	ProjectileChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("Projectile Actor"));
	ProjectileChildActorComponent->SetupAttachment(BowstringPullPoint);
	//ProjectileChildActorComponent->SetChildActorClass(GetItemWeaponRangedData()->ProjectileInUse.Get());
	

	ProjectileSpawnerComponent = CreateDefaultSubobject<UProjectileSpawnerComponent>(TEXT("Projectile Spawner Component"));
}

void AItemWeaponRanged::BeginPlay()
{
	Super::BeginPlay();

	SetItemData(ItemDataDefault);
	
	ProjectileChildActorComponent->SetChildActorClass(GetItemWeaponRangedData()->ProjectileInUse.Get());
	ProjectileChildActorRef = Cast<AProjectileActor>(ProjectileChildActorComponent->GetChildActor());

	if (ProjectileChildActorRef)
	{
		ProjectileChildActorRef->SetEnableProjectile(false);
		ProjectileChildActorRef->GetProjectileMovementComponent()->Deactivate();
		ProjectileChildActorRef->GetProjectileMovementComponent()->Velocity = { 0.f, 0.f, 0.f };
		ProjectileChildActorRef->GetProjectileMovementComponent()->InitialSpeed = 0.f;

		ProjectileChildActorRef->SetActorEnableCollision(false);

		if (ProjectileChildActorRef->GetProjectileMesh()->DoesSocketExist(ProjectileChildActorRef->GetProjectileStartSocket()) && ProjectileChildActorRef->GetProjectileMesh()->DoesSocketExist(ProjectileChildActorRef->GetProjectileEndSocket()))
		{
			FVector ProjectileStart = ProjectileChildActorRef->GetProjectileMesh()->GetSocketTransform(ProjectileChildActorRef->GetProjectileStartSocket()).GetLocation();
			FVector ProjectileEnd = ProjectileChildActorRef->GetProjectileMesh()->GetSocketTransform(ProjectileChildActorRef->GetProjectileEndSocket()).GetLocation();

			FVector ProjectileDisplacement = UKismetMathLibrary::Subtract_VectorVector(ProjectileEnd, ProjectileStart);

			ProjectileChildActorComponent->SetWorldLocation(ProjectileChildActorComponent->GetComponentLocation() + ProjectileDisplacement);
		}
	}
}

void AItemWeaponRanged::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateCurves();

	if (HoldBowstringCurve >= 1.f && EnableFireCurve >= 1.f)
	{
		if (ProjectileSpawnerComponent && GetParentActor())
		{
			ProjectileSpawnerComponent->SetProjectileFiringAngle(GetParentActor()->GetActorRotation());
		}
	}

	if (GetItemWeaponRangedData()->AmmoInMagazine > 0.f && ProjectileChildActorRef->IsHidden())
	{
		ProjectileChildActorRef->SetActorHiddenInGame(false);
	}
	else if (GetItemWeaponRangedData()->AmmoInMagazine <= 0.f && !ProjectileChildActorRef->IsHidden())
	{
		ProjectileChildActorRef->SetActorHiddenInGame(true);
	}

	if (GetItemWeaponRangedData()->WeaponType == EWeaponType::WT_2HandBow)
	{
		UpdateProjectileRotation();
		UpdateBowstringPosition();
		
		/*if (bCanFire && EnableFireCurve >= 1.f && GetItemWeaponRangedData()->AmmoInMagazine > 0.f)
		{
			if (BowstringPullPoint->GetRelativeLocation() == FVector{ 0.f, 0.f, 0.f } )
			{
				if (GetParentActor())
				{
					if (UCombatComponent* CombatComponentRef = Cast<UCombatComponent>(GetParentActor()->GetComponentByClass(UCombatComponent::StaticClass())))
					{
						FireProjectile(CombatComponentRef->GetAimAtCrosshair());
					}
				}
				
			}
		}*/
	}
}

void AItemWeaponRanged::SetItemData(UItemData* NewItemData)
{
	if (UItemWeaponRangedData* NewItemWeaponRangedData = Cast<UItemWeaponRangedData>(NewItemData))
	{
		WeaponRangedData = NULL;
		WeaponRangedData = NewObject<UItemWeaponRangedData>(this, UItemWeaponRangedData::StaticClass(), TEXT("Equipment Data"), EObjectFlags::RF_NoFlags, NewItemWeaponRangedData);
	}
}

void AItemWeaponRanged::FireProjectile(bool bAimAtCrosshair)
{
	UE_LOG(LogTemp, Warning, TEXT("AItemWeaponRanged::FireProjectile Called."));

	if (!ProjectileSpawnerComponent) { return; }

	float ProjectileStrength = 0.f;

	if (bEnableDrawSweetSpotCurve)
	{
		ProjectileStrength = ProjectileStrengthScaleCurve * SweetSpotDamageMultiplier;
	}
	else
	{
		ProjectileStrength = ProjectileStrengthScaleCurve;
	}

	if (ProjectileSpawnerComponent->SpawnProjectile(GetItemWeaponRangedData()->ProjectileInUse.Get(), bAimAtCrosshair, ProjectileStrength))
	{
		GetItemWeaponRangedData()->AmmoInMagazine -= 1.f;
		//GetItemWeaponRangedData()->CurrentAmmo -= 1.f;

		SetCanFire(false);

		if (!GetWorldTimerManager().TimerExists(FireProjectileCooldownTimerHandle))
		{
			GetWorldTimerManager().SetTimer(FireProjectileCooldownTimerHandle, this, &AItemWeaponRanged::FireProjectileCooldown, GetWorld()->GetDeltaSeconds(), false, WeaponRangedData->FireRate);
		}
	}
}

void AItemWeaponRanged::ReloadWeapon(EReloadType InReloadType)
{
	UItemWeaponRangedData* RangedWeaponData = GetItemWeaponRangedData();
	const int AmmoNeededToFillMagazine = RangedWeaponData->MagazineCapacity - RangedWeaponData->AmmoInMagazine;

	if (AmmoNeededToFillMagazine <= 0.f) { return; }
	//if (RangedWeaponData->CurrentAmmo - RangedWeaponData->AmmoInMagazine) { return; }

	switch (InReloadType)
	{
	case EReloadType::Single_Reload:
		if (RangedWeaponData->CurrentAmmo >= 1.f)
		{
			RangedWeaponData->AmmoInMagazine += 1.f;
			RangedWeaponData->CurrentAmmo -= 1.f;
		}
		break;

	case EReloadType::Full_Reload:
		if (RangedWeaponData->CurrentAmmo >= AmmoNeededToFillMagazine)
		{
			RangedWeaponData->AmmoInMagazine += AmmoNeededToFillMagazine;
			RangedWeaponData->CurrentAmmo -= AmmoNeededToFillMagazine;
		}
		else if (RangedWeaponData->CurrentAmmo > 0.f)
		{
			RangedWeaponData->AmmoInMagazine += RangedWeaponData->CurrentAmmo;
			RangedWeaponData->CurrentAmmo -= RangedWeaponData->CurrentAmmo;
		}
		break;

	default:
		break;
	}
}

void AItemWeaponRanged::UnloadWeapon()
{
	GetItemWeaponRangedData()->CurrentAmmo += GetItemWeaponRangedData()->AmmoInMagazine;
	GetItemWeaponRangedData()->AmmoInMagazine = 0.f;
}

void AItemWeaponRanged::AttachAmmunitionToHand(USkeletalMeshComponent* MeshComp)
{
	if (!ProjectileChildActorRef) { return; }
	if (!MeshComp) { return; }

	if (URPGProjectAnimInstance* RPGProjectAnimInstance = Cast<URPGProjectAnimInstance>(MeshComp->GetAnimInstance()))
	{
		ProjectileChildActorRef->AttachToComponent(MeshComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Bowstring_Pull");

		if (ProjectileChildActorRef->GetProjectileMesh()->DoesSocketExist(ProjectileChildActorRef->GetProjectileStartSocket()) && ProjectileChildActorRef->GetProjectileMesh()->DoesSocketExist(ProjectileChildActorRef->GetProjectileEndSocket()))
		{
			FVector ProjectileStart = ProjectileChildActorRef->GetProjectileMesh()->GetSocketTransform(ProjectileChildActorRef->GetProjectileStartSocket()).GetLocation();
			FVector ProjectileEnd = ProjectileChildActorRef->GetProjectileMesh()->GetSocketTransform(ProjectileChildActorRef->GetProjectileEndSocket()).GetLocation();

			FVector ProjectileDisplacement = UKismetMathLibrary::Subtract_VectorVector(ProjectileEnd, ProjectileStart);

			ProjectileChildActorRef->SetActorLocation(MeshComp->GetSocketLocation("Bowstring_Pull") + ProjectileDisplacement);
		}
	}
}

void AItemWeaponRanged::AttachAmmunitionToWeapon()
{
	if (!ProjectileChildActorRef) { return; }

	switch (GetItemWeaponRangedData()->WeaponType)
	{
	case EWeaponType::WT_1HandPistol:
		break;
	case EWeaponType::WT_2HandBow:
		ProjectileChildActorRef->AttachToComponent(ProjectileChildActorComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);
		break;

	case EWeaponType::WT_2HandCrossbow:
		break;
	case EWeaponType::WT_2HandShooter:
		break;
	default:
		break;
	}
	
}

void AItemWeaponRanged::UpdateCurves()
{
	Super::UpdateCurves();
	
	if (!OwnerAnimInstanceRef) { return; }
	
	OwnerAnimInstanceRef->GetCurveValue("EnableFire", EnableFireCurve);

	OwnerAnimInstanceRef->GetCurveValue("EnableHoldBowstring", HoldBowstringCurve);

	//OwnerAnimInstanceRef->GetCurveValue("ProjectileStrengthScale", ProjectileStrengthScaleCurve);

}

void AItemWeaponRanged::FireProjectileCooldown()
{
	SetCanFire(true);
}

void AItemWeaponRanged::UpdateProjectileRotation()
{
	BowstringPullPoint->SetRelativeRotation(UKismetMathLibrary::FindLookAtRotation(BowstringPullPoint->GetRelativeLocation(), ProjectileAimPoint->GetRelativeLocation()));
}

void AItemWeaponRanged::UpdateBowstringPosition()
{
	if (HoldBowstringCurve >= 1.f && GetItemWeaponRangedData()->AmmoInMagazine > 0.f)
	{
		if (!GetParentActor()) { return; }
		if (!OwnerSkeletalMeshComponentRef) { return; }

		BowstringResetTime = 0.f;

		if (OwnerSkeletalMeshComponentRef->DoesSocketExist("Bowstring_Pull"))
		{
			BowstringPullPoint->SetWorldLocation(OwnerSkeletalMeshComponentRef->GetSocketTransform("Bowstring_Pull").GetLocation());
		}
	}
	else if (BowstringPullPoint->GetRelativeLocation() != FVector{0.f, 0.f, 0.f})
	{
		BowstringPullPoint->SetRelativeLocation(UKismetMathLibrary::VInterpTo_Constant(BowstringPullPoint->GetRelativeLocation(), { 0.f, 0.f, 0.f }, GetWorld()->GetDeltaSeconds(), BowstringInterpSpeed));
		BowstringResetTime += GetWorld()->GetDeltaSeconds();
	}
}
