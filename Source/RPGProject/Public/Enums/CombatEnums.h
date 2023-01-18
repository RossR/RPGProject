// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None				UMETA(DisplayName = "None"),
	LightAttack			UMETA(DisplayName = "Light Attack"),
	HeavyAttack			UMETA(DisplayName = "Heavy Attack"),
	LightFinisher		UMETA(DisplayName = "Light Finisher"),
	HeavyFinisher		UMETA(DisplayName = "Heavy Finisher"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EAttackBlockerType : uint8
{
	None				UMETA(DisplayName = "None"),
	Box					UMETA(DisplayName = "Box"),
	Capsule				UMETA(DisplayName = "Capsule"),

	MAX					UMETA(Hidden)

};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	AtEase				UMETA(DisplayName = "At Ease"),
	CombatReady			UMETA(DisplayName = "Combat Ready"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECombatWeaponStance : uint8
{
	None				UMETA(DisplayName = "None"),
	Mainhand			UMETA(DisplayName = "Mainhand"),
	Offhand				UMETA(DisplayName = "Offhand"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDodgeType : uint8
{
	None				UMETA(DisplayName = "None"),
	Light				UMETA(DisplayName = "Light"),
	Medium				UMETA(DisplayName = "Medium"),
	Heavy				UMETA(DisplayName = "Heavy"),
	OverEncumbered		UMETA(DisplayName = "Over-Encumbered"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EReloadType : uint8
{
	None				UMETA(DisplayName = "None", Hidden),
	Single_Reload		UMETA(DisplayName = "Single Reload"),
	Full_Reload			UMETA(DisplayName = "Full Reload"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETraceType : uint8
{
	None				UMETA(Hidden),
	Multi_Line			UMETA(DisplayName = "Multi Line"),
	Multi_Box			UMETA(DisplayName = "Multi Box"),
	Multi_Sphere		UMETA(DisplayName = "Multi Sphere"),
	Unique_Trace		UMETA(DisplayName = "Unique Trace"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponToUse : uint8
{
	None				UMETA(DisplayName = "None", Hidden),
	Mainhand			UMETA(DisplayName = "Mainhand"),
	Offhand				UMETA(DisplayName = "Offhand"),
	Both				UMETA(DisplayName = "Both Hands"),

	MAX					UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EStaminaExhaustionType : uint8
{
	Default				UMETA(DisplayName = "Default"),
	GuardBreak			UMETA(DisplayName = "Guard Break"),

	MAX					UMETA(Hidden)
};