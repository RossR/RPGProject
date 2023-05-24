// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Enums/CombatEnums.h"
#include "Structs/CombatStructs.h"
#include "ItemData.generated.h"

class UAnimSequence;
class AItemBase;
class AProjectileActor;

//------------------
// Currency
//------------------

USTRUCT(BlueprintType)
struct FCurrency
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int Platinum = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int Gold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int Silver = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Currency")
	int Copper = 0;

};

//------------------
// ItemData
//------------------

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIC_Default			UMETA(DisplayName = "Default"),
	EIC_Consumable		UMETA(DisplayName = "Consumable"),
	EIC_Equipment		UMETA(DisplayName = "Equipment"),
	EIC_Miscellaneous	UMETA(DisplayName = "Miscellaneous"),
	EIC_Valuable		UMETA(DisplayName = "Valuable"),
	EIC_Weapon			UMETA(DisplayName = "Weapon"),

	EIC_Max				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_None		UMETA(DisplayName = "Default"),
	EIR_Common		UMETA(DisplayName = "Common"),
	EIR_Uncommon	UMETA(DisplayName = "Uncommon"),
	EIR_Rare		UMETA(DisplayName = "Rare"),
	EIR_Mythic		UMETA(DisplayName = "Mythic"),
	EIR_Legendary	UMETA(DisplayName = "Legendary"),

	EIR_Max			UMETA(Hidden)
};

UCLASS(BlueprintType)
class RPGPROJECT_API UItemData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	int ItemQualityRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	FCurrency ItemValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Info")
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Info")
	float ItemMaxDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Info")
	float ItemCurrentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Info")
	bool bUseItemWeightForPhysics = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Info")
	bool bCanBeDropped = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Info")
	TSoftClassPtr<AItemBase> ClassToSpawn;
};

//------------------
// ItemEquipmentData
//------------------

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_None			UMETA(DisplayName = "None"),
	EET_1HWeapon		UMETA(DisplayName = "One-handed Weapon"),
	EET_2HWeapon		UMETA(DisplayName = "Two-handed Weapon"),
	EET_OffHand			UMETA(DisplayName = "Off-hand"),
	EET_Head			UMETA(DisplayName = "Head"),
	EET_Torso			UMETA(DisplayName = "Torso"),
	EET_Hands			UMETA(DisplayName = "Hands"),
	EET_Legs			UMETA(DisplayName = "Legs"),
	EET_Feet			UMETA(DisplayName = "Feet"),
	EET_Torso_Accessory	UMETA(DisplayName = "Torso Accessory"),
	EET_Head_Accessory	UMETA(DisplayName = "Head Accessory"),
	EET_Neck_Accessory	UMETA(DisplayName = "Neck Accessory"),
	EET_Arm_Accessory	UMETA(DisplayName = "Arm Accessory"),
	EET_Ring_Accessory	UMETA(DisplayName = "Ring Accessory"),
	
	EET_MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponStanceType : uint8
{
	ST_None			UMETA(DisplayName = "None"),
	ST_Ranged		UMETA(DisplayName = "Ranged"),
	ST_Guard		UMETA(DisplayName = "Guard"),

	ST_MAX			UMETA(Hidden)
};

UCLASS()
class RPGPROJECT_API UItemEquipmentData : public UItemData
{
	GENERATED_BODY()

public:



public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Info")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Defence")
	float PhysicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Defence")
	float MagicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Defence")
	float ElementalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Defence")
	float MentalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredDexterity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredVitality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredGrit;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredIntelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredWisdom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredCharisma;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Stat Requirements")
	int RequiredLuck;

};

//------------------
// ItemWeaponData 
//------------------

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WT_None				UMETA(DisplayName = "None"),
	WT_Unarmed			UMETA(DisplayName = "Unarmed"),
	WT_Item				UMETA(DisplayName = "Item"),
	WT_1HandDagger		UMETA(DisplayName = "One-handed Dagger"),
	WT_1HandMace		UMETA(DisplayName = "One-handed Mace"),
	WT_1HandPistol		UMETA(DisplayName = "One-handed Pistol"),
	WT_1HandSpear		UMETA(DisplayName = "One-handed Spear"),
	WT_1HandSword		UMETA(DisplayName = "One-handed Sword"),
	WT_2HandAxe			UMETA(DisplayName = "Two-handed Axe"),
	WT_2HandBow			UMETA(DisplayName = "Two-handed Bow"),
	WT_2HandCrossbow	UMETA(DisplayName = "Two-handed Crossbow"),
	WT_2HandShooter		UMETA(DisplayName = "Two-handed Shooter"),
	WT_2HandSpear		UMETA(DisplayName = "Two-handed Spear"),
	WT_2HandStaff		UMETA(DisplayName = "Two-handed Staff"),
	WT_2HandSword		UMETA(DisplayName = "Two-handed Sword"),
	WT_1HandShield		UMETA(DisplayName = "One-handed Shield"),

	WT_Max UMETA(Hidden)
};



UCLASS()
class RPGPROJECT_API UItemWeaponData : public UItemEquipmentData
{
	GENERATED_BODY()

public:

	//UFUNCTION(BlueprintPure)
	//TMap<uint8, FWeaponAttackInfo> GetLightAttackInfo() const { if (!LightAttackInfo.IsEmpty()) { return LightAttackInfo; } else { return TMap<uint8, FWeaponAttackInfo>(); } }

	//UFUNCTION(BlueprintPure)
	//FWeaponAttackInfo GetLightAttackInfoAtIndex(uint8 Index) { return LightAttackInfo.Find(Index) ? LightAttackInfo[Index] : FWeaponAttackInfo(); }

	//UFUNCTION(BlueprintPure)
	//TMap<uint8, FWeaponAttackInfo> GetHeavyAttackInfo() const { if (!HeavyAttackInfo.IsEmpty()) { return HeavyAttackInfo; } else { return TMap<uint8, FWeaponAttackInfo>(); } }

	//UFUNCTION(BlueprintPure)
	//FWeaponAttackInfo GetHeavyAttackInfoAtIndex(uint8 Index) { return HeavyAttackInfo.Find(Index) ? HeavyAttackInfo[Index] : FWeaponAttackInfo(); }

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animations")
	UAnimSequence* CombatIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animations")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack Combos")
	uint8 LightAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack Combos")
	uint8 HeavyAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack Info")
	TMap<uint8, FWeaponAttackInfo> LightAttackInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Attack Info")
	TMap<uint8, FWeaponAttackInfo> HeavyAttackInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Blocking")
	float WeaponBlockAngle = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Blocking")
	float WeaponBlockStaminaDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Combat Actions")
	FCombatActionInfo NeutalStanceCombatActionInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Combat Actions")
	FCombatActionInfo MainhandStanceCombatActionInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Combat Actions")
	FCombatActionInfo OffhandStanceCombatActionInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	EWeaponStanceType MainhandStanceType = EWeaponStanceType::ST_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	EWeaponStanceType OffhandStanceType = EWeaponStanceType::ST_None;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	//float WeaponCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Info")
	float WeaponDamageVariance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mainhand Sockets")
	FName MainhandSheatheSocket = "SheatheRBack";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mainhand Sockets")
	FName MainhandEquippedSocket = "EquipItem_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Offhand Sockets")
	FName OffhandSheatheSocket = "SheatheLBack";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Offhand Sockets")
	FName OffhandEquippedSocket = "EquipItem_l";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|SFX")
	TObjectPtr<USoundBase> AttackSFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sheathe / Unsheathe")
	FName WeaponSheatheMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sheathe / Unsheathe")
	FName WeaponUnsheatheMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stance Info")
	FWeaponStanceInfo MainhandStanceInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Stance Info")
	FWeaponStanceInfo OffhandStanceInfo;
	
};

UCLASS()
class RPGPROJECT_API UItemWeaponMeleeData : public UItemWeaponData 
{
	GENERATED_BODY()

		// Create functions to get all this data?

public:

	

};

UENUM(BlueprintType)
enum class EFiringMode : uint8
{
	Single		UMETA(DisplayName = "Single Shot"),
	Burst		UMETA(DisplayName = "Burst Fire"),
	FullAuto	UMETA(DisplayName = "Full Auto"),

	MAX			UMETA(Hidden)
};

UCLASS()
class RPGPROJECT_API UItemWeaponRangedData : public UItemWeaponData
{
	GENERATED_BODY()

public:



public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ranged Weapon|Projectiles")
	TSubclassOf<AProjectileActor> ProjectileInUse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	EFiringMode FiringMode = EFiringMode::Single;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	int AmmoInMagazine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	int MagazineCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	int CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	int MaxAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ranged Weapon|Info")
	float MinimumWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animations")
	UAnimMontage* MainhandReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Animations")
	UAnimMontage* OffhandReloadMontage;
};

UCLASS()
class RPGPROJECT_API UItemWeaponShieldData : public UItemWeaponData
{
	GENERATED_BODY()

public:


};