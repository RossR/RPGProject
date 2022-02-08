// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/ItemWeapon.h"
#include "EquipmentComponent.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	EES_None			UMETA(DisplayName = "None"),
	EES_MainHand		UMETA(DisplayName = "Main-hand"),
	EES_OffHand			UMETA(DisplayName = "Off-hand"),
	EES_Head			UMETA(DisplayName = "Head"),
	EES_Torso			UMETA(DisplayName = "Torso"),
	EES_Hands			UMETA(DisplayName = "Hands"),
	EES_Legs			UMETA(DisplayName = "Legs"),
	EES_Feet			UMETA(DisplayName = "Feet"),
	EES_TorsoAccessory	UMETA(DisplayName = "Torso Accessory"),
	EES_Accessory1		UMETA(DisplayName = "Accessory #1"),
	EES_Accessory2		UMETA(DisplayName = "Accessory #2"),
	EES_Accessory3		UMETA(DisplayName = "Accessory #3"),
	EES_Accessory4		UMETA(DisplayName = "Accessory #4"),
	EES_Accessory5		UMETA(DisplayName = "Accessory #5"),

	EES_MAX			UMETA(Hidden)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<int, FItemInfo> ItemInfoMap;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	//TMap<int, FWeaponInfo2> WeaponInfoMap;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	// TMap<int, FEquipmentInfo> EquipmentInfoMap;

};
