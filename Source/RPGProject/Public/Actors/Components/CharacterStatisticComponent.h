// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CharacterStatisticComponent.generated.h"

UENUM(BlueprintType)
enum class EAttributeType : uint8
{
	EAT_None			UMETA(DisplayName="None"),
	EAT_Strength		UMETA(DisplayName = "Strength"),
	EAT_Dexterity		UMETA(DisplayName = "Dexterity"),
	EAT_Vitality		UMETA(DisplayName = "Vitality"),
	EAT_Grit			UMETA(DisplayName = "Grit"),
	EAT_Intelligence	UMETA(DisplayName = "Intelligence"),
	EAT_Wisdom			UMETA(DisplayName = "Wisdom"),
	EAT_Charisma		UMETA(DisplayName = "Charisma"),
	EAT_Luck			UMETA(DisplayName = "Luck"),

	EAT_Max				UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCharacterStatistics
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int HealthPoints;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int StaminaPoints;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UCharacterStatisticComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatisticComponent();

	UFUNCTION(BlueprintCallable)
	FCharacterStatistics GetCharacterStatistics() { return CharacterStatistics; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void IncreaseAttribute();

	UFUNCTION(BlueprintCallable)
	void DecreaseAttribute();

	UFUNCTION(BlueprintCallable)
	void ApplyNewAttributes();

	UFUNCTION(BlueprintCallable)
	void ResetNewAttributes();

	UFUNCTION(BlueprintCallable)
	// Get the character's total amount of attribute points
	int GetTotalAttributePoints() { return TotalAttributePoints; }
	UFUNCTION(BlueprintCallable)
	// Get the character's total amount of spent attribute points 
	int GetTotalAttributePointsSpent() { return TotalAttributePointsSpent; }

	UFUNCTION(BlueprintCallable)
	// Returns true of the character's statistics have changed
	bool HaveStatisticsChanged() { return bHaveStatisticsChanged; }
	UFUNCTION(BlueprintCallable)
	// Set value of bHaveStatisticsChanged
	void SetHaveStatisticsChanged(bool bActive) { bHaveStatisticsChanged = bActive; }

	UFUNCTION(BlueprintCallable)
	int GetAttribute(EAttributeType Attribute);

	UFUNCTION(BlueprintCallable)
	int GetNewAttribute(EAttributeType Attribute);

	UFUNCTION(BlueprintCallable)
	void SetAttribute(EAttributeType Attribute, int NewValue);

	UFUNCTION(BlueprintCallable)
	// Get the value of the Strength attribute
	int GetStrengthAttribute() { return StrengthAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Strength attribute
	void SetStrengthAttribute(int NewValue) { StrengthAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Dexterity attribute
	int GetDexterityAttribute() { return DexterityAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Dexterity attribute
	void SetDexterityAttribute(int NewValue) { DexterityAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Vitality attribute
	int GetVitalityAttribute() { return VitalityAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Vitality attribute
	void SetVitalityAttribute(int NewValue) { VitalityAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Grit attribute
	int GetGritAttribute() { return GritAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Grit attribute
	void SetGritAttribute(int NewValue) { GritAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Intelligence attribute
	int GetIntelligenceAttribute() { return IntelligenceAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Intelligence attribute
	void SetIntelligenceAttribute(int NewValue) { IntelligenceAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Wisdom attribute
	int GetWisdomAttribute() { return WisdomAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Wisdom attribute
	void SetWisdomAttribute(int NewValue) { WisdomAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Charisma attribute
	int GetCharismaAttribute() { return CharismaAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Charisma attribute
	void SetCharismaAttribute(int NewValue) { CharismaAttribute = NewValue; }

	UFUNCTION(BlueprintCallable)
	// Get the value of the Luck attribute
	int GetLuckStat() { return LuckAttribute; }
	UFUNCTION(BlueprintCallable)
	// Set the value of the Luck attribute
	void SetLuckStat(int NewValue) { LuckAttribute = NewValue; }

public:


protected:

	//-------------------------------------
	// Character Attribute - Variables
	//-------------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Attributes")
	int TotalAttributePoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character Attributes")
	int TotalAttributePointsSpent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int StrengthAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int DexterityAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int VitalityAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int GritAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int IntelligenceAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int WisdomAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int CharismaAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Attributes")
	int LuckAttribute;

	//-------------------------------------
	// New Character Attributes - Variables
	//-------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "New Character Attributes")
	EAttributeType SelectedAttributeType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "New Character Attributes")
	FTimerHandle AttributeButtonTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewTotalAttributePointsSpent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewStrengthAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewDexterityAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewVitalityAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewGritAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewIntelligenceAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewWisdomAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewCharismaAttribute;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Attributes")
	int NewLuckAttribute;

	//-----

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Statistics")
		FCharacterStatistics BaseCharacterStatistics
	{
		// Health Points
		300,
		// Stamina Points
		400,
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	FCharacterStatistics CharacterStatistics
	{
		// Health Points
		300,
		// Stamina Points
		400,
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int OldHealthPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int OldStaminaPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int AdditionalHealthPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int AdditionalStaminaPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Statistics")
	bool bHaveStatisticsChanged = false;
};
