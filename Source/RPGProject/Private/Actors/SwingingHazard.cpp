// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SwingingHazard.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CableComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Characters/RPGProjectPlayerCharacter.h"

// Sets default values
ASwingingHazard::ASwingingHazard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HangingPoint = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Hanging Point"));
	SwingingHazard = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Swinging Hazard"));
	Cable = CreateDefaultSubobject<UCableComponent>(TEXT("Cable"));
	PhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("Physics Constraint"));
	TriggerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Trigger Capsule"));

	RootComponent = HangingPoint;

	SwingingHazard->SetupAttachment(RootComponent);
	SwingingHazard->SetRelativeLocation({ 0.0f,0.0f,-800.0f });
	SwingingHazard->SetSimulatePhysics(true);

	Cable->SetupAttachment(RootComponent);
	// Cable->SetAttachEndToComponent(SwingingHazard, NAME_None);
	Cable->EndLocation = { 0,0,0 };

	PhysicsConstraint->SetupAttachment(RootComponent);
	
	PhysicsConstraint->SetRelativeLocation({ 0.0f,0.0f,-50.0f });

	bSwingingClockwise = true;
	StartSwingDelay = 1.0f;
	ImpulseVector = { 0,0,0 };
	DirectionChangeAngle = 60.0f;

	TriggerCapsule->SetupAttachment(SwingingHazard);
	TriggerCapsule->SetCapsuleHalfHeight(52.0f);
	TriggerCapsule->SetCapsuleRadius(52.0f);
	TriggerCapsule->OnComponentBeginOverlap.AddDynamic(this, &ASwingingHazard::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ASwingingHazard::BeginPlay()
{
	Super::BeginPlay();

	PhysicsConstraint->SetConstrainedComponents(HangingPoint, NAME_None, SwingingHazard, NAME_None);
	
	// GetWorldTimerManager().SetTimer(StartSwingDelayTimerHandle, this, &ASwingingHazard::StartSwingDelayFinished, StartSwingDelay, false);

}

// Called every frame
void ASwingingHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DeltaSeconds = UGameplayStatics::GetWorldDeltaSeconds(GetWorld());

	// If swinging on X-axis
	if (ImpulseVector.X > ImpulseVector.Y || ImpulseVector.X > ImpulseVector.Z)
	{
		RotationToCheck = SwingingHazard->GetRelativeRotation().Pitch;
	}
	// If swinging on Y-axis
	else if (ImpulseVector.Y > ImpulseVector.Z || ImpulseVector.Y > ImpulseVector.X)
	{
		RotationToCheck = SwingingHazard->GetRelativeRotation().Roll;
	}
	// If swinging on Z-axis
	else if (ImpulseVector.Z > ImpulseVector.X || ImpulseVector.Z > ImpulseVector.Y)
	{
		RotationToCheck = SwingingHazard->GetRelativeRotation().Yaw;
	}

	if (bSwingingClockwise)
	{
		SwingClockwise();
	} 
	else if (!bSwingingClockwise)
	{
		SwingAnticlockwise();
	}
}


void ASwingingHazard::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARPGProjectPlayerCharacter* PlayerCharacter = Cast<ARPGProjectPlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		PlayerCharacter->GetMesh()->SetSimulatePhysics(true);
		PlayerCharacter->SetIsRagdollDeath(true);
		PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//PlayerCharacter->ReduceHealth(PlayerCharacter->GetCurrentHealthPoints(), DamageEvent, nullptr, GetOwner());
		// PlayerCharacter->GetMesh()->AddImpulse(SwingingHazard->GetComponentVelocity());
	}
}


void ASwingingHazard::StartSwingDelayFinished()
{
	GetWorldTimerManager().ClearTimer(StartSwingDelayTimerHandle);

	if (!bSwingingClockwise)
	{
		// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::BeginPlay SwingClockwise timer started"));
		GetWorldTimerManager().SetTimer(SwingClockwiseTimerHandle, this, &ASwingingHazard::SwingClockwise, DeltaSeconds, true);
	}
	else
	{
		// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::BeginPlay SwingClockwise timer started"));
		GetWorldTimerManager().SetTimer(SwingAnticlockwiseTimerHandle, this, &ASwingingHazard::SwingAnticlockwise, DeltaSeconds, true);
	}
}

void ASwingingHazard::SwingClockwise()
{
	SwingingHazard->AddImpulse(ImpulseVector * DeltaSeconds);

	// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::SwingClockwise called"));

	if (RotationToCheck > DirectionChangeAngle)
	{
		SwingingHazard->SetAllPhysicsLinearVelocity({ (ImpulseVector.X * 0.1f) * DeltaSeconds, (ImpulseVector.Y * 0.1f) * DeltaSeconds, (ImpulseVector.Z * 0.1f) * DeltaSeconds }, false);
		bSwingingClockwise = false;
		// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::SwingClockwise ended"));
		// GetWorldTimerManager().ClearTimer(SwingClockwiseTimerHandle);
		// GetWorldTimerManager().SetTimer(SwingAnticlockwiseTimerHandle, this, &ASwingingHazard::SwingAnticlockwise, DeltaSeconds, true);
	}
}

void ASwingingHazard::SwingAnticlockwise()
{
	// SwingingHazard->AddImpulse({ (ImpulseVector.X * -1), (ImpulseVector.Y * -1), (ImpulseVector.Z * -1) });
	SwingingHazard->AddImpulse(-ImpulseVector * DeltaSeconds);

	// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::SwingAnticlockwise called"));

	if (RotationToCheck< -DirectionChangeAngle)
	{
		SwingingHazard->SetAllPhysicsLinearVelocity({ (- ImpulseVector.X * 0.1f) * DeltaSeconds, (- ImpulseVector.Y * 0.1f) * DeltaSeconds, (- ImpulseVector.Z * 0.1f) * DeltaSeconds }, false);
		bSwingingClockwise = true;
		// UE_LOG(LogTemp, Warning, TEXT("ASwingingHazard::SwingAnticlockwise ended"));
		// GetWorldTimerManager().ClearTimer(SwingAnticlockwiseTimerHandle);
		// GetWorldTimerManager().SetTimer(SwingClockwiseTimerHandle, this, &ASwingingHazard::SwingClockwise, DeltaSeconds, true);
	}
}