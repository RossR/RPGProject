// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ProjectileActor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundAttenuation.h"
#include "Components/ShapeComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "DataAssets/HitFXData.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Actors/Components/CombatComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/ItemTypes/Equipment/Weapons/ItemWeaponRanged.h"

// Sets default values
AProjectileActor::AProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileCollisionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("Projectile Collision Volume"));
	RootComponent = ProjectileCollisionVolume;
	ProjectileCollisionVolume->OnComponentHit.AddDynamic(this, &AProjectileActor::OnProjectileCollisionVolumeHit);
	ProjectileCollisionVolume->SetSphereRadius(3.f);
	ProjectileCollisionVolume->SetCollisionProfileName("Projectile");
	ProjectileCollisionVolume->bReturnMaterialOnMove = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileActor::OnProjectileMeshHit);
	ProjectileMesh->SetCollisionProfileName("Projectile");

	ProjectileTrailVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Projectile Trail VFX"));
	ProjectileTrailVFX->SetupAttachment(ProjectileMesh, "ProjectileStart");
	
	ProjectileSweetSpotVFX= CreateDefaultSubobject<UNiagaraComponent>(TEXT("Projectile Sweet Spot VFX"));
	ProjectileSweetSpotVFX->SetupAttachment(ProjectileMesh, "ProjectileEnd");

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
	
	ProjectileAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Projectile Audio Component"));
	ProjectileAudioComponent->SetupAttachment(RootComponent);
	ProjectileAudioComponent->bAutoActivate = false;

}

// Called when the game starts or when spawned
void AProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if (GetProjectileOwner())
	{
		ProjectileCollisionVolume->IgnoreActorWhenMoving(GetProjectileOwner(), true);
		ProjectileMesh->IgnoreActorWhenMoving(GetProjectileOwner(), true);
	}

	if (bEnableProjectile)
	{
		ProjectileMovementComponent->Velocity = (ProjectileMovementComponent->InitialSpeed * ProjectileCollisionVolume->GetForwardVector());
		ProjectileMovementComponent->Activate(true);

		if (ProjectileType == EProjectileType::PT_Arrow)
		{
			GetWorldTimerManager().SetTimer(SpinProjectileTimerHandle, this, &AProjectileActor::SpinProjectile, GetWorld()->GetDeltaSeconds(), true);
		}
	}

	if (GetOwner())
	{
		IgnoredActorArray.Add(GetOwner());
	}

	if (ProjectileType == EProjectileType::PT_Arrow && CheckIfCloseToObject())
	{
		EnableProjectilePhysics(true);
	}

	
}

// Called every frame
void AProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileActor::OnProjectileCollisionVolumeHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		bool bAttached = false;

		ProjectileAudioComponent->Stop();

		if (OtherActor)
		{
			AItemWeapon* ItemWeaponRef = Cast<AItemWeapon>(OtherActor);
			AActor* ParentActorRef = OtherActor->GetParentActor();

			if (UCombatComponent* CombatComponentRef = Cast<UCombatComponent>(OtherActor->GetComponentByClass(UCombatComponent::StaticClass())))
			{
				AItemWeapon* ProjecileOwnerWeaponRef = Cast<AItemWeapon>(GetProjectileOwner());
				CombatComponentRef->EvaluateHitResult(Hit, ProjecileOwnerWeaponRef, this);
			}

		
			if (USkeletalMeshComponent* SkeletalMeshComponentRef = Cast<USkeletalMeshComponent>(OtherComp))
			{
				if (SkeletalMeshComponentRef->DoesSocketExist(Hit.BoneName))
				{
					AttachToComponent(SkeletalMeshComponentRef, FAttachmentTransformRules::KeepWorldTransform, Hit.BoneName);
					bAttached = true;
				}
			}

			if (ItemWeaponRef)
			{
				if (ParentActorRef)
				{
					UEquipmentComponent* ParentEquipmentComponentRef = Cast<UEquipmentComponent>(ParentActorRef->GetComponentByClass(UEquipmentComponent::StaticClass()));
					UCombatComponent* ParentCombatComponentRef = Cast<UCombatComponent>(ParentActorRef->GetComponentByClass(UCombatComponent::StaticClass()));

					if (ParentEquipmentComponentRef && ParentCombatComponentRef)
					{
						if (ParentCombatComponentRef->GetCombatState() == ECombatState::CS_CombatReady && ParentCombatComponentRef->GetIsGuarding() && (ParentEquipmentComponentRef->GetMainhandWeaponActor() == ItemWeaponRef || ParentEquipmentComponentRef->GetOffhandWeaponActor() == ItemWeaponRef))
						{
							FWeaponAttackInfo WeaponAttackInfo;

							ParentCombatComponentRef->AttackBlocked(WeaponAttackInfo, nullptr, EAttackType::AT_None, this);
							//ItemWeaponRef->AttackBlocked(WeaponAttackInfo, nullptr, EAttackType::AT_None, this);
						}
					}
				}

				PlayProjectileHitFX(Hit);

				
				/*if (WeaponData && WeaponData->WeaponType == EWeaponType::WT_1HandShield)
				{
					AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
					PlayProjectileHitFX(Hit);
					bAttached = true;
				}
				else
				{
					PlayProjectileHitFX(Hit);
				}*/
			}
			else if (!bAttached)
			{
				AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform, NAME_None);
				PlayProjectileHitFX(Hit);
				bAttached = true;
			}
		}
		else if (OtherComp && !bAttached)
		{
			AttachToComponent(OtherComp, FAttachmentTransformRules::KeepWorldTransform);
			PlayProjectileHitFX(Hit);
			bAttached = true;
		}

		if (OtherComp)
		{
			if (OtherComp->IsSimulatingPhysics())
			{
				FName HitBoneName = Hit.BoneName;
				if (bIgnoreMassForImpulse)
				{
					OtherComp->AddVelocityChangeImpulseAtLocation((Hit.ImpactNormal * ProjectileImpactImpulse), NormalImpulse, Hit.BoneName);
				}
				else
				{
					OtherComp->AddImpulseAtLocation((Hit.ImpactNormal * ProjectileImpactImpulse), NormalImpulse, Hit.BoneName);
				}
			}
		}

		if (bAttached)
		{
			ProjectileCollisionVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


			ProjectileCollisionVolume->IgnoreActorWhenMoving(GetProjectileOwner(), false);
			ProjectileMesh->IgnoreActorWhenMoving(GetProjectileOwner(), false);

			if (ProjectileMesh->DoesSocketExist(ProjectileEndSocket) && ProjectileMesh->DoesSocketExist(ProjectileImpalePointLargeSocket))
			{
				SetActorLocation((GetActorLocation() + (ProjectileMesh->GetSocketLocation(ProjectileEndSocket) - ProjectileMesh->GetSocketLocation(ProjectileImpalePointLargeSocket))));
			}

			SetLifeSpan(ProjectileLifeSpawn*2.f);
		}
		else
		{
			EnableProjectilePhysics(true);
			FVector LastVelocity = ProjectileMovementComponent->Velocity;
			ProjectileCollisionVolume->SetAllPhysicsLinearVelocity({ 0.f, 0.f, 0.f }, false);

			FVector Impulse = Hit.ImpactNormal * (UKismetMathLibrary::VSize(LastVelocity) * ProjectileVelocityDeflectionMultiplier);
			FVector Location = ProjectileMesh->DoesSocketExist("ProjectileEnd") ? ProjectileMesh->GetSocketLocation("ProjectileEnd") : Hit.ImpactPoint;

			if (ProjectileCollisionVolume->IsSimulatingPhysics()) { ProjectileCollisionVolume->AddVelocityChangeImpulseAtLocation(Impulse, Location); }

			SetLifeSpan(ProjectileLifeSpawn);
		}
	}

	//Destroy();
}

void AProjectileActor::OnProjectileMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Hit.bBlockingHit)
	{
		EnableProjectilePhysics(true);
		ProjectileAudioComponent->Stop();
	}
}

void AProjectileActor::EnableProjectilePhysics(bool bActive)
{
	ProjectileMovementComponent->Activate(false);
	ProjectileCollisionVolume->SetSimulatePhysics(true);
	//ProjectileMesh->SetSimulatePhysics(true);
}

void AProjectileActor::PlayProjectileSFX()
{
	ProjectileAudioComponent-> Play();
	
	//UGameplayStatics::SpawnSoundAttached(Projectile)
}

void AProjectileActor::SpinProjectile()
{
	if (GetVelocity() != FVector{ 0.f, 0.f, 0.f })
	{
		ProjectileMesh->AddRelativeRotation(FRotator{ 0.f, 0.f, ProjectileSpinSpeed });
	}
	else
	{
		GetWorldTimerManager().ClearTimer(SpinProjectileTimerHandle);

		if (!GetAttachParentActor())
		{
			EnableProjectilePhysics(true);
		}
	}
}

bool AProjectileActor::CheckIfCloseToObject()
{
	if (ProjectileMesh->DoesSocketExist("ProjectileStart") && ProjectileMesh->DoesSocketExist("ProjectileEnd"))
	{
		FVector TraceStart = ProjectileMesh->GetSocketLocation("ProjectileStart");
		FVector TraceEnd = TraceStart + (ProjectileCollisionVolume->GetForwardVector() * (2.5f * FMath::Abs(UKismetMathLibrary::VSize(ProjectileMesh->GetSocketLocation("ProjectileStart") - ProjectileMesh->GetSocketLocation("ProjectileEnd")))));
		FHitResult HitResult;

		return UKismetSystemLibrary::LineTraceSingleByProfile(GetWorld(), TraceStart, TraceEnd, "Projectile", false, IgnoredActorArray, EDrawDebugTrace::None, HitResult, true);
	}

	return false;
}

void AProjectileActor::PlayProjectileHitFX(FHitResult InHitResult)
{
	if (!InHitResult.HasValidHitObjectHandle()) { return; }

	UNiagaraSystem* HitVFXSystem = nullptr;
	USoundCue* HitSoundCue = nullptr;

	if (InHitResult.PhysMaterial.IsValid())
	{
		const TEnumAsByte<EPhysicalSurface> HitSurface = InHitResult.PhysMaterial.Get() ? InHitResult.PhysMaterial.Get()->SurfaceType : EPhysicalSurface::SurfaceType_Default;

		if (GetHitFXData())
		{
			if (GetHitFXData()->HitVFXMap.Contains(HitSurface))
			{
				HitVFXSystem = GetHitFXData()->HitVFXMap[HitSurface].NiagaraSystem;
				HitSoundCue = GetHitFXData()->HitVFXMap[HitSurface].SoundCue;
			}
		}
	}
	if (HitVFXSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HitVFXSystem, InHitResult.ImpactPoint, InHitResult.ImpactNormal.Rotation(), { 1.f, 1.f, 1.f }, true, true, ENCPoolMethod::None, true);
	}

	if (HitSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSoundCue, InHitResult.ImpactPoint, 1.f, 1.f, 0.f);
	}
}
