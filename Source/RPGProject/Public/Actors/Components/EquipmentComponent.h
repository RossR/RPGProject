// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
//#include "Actors/ItemTypes/ItemBase.h"
//#include "Actors/ItemTypes/Equipment/Weapons/ItemWeapon.h"
//#include "Actors/ItemTypes/Equipment/ItemEquipment.h"
#include "EquipmentComponent.generated.h"

class UCombatComponent;
class UInventoryComponent;
class AItemBase;
class UItemData;
class AItemWeapon;
class UItemWeaponData;
class AItemEquipment;
class UItemEquipmentData;

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	ES_None				UMETA(DisplayName = "None"),
	ES_MainHandOne		UMETA(DisplayName = "Main-hand #1"),
	ES_OffHandOne		UMETA(DisplayName = "Off-hand #1"),
	ES_MainHandTwo		UMETA(DisplayName = "Main-hand #2"),
	ES_OffHandTwo		UMETA(DisplayName = "Off-hand #2"),
	ES_Head				UMETA(DisplayName = "Head"),
	ES_Torso			UMETA(DisplayName = "Torso"),
	ES_Hands			UMETA(DisplayName = "Hands"),
	ES_Legs				UMETA(DisplayName = "Legs"),
	ES_Feet				UMETA(DisplayName = "Feet"),
	ES_TorsoAccessory	UMETA(DisplayName = "Torso Accessory"),
	ES_HeadAccessory	UMETA(DisplayName = "Head Accessory"),
	ES_NeckAccessory	UMETA(DisplayName = "Neck Accessory"),
	ES_ArmAccessory		UMETA(DisplayName = "Arm Accessory"),
	ES_RingAccessory1	UMETA(DisplayName = "Ring Accessory #1"),
	ES_RingAccessory2	UMETA(DisplayName = "Ring Accessory #2"),

	ES_MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponLoadout : uint8
{
	WL_LoadoutOne	UMETA(DisplayName = "Loadout 1"),
	WL_LoadoutTwo	UMETA(DisplayName = "Loadout 2"),

	WL_MAX			UMETA(Hidden)
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// --- FUNCTIONS --- //

	// Sets default values for this component's properties
	UEquipmentComponent();

	UFUNCTION(BlueprintPure)
	bool HasEquipmentInSlot(EEquipmentSlot EquipmentSlot) const { return WornEquipmentDataMap.Contains(EquipmentSlot); }

	UFUNCTION(BlueprintPure)
	UItemData* GetWornEquipmentDataInSlot(EEquipmentSlot EquipmentSlot) const { if (WornEquipmentDataMap.Contains(EquipmentSlot)) { return WornEquipmentDataMap[EquipmentSlot]; } return nullptr; }

	UFUNCTION(BlueprintPure)
	UChildActorComponent* GetWornEquipmentActorInSlot(EEquipmentSlot EquipmentSlot) const { if (WornEquipmentActorMap.Contains(EquipmentSlot)) { return WornEquipmentActorMap[EquipmentSlot]; } return nullptr; }

	UFUNCTION(BlueprintPure)
	TMap<EEquipmentSlot, UItemData*>  GetWornEquipmentDataMap() const { return WornEquipmentDataMap; }

	UFUNCTION(BlueprintPure)
	TMap<EEquipmentSlot, UChildActorComponent*>  GetWornEquipmentActorMap() const { return WornEquipmentActorMap; }

	UFUNCTION(BlueprintPure)
	TMap<EEquipmentSlot, TSubclassOf<AItemEquipment>> GetStartingEquipment() const { return StartingEquipment; }

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
	bool Equip(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo = EEquipmentSlot::ES_None, UInventoryComponent* InInventoryComponentRef = nullptr, int ItemToEquipInventoryKey = -1, bool bRemoveFromInventoryOnSuccessfulEquip = true);

	UFUNCTION(BlueprintCallable)
	bool Unequip(EEquipmentSlot WornEquipmentSlot, UInventoryComponent* InInventoryComponentRef = nullptr, int InventoryItemKey = -1, bool bDropOnGround = false);

	UFUNCTION(BlueprintCallable)
	bool EquipCheck(UItemData* ItemToEquip, EEquipmentSlot SlotToEquipTo = EEquipmentSlot::ES_None, UInventoryComponent* InInventoryComponentRef = nullptr, int ItemToEquipInventoryKey = -1, bool bPopulateEquipCheckDataMap = true);

	UFUNCTION(BlueprintCallable)
	bool RemoveEquipmentInSlot(EEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable)
	EEquipmentSlot GetEquipmentSlotForItem(UItemData* Item);
	
	UFUNCTION(BlueprintCallable)
	void EquipOffHandIfMainHandIsEmpty();

	UFUNCTION(BlueprintCallable)
	bool SwapEquipment(EEquipmentSlot FirstEquipment, EEquipmentSlot SecondEquipment);

	UFUNCTION(BlueprintCallable)
	void CreateEquipmentChildActors();

	UFUNCTION(BlueprintCallable)
	void UpdateEquipmentChildActors(bool bForceUpdate = false);

	UFUNCTION(BlueprintCallable)
	void EquipStartingEquipment();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSocket(AItemEquipment* WeaponToAttach, FName SocketName);

	UFUNCTION(BlueprintCallable)
	void AttachEquipmentToMesh(USkeletalMeshComponent* CharacterMesh);

	UFUNCTION(BlueprintCallable)
	void AttachEquipmentToSocket(EEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable)
	EEquipmentSlot GetEquipmentSlotForCurrentWeaponLoadout(bool bReturnMainhandSlot = true);

	UFUNCTION(BlueprintPure)
	// Returns current weapon loadout
	EWeaponLoadout GetCurrentWeaponLoadout() { return CurrentWeaponLoadout; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentWeaponLoadout(EWeaponLoadout NewCurrentWeaponLoadout) { CurrentWeaponLoadout = NewCurrentWeaponLoadout; }

	// ToDo - Add functionality to calculate total weight of equipment
	UFUNCTION(BlueprintPure)
	float GetEquipmentWeight();

	UFUNCTION(BlueprintPure)
	AItemWeapon* GetMainhandWeaponActor();
	UFUNCTION(BlueprintPure)
	UItemWeaponData* GetMainhandWeaponData();

	UFUNCTION(BlueprintPure)
	AItemWeapon* GetOffhandWeaponActor();
	UFUNCTION(BlueprintPure)
	UItemWeaponData* GetOffhandWeaponData();

protected:

	// --- VARIABLES --- //

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Data")
	TMap<EEquipmentSlot, UItemData*> WornEquipmentDataMap;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	EWeaponLoadout CurrentWeaponLoadout = EWeaponLoadout::WL_LoadoutOne;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	ACharacter* OwningCharacter;

	// Child actor variables for worn equipment

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<EEquipmentSlot, UChildActorComponent*> WornEquipmentActorMap;

	// TMap for starting equipment
	UPROPERTY(EditAnywhere, Category = "Starting Equipment");
	TMap<EEquipmentSlot, TSubclassOf<AItemEquipment>> StartingEquipment;

	UInventoryComponent* OwnerInventoryComponentRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment")
	TMap<EEquipmentSlot, UItemData*> EquipCheckDataMap;

	int EmptyInventorySlots = 0;

private:

	bool IsEquipSlotValidForItem(EEquipmentSlot EquipSlot, UItemEquipmentData* Item);

	bool OwningCharacterHasAttributesToEquipItem(UItemEquipmentData* Item);

};
