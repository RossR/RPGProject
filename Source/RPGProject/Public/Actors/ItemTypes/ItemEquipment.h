// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "ItemEquipment.generated.h"

//UENUM(BlueprintType)
//enum class EEquipmentType : uint8
//{
//	EET_None			UMETA(DisplayName = "None"),
//	EET_Head			UMETA(DisplayName = "Head"),
//	EET_Torso			UMETA(DisplayName = "Torso"),
//	EET_Hands			UMETA(DisplayName = "Hands"),
//	EET_Legs			UMETA(DisplayName = "Legs"),
//	EET_Feet			UMETA(DisplayName = "Feet"),
//	EET_TorsoAccessory	UMETA(DisplayName = "Torso Accessory"),
//	EET_HeadAccessory	UMETA(DisplayName = "Head Accessory"),
//	EET_NeckAccessory	UMETA(DisplayName = "Neck Accessory"),
//	EET_ArmAccessory	UMETA(DisplayName = "Arm Accessory"),
//	EET_RingAccessory	UMETA(DisplayName = "Ring Accessory"),
//	
//	EET_Max				UMETA(Hidden)
//};
//
//USTRUCT(BlueprintType)
//struct FEquipmentInfo : public FItemInfo
//{
//	GENERATED_BODY()
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	EEquipmentType EquipmentType;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	float EquipmentDurability;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	float PhysicalDefence;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	float MagicalDefence;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	float ElementalDefence;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
//	float MentalDefence;
//
//};


UCLASS()
class RPGPROJECT_API AItemEquipment : public AItemBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AItemEquipment();

	virtual UItemData* GetItemData() override { return ItemData; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data")
	UItemEquipmentData* EquipmentData;
	
};
