// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/ProjectileSpawnerComponent.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"
#include "Characters/RPGProjectPlayerCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/ArrowComponent.h"
#include "Actors/ProjectileActor.h"


// Sets default values for this component's properties
UProjectileSpawnerComponent::UProjectileSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ProjectileSpawnTransform = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Transform"));

	RangedWeaponRef = Cast<AItemWeaponRanged>(GetOwner());

	if (RangedWeaponRef)
	{
		ProjectileSpawnTransform->SetupAttachment(RangedWeaponRef->GetRootComponent(), "ProjectileSpawn");
	}
	else if (GetOwner())
	{
		ProjectileSpawnTransform->SetupAttachment( GetOwner()->GetRootComponent());
	}

	ProjectileSpawnArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Projectile Spawn Arrow"));
	ProjectileSpawnArrow->SetupAttachment(ProjectileSpawnTransform);
	ProjectileSpawnArrow->ArrowColor = FColor().FromHex("FFE100FF"); // Uses Hex sRGB value
	ProjectileSpawnArrow->ArrowSize = .2f;
	ProjectileSpawnArrow->ArrowLength = 80.f;

	ProjectileSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
}


// Called when the game starts
void UProjectileSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProjectileSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UProjectileSpawnerComponent::SpawnProjectile(TSubclassOf<AProjectileActor> ProjectileInUse, bool bAimAtCrosshair, float ProjectileStrength)
{
	UClass* ProjectileClass = ProjectileInUse.Get();
	
	FRotator SpawnAngle;

	if (bAimAtCrosshair && RangedWeaponRef)
	{
		if (ARPGProjectPlayerCharacter* OwningCharacterRef = Cast<ARPGProjectPlayerCharacter>(RangedWeaponRef->GetParentActor()))
		{
			if (ARPGPlayerCameraManager* CameraManagerRef = OwningCharacterRef->GetRPGPlayerCameraManager())
			{
				FHitResult CrosshairHitResult;
				if (true)
				{
					CameraManagerRef->GetCrosshairTarget(CrosshairHitResult);
					
					FRotator FireDirection = UKismetMathLibrary::FindLookAtRotation(ProjectileSpawnTransform->GetComponentLocation(), CrosshairHitResult.Location);

					SpawnAngle = FireDirection + FRotator((FMath::FRandRange(-FiringAngleVariance.Pitch, FiringAngleVariance.Pitch)) / ProjectileStrength, (FMath::FRandRange(-FiringAngleVariance.Yaw, FiringAngleVariance.Yaw)) / ProjectileStrength, 0.f);
				}
			}
		}
	}
	else
	{
		SpawnAngle = ProjectileSpawnTransform->GetComponentRotation() + FRotator((FMath::FRandRange(-FiringAngleVariance.Pitch, FiringAngleVariance.Pitch)) / ProjectileStrength, (FMath::FRandRange(-FiringAngleVariance.Yaw, FiringAngleVariance.Yaw)) / ProjectileStrength, 0.f);
	}
	
	const FTransform SpawnTransform{ SpawnAngle, ProjectileSpawnTransform->GetComponentLocation(), ProjectileSpawnTransform->GetComponentScale() };
	ProjectileSpawnParameters.Owner = GetOwner();

	AProjectileActor* LoadedProjectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SpawnTransform, ProjectileSpawnParameters);

	int SpawnIndex = 0;
	if (LoadedProjectile)
	{
		if (LoadedProjectile->GetProjectileMesh()->DoesSocketExist("ProjectileStart") && LoadedProjectile->GetProjectileMesh()->DoesSocketExist("ProjectileEnd"))
		{
			FVector ProjectileStart = LoadedProjectile->GetProjectileMesh()->GetSocketTransform("ProjectileStart").GetLocation();
			FVector ProjectileEnd = LoadedProjectile->GetProjectileMesh()->GetSocketTransform("ProjectileEnd").GetLocation();

			FVector FSpawnLocationAdjustment = UKismetMathLibrary::Subtract_VectorVector(ProjectileEnd, ProjectileStart);

			LoadedProjectile->SetActorLocation(ProjectileSpawnTransform->GetComponentLocation() + FSpawnLocationAdjustment);
			LoadedProjectile->SetProjectileOwner(GetOwner());
		}

		LoadedProjectile->GetProjectileMovementComponent()->InitialSpeed = LoadedProjectile->GetProjectileMovementComponent()->InitialSpeed * ProjectileStrength;
		LoadedProjectile->SetProjectileStrengthMultiplier(ProjectileStrength);

		LoadedProjectile->PlayProjectileSFX();
	}
	
	if (LoadedProjectile)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UProjectileSpawnerComponent::SpawnProjectile Projectile spawn failed."));
	}

	return false;
}

