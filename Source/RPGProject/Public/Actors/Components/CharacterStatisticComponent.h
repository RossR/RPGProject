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
	int HealthPoints = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int StaminaPoints = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	float CarryWeight = 0.f;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UCharacterStatisticComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterStatisticComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//-------------------------------------
	// Button Functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	// Button Function - Increase the selected attribute by 1
	void IncreaseAttribute();

	UFUNCTION(BlueprintCallable)
	// Button Function - Decrease the selected attribute by 1
	void DecreaseAttribute();

	UFUNCTION(BlueprintCallable)
	// Button Function - Save the changes to the character's attributes
	void ApplyNewAttributes();

	UFUNCTION(BlueprintCallable)
	// Button Function - Clear the changes to the character's attributes
	void ResetNewAttributes();

	//-------------------------------------
	// Get Character Statistic Functions
	//-------------------------------------

	UFUNCTION(BlueprintCallable)
	FCharacterStatistics GetCharacterStatistics() { return CharacterStatistics; }

	UFUNCTION(BlueprintCallable)
	FCharacterStatistics GetDisplayedCharacterStatistics() { return DisplayedCharacterStatistics; }

	//----------------

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
	// Get the value of the selected attribute
	int GetAttribute(EAttributeType Attribute);

	UFUNCTION(BlueprintCallable)
	// Get the new value of the selected attribute
	int GetNewAttribute(EAttributeType Attribute);

	UFUNCTION(BlueprintCallable)
	// Set the value of the selected attribute
	void SetAttribute(EAttributeType Attribute, int NewValue);

protected:

	void UpdateTotalAttributePointsSpent();

	void UpdateCharacterStatistics();

	void UpdateDisplayedCharacterStatistics();

	void ClampNewAttributes();

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

	//-------------------------------------
	// Character Statistics - Variables
	//-------------------------------------

	// Base values for the character's statistics, will be determined by "class" in the future
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Character Statistics")
	FCharacterStatistics BaseCharacterStatistics
	{
		// Health Points
		300,
		// Stamina Points
		400,
		// Carry Weight
		25.f,
	};

	// Added values for the character's statistics, scales with attributes
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Character Statistics")
	FCharacterStatistics AdditionalCharacterStatistics {};

	// Current values for the character's statistics
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	FCharacterStatistics CharacterStatistics
	{
		// Health Points
		300,
		// Stamina Points
		400,
		// Carry Weight
		25.f,
	};

	// New values for the character's statistics, determined by attribute increases the player is planning on applying
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "New Character Statistics")
	FCharacterStatistics DisplayedCharacterStatistics{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int OldHealthPoints;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Statistics")
	int OldStaminaPoints;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Statistics")
	bool bHaveStatisticsChanged = true;
};
