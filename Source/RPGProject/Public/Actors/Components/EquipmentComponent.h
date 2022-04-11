// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/ItemTypes/ItemBase.h"
#include "Actors/ItemTypes/ItemWeapon.h"
#include "Actors/ItemTypes/ItemEquipment.h"
#include "EquipmentComponent.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	EES_None			UMETA(DisplayName = "None"),
	EES_MainHandOne		UMETA(DisplayName = "Main-hand #1"),
	EES_OffHandOne		UMETA(DisplayName = "Off-hand #1"),
	EES_MainHandTwo		UMETA(DisplayName = "Main-hand #2"),
	EES_OffHandTwo		UMETA(DisplayName = "Off-hand #2"),
	EES_Head			UMETA(DisplayName = "Head"),
	EES_Torso			UMETA(DisplayName = "Torso"),
	EES_Hands			UMETA(DisplayName = "Hands"),
	EES_Legs			UMETA(DisplayName = "Legs"),
	EES_Feet			UMETA(DisplayName = "Feet"),
	EES_TorsoAccessory	UMETA(DisplayName = "Torso Accessory"),
	EES_HeadAccessory	UMETA(DisplayName = "Head Accessory"),
	EES_NeckAccessory	UMETA(DisplayName = "Neck Accessory"),
	EES_ArmAccessory	UMETA(DisplayName = "Arm Accessory"),
	EES_RingAccessory1	UMETA(DisplayName = "Ring Accessory #1"),
	EES_RingAccessory2	UMETA(DisplayName = "Ring Accessory #2"),

	EES_MAX			UMETA(Hidden)
};

class UCombatComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// --- FUNCTIONS --- //

	// Sets default values for this component's properties
	UEquipmentComponent();

	UFUNCTION(BlueprintPure)
	UItemData* GetWornEquipmentDataInSlot(EEquipmentSlot EquipmentSlot) { if (WornEquipmentData.Contains(EquipmentSlot)) { return WornEquipmentData[EquipmentSlot]; } return nullptr; }

	UFUNCTION(BlueprintPure)
	UChildActorComponent* GetWornEquipmentActorInSlot(EEquipmentSlot EquipmentSlot) { if (WornEquipmentActors.Contains(EquipmentSlot)) { return WornEquipmentActors[EquipmentSlot]; } return nullptr; }

	UFUNCTION(BlueprintCallable)
	TMap<EEquipmentSlot, UItemData*>  GetWornEquipmentData() { return WornEquipmentData; }

	UFUNCTION(BlueprintCallable)
	TMap<EEquipmentSlot, UChildActorComponent*>  GetWornEquipmentActors() { return WornEquipmentActors; }

	UFUNCTION(BlueprintCallable)
	bool RemoveWornEquipmentDataInSlot(EEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable)
	void SetOwnerCharacter(ACharacter* Character) { OwningCharacter = Character; }

	UFUNCTION(BlueprintPure)
	ACharacter* GetOwnerCharacter() { return OwningCharacter; }

protected:

	// --- FUNCTIONS --- //

	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// --- FUNCTIONS --- //

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool Equip(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo = EEquipmentSlot::EES_None);

	UFUNCTION(BlueprintCallable)
	bool Unequip(EEquipmentSlot WornEquipmentSlot);

	UFUNCTION(BlueprintCallable)
	EEquipmentSlot GetEquipmentSlotForItem(UItemData* Item);
	
	UFUNCTION(BlueprintCallable)
	void EquipOffHandIfMainHandIsEmpty();

	UFUNCTION(BlueprintCallable)
	bool SwapEquipment(EEquipmentSlot FirstEquipment, EEquipmentSlot SecondEquipment);

	UFUNCTION(BlueprintCallable)
	void CreateEquipmentChildActors();

	UFUNCTION(BlueprintCallable)
	void UpdateEquipmentChildActors();

	UFUNCTION(BlueprintCallable)
	void AttachEquipmentToMesh(USkeletalMeshComponent* CharacterMesh);

	UFUNCTION(BlueprintCallable)
	void AttachEquipmentToSocket(EEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable)
	EEquipmentSlot GetCurrentlyEquippedWeaponSet();

	UFUNCTION(BlueprintPure)
	// Returns true if using the first weapon set.
	bool GetIsUsingFirstWeaponSet() { return bIsUsingFirstWeaponSet; }

	// ToDo - Add functionality to calculate total weight of equipment


protected:

	// --- VARIABLES --- //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
	TMap<EEquipmentSlot, UItemData*> WornEquipmentData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
	bool bIsUsingFirstWeaponSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	ACharacter* OwningCharacter;

	// Child actor variables for worn equipment

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<EEquipmentSlot, UChildActorComponent*> WornEquipmentActors;
};
