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

	RangedWeaponRef = Cast<AItemWeaponRanged>(GetOwner());
}

// Called every frame
void UProjectileSpawnerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

bool UProjectileSpawnerComponent::SpawnProjectile(TSubclassOf<AProjectileActor> ProjectileInUse, bool bAimAtCrosshair, float ProjectileStrengthScale)
{
	UClass* ProjectileClass = ProjectileInUse.Get();

	FRotator ProjectileSpawnAngle;

	if (bAimAtCrosshair && RangedWeaponRef)
	{
		if (ARPGProjectPlayerCharacter* OwningCharacterRef = Cast<ARPGProjectPlayerCharacter>(RangedWeaponRef->GetParentActor()))
		{
			if (ARPGPlayerCameraManager* CameraManagerRef = OwningCharacterRef->GetRPGPlayerCameraManager())
			{
				FHitResult CrosshairHitResult;

				CameraManagerRef->GetCrosshairTarget(CrosshairHitResult);

				FRotator FireDirection = UKismetMathLibrary::FindLookAtRotation(ProjectileSpawnTransform->GetComponentLocation(), CrosshairHitResult.Location);

				// Angle towards the player's crosshair with a random offset (accuracy)
				ProjectileSpawnAngle = FireDirection + FRotator((FMath::FRandRange(-FiringAngleVariance.Pitch, FiringAngleVariance.Pitch)) / ProjectileStrengthScale, (FMath::FRandRange(-FiringAngleVariance.Yaw, FiringAngleVariance.Yaw)) / ProjectileStrengthScale, 0.f);
			}
		}
	}
	else
	{
		// Use the ProjectileSpawnTransform's rotation with a random offset (accuracy)
		ProjectileSpawnAngle = ProjectileSpawnTransform->GetComponentRotation() + FRotator((FMath::FRandRange(-FiringAngleVariance.Pitch, FiringAngleVariance.Pitch)) / ProjectileStrengthScale, (FMath::FRandRange(-FiringAngleVariance.Yaw, FiringAngleVariance.Yaw)) / ProjectileStrengthScale, 0.f);
	}
	
	const FTransform SpawnTransform{ ProjectileSpawnAngle, ProjectileSpawnTransform->GetComponentLocation(), ProjectileSpawnTransform->GetComponentScale() };
	ProjectileSpawnParameters.Owner = GetOwner();

	// Spawn the projectile
	AProjectileActor* LoadedProjectile = GetWorld()->SpawnActor<AProjectileActor>(ProjectileClass, SpawnTransform, ProjectileSpawnParameters);

	int SpawnIndex = 0;

	// If the projectile was spawned successfully
	if (LoadedProjectile)
	{
		// Appropriately set the location of the projectile to the location of the ProjectileSpawnTransform
		if (LoadedProjectile->GetProjectileMesh()->DoesSocketExist("ProjectileStart") && LoadedProjectile->GetProjectileMesh()->DoesSocketExist("ProjectileEnd"))
		{
			FVector ProjectileStart = LoadedProjectile->GetProjectileMesh()->GetSocketTransform("ProjectileStart").GetLocation();
			FVector ProjectileEnd = LoadedProjectile->GetProjectileMesh()->GetSocketTransform("ProjectileEnd").GetLocation();

			FVector SpawnLocationAdjustment = UKismetMathLibrary::Subtract_VectorVector(ProjectileEnd, ProjectileStart);

			LoadedProjectile->SetActorLocation(ProjectileSpawnTransform->GetComponentLocation() + SpawnLocationAdjustment);
			LoadedProjectile->SetProjectileOwner(GetOwner());
		}

		// Set the speed of the projectile depending on the projectile's strength
		LoadedProjectile->GetProjectileMovementComponent()->InitialSpeed = LoadedProjectile->GetProjectileMovementComponent()->InitialSpeed * ProjectileStrengthScale;
		LoadedProjectile->SetProjectileStrengthMultiplier(ProjectileStrengthScale);

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

