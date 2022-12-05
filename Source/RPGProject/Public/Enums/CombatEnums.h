// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//#include "Enums/CombatEnums.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	AT_None				UMETA(DisplayName = "None"),
	AT_LightAttack		UMETA(DisplayName = "Light Attack"),
	AT_HeavyAttack		UMETA(DisplayName = "Heavy Attack"),
	AT_LightFinisher	UMETA(DisplayName = "Light Finisher"),
	AT_HeavyFinisher	UMETA(DisplayName = "Heavy Finisher"),

	AT_MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAttackBlockerType : uint8
{
	None		UMETA(DisplayName = "None"),
	Box			UMETA(DisplayName = "Box"),
	Capsule		UMETA(DisplayName = "Capsule"),

	MAX			UMETA(Hidden)

};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	CS_AtEase UMETA(DisplayName = "At Ease"),
	CS_CombatReady UMETA(DisplayName = "Combat Ready"),

	CS_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECombatWeaponStance : uint8
{
	CWS_None		UMETA(DisplayName = "None"),
	CWS_Mainhand	UMETA(DisplayName = "Mainhand"),
	CWS_Offhand		UMETA(DisplayName = "Offhand"),

	CWS_MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDodgeType : uint8
{
	DT_None				UMETA(DisplayName = "None"),
	DT_Light			UMETA(DisplayName = "Light"),
	DT_Medium			UMETA(DisplayName = "Medium"),
	DT_Heavy			UMETA(DisplayName = "Heavy"),
	DT_OverEncumbered	UMETA(DisplayName = "Over-Encumbered"),

	DT_MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	None			UMETA(DisplayName = "None", Hidden),
	Single_Reload	UMETA(DisplayName = "Single Reload"),
	Full_Reload		UMETA(DisplayName = "Full Reload"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETraceType : uint8
{
	None			UMETA(Hidden),
	Multi_Line		UMETA(DisplayName = "Multi Line"),
	Multi_Box		UMETA(DisplayName = "Multi Box"),
	Multi_Sphere	UMETA(DisplayName = "Multi Sphere"),
	Unique_Trace	UMETA(DisplayName = "Unique Trace"),

	MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponToUse : uint8
{
	None		UMETA(DisplayName = "None", Hidden),
	Mainhand	UMETA(DisplayName = "Mainhand"),
	Offhand		UMETA(DisplayName = "Offhand"),
	Both		UMETA(DisplayName = "Both Hands"),

	MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EStaminaExhaustionType : uint8
{
	Default		UMETA(DisplayName = "Default"),
	GuardBreak	UMETA(DisplayName = "Guard Break"),

	MAX			UMETA(Hidden)
};