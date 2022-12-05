// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "ItemEquipment.generated.h"


UCLASS()
class RPGPROJECT_API AItemEquipment : public AItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemEquipment();

	virtual UItemData* GetItemData() override { return Cast<UItemData>(EquipmentData); }

	UFUNCTION(BlueprintPure)
	UItemEquipmentData* GetItemEquipmentData() { return EquipmentData; }

	virtual void ItemPickupEvent() override { OnItemPickup.ExecuteIfBound(EquipmentData); }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetItemData(UItemData* NewItemData) override;

protected:

	virtual void UpdateCurves() override;

	
protected:

	/*UPROPERTY(EditAnywhere, Instanced, Category = "TEST")
	UItemEquipmentData* EquipmentData;*/

private:

	UItemEquipmentData* EquipmentData;
	
};
