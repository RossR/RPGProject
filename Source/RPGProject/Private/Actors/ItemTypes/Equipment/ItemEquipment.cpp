// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ItemTypes/Equipment/ItemEquipment.h"
#include "Actors/ItemTypes/ItemBase.h"


// Sets default values
AItemEquipment::AItemEquipment()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemDataDefault = CreateDefaultSubobject<UItemEquipmentData>(TEXT("Equipment Data Default"));
	//EquipmentData = CreateDefaultSubobject<UItemEquipmentData>(TEXT("Equipment Data"));

}

// Called when the game starts or when spawned
void AItemEquipment::BeginPlay()
{
	Super::BeginPlay();
	
	SetItemData(ItemDataDefault);
}

// Called every frame
void AItemEquipment::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

void AItemEquipment::SetItemData(UItemData* NewItemData)
{
	if (UItemEquipmentData* NewItemEquipmentData = Cast<UItemEquipmentData>(NewItemData))
	{
		EquipmentData = NULL;
		EquipmentData = NewObject<UItemEquipmentData>(this, UItemEquipmentData::StaticClass(), TEXT("Equipment Data"), EObjectFlags::RF_NoFlags, NewItemEquipmentData);
	}
}

void AItemEquipment::UpdateCurves()
{
	Super::UpdateCurves();

}
