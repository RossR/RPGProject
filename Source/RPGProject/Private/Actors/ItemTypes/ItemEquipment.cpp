// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/ItemEquipment.h"

// Sets default values
AItemEquipment::AItemEquipment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EquipmentMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Equipment Mesh"));

	SetRootComponent(EquipmentMesh);

}

// Called when the game starts or when spawned
void AItemEquipment::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AItemEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

