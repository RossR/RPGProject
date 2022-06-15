// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundWave.h"
#include "ItemData.generated.h"

class UAnimSequence;
class AItemBase;

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

//--------------------------------------------------------------
// ItemData - Data Asset
//--------------------------------------------------------------

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIC_Default		UMETA(DisplayName = "Default"),
	EIC_Consumable	UMETA(DisplayName = "Consumable"),
	EIC_Weapon		UMETA(DisplayName = "Weapon"),
	EIC_Equipment	UMETA(DisplayName = "Equipment"),

	EIC_Max			UMETA(Hidden)
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

USTRUCT(BlueprintType)
struct FItemInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	int ItemQualityRating;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemMaxDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemCurrentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	TSubclassOf<AActor> ClassToSpawn;

};

UCLASS(BlueprintType)
class RPGPROJECT_API UItemData : public UDataAsset
{
	GENERATED_BODY()

	// Create functions to get all this data?

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	int ItemQualityRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FCurrency ItemValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemMaxDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	float ItemCurrentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	bool bUseItemWeightForPhysics = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	bool bCanBeDropped = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	TSoftClassPtr<AItemBase> ClassToSpawn;
};

//--------------------------------------------------------------
// ItemEquipmentData - Data Asset
//--------------------------------------------------------------

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
	EET_TorsoAccessory	UMETA(DisplayName = "Torso Accessory"),
	EET_HeadAccessory	UMETA(DisplayName = "Head Accessory"),
	EET_NeckAccessory	UMETA(DisplayName = "Neck Accessory"),
	EET_ArmAccessory	UMETA(DisplayName = "Arm Accessory"),
	EET_RingAccessory	UMETA(DisplayName = "Ring Accessory"),
	
	EET_Max				UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FEquipmentInfo : public FItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
		float EquipmentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Defence")
		float PhysicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
		float MagicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
		float ElementalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
		float MentalDefence;

};

UCLASS()
class RPGPROJECT_API UItemEquipmentData : public UItemData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Info")
	EEquipmentType EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Defence")
	float PhysicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Defence")
	float MagicalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Defence")
	float ElementalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Defence")
	float MentalDefence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredDexterity;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredVitality;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredGrit;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredIntelligence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredWisdom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredCharisma;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Stat Requirement")
	int RequiredLuck;

};

//--------------------------------------------------------------
// ItemWeaponData - Data Asset
//--------------------------------------------------------------

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

USTRUCT(BlueprintType)
struct FWeaponInfo : public FEquipmentInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	TArray<USoundWave*> RandomAttackSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UAnimSequence* CombatIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	uint8 LightAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	uint8 HeavyAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName SheathSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	FName EquippedSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponDamage;

};

UCLASS()
class RPGPROJECT_API UItemWeaponData : public UItemEquipmentData
{
	GENERATED_BODY()

	// Create functions to get all this data?

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	TArray<USoundWave*> RandomAttackSoundArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Montages")
	UAnimSequence* CombatIdle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Attack Combos")
	uint8 LightAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Attack Combos")
	uint8 HeavyAttackComboLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Sockets")
	FName SheathSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Sockets")
	FName EquippedSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Montages")
	FName WeaponSheathMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info - Montages")
	FName WeaponUnSheathMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponCondition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Info")
	float WeaponDamage;
};

