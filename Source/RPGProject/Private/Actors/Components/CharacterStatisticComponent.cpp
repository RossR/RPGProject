// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/CharacterStatisticComponent.h"
#include "Actors/Components/HealthComponent.h"
#include "Actors/Components/StaminaComponent.h"

// Sets default values for this component's properties
UCharacterStatisticComponent::UCharacterStatisticComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TotalAttributePoints = 100;

	StrengthAttribute = 0;
	DexterityAttribute = 0;
	VitalityAttribute = 0;
	GritAttribute = 0;
	IntelligenceAttribute = 0;
	WisdomAttribute = 0;
	CharismaAttribute = 0;
	LuckAttribute = 0;

	TotalAttributePointsSpent = 0;

	bHaveStatisticsChanged = false;
	OldHealthPoints = 0;
}


// Called when the game starts
void UCharacterStatisticComponent::BeginPlay()
{
	Super::BeginPlay();

	ResetNewAttributes();

	UpdateCharacterStatistics();

	UpdateTotalAttributePointsSpent();
}


// Called every frame
void UCharacterStatisticComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCharacterStatistics();
	UpdateDisplayedCharacterStatistics();

	if (bHaveStatisticsChanged)
	{
		UpdateTotalAttributePointsSpent();

		int StatChangeHealthDifference = AdditionalCharacterStatistics.HealthPoints - OldHealthPoints;

		UHealthComponent* HealthComponent = Cast<UHealthComponent>(GetOwner()->GetComponentByClass(UHealthComponent::StaticClass()));

		if (HealthComponent)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Emerald, FString::FromInt(StatChangeHealthDifference));
			HealthComponent->SetMaxHealth(CharacterStatistics.HealthPoints);
			HealthComponent->HealDamage(StatChangeHealthDifference);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UCharacterStatComponent::TickComponent HealthComponent is NULL."));
		}
		
		UStaminaComponent* StaminaComponent = Cast<UStaminaComponent>(GetOwner()->GetComponentByClass(UStaminaComponent::StaticClass()));

		if (StaminaComponent)
		{
			StaminaComponent->SetMaxStamina(CharacterStatistics.StaminaPoints);

			StaminaComponent->TakeStaminaDamage(OldStaminaPoints - AdditionalCharacterStatistics.StaminaPoints);
		}


		bHaveStatisticsChanged = false;
	}

	OldHealthPoints = AdditionalCharacterStatistics.HealthPoints;
	OldStaminaPoints = AdditionalCharacterStatistics.StaminaPoints;

	ClampNewAttributes();
}

void UCharacterStatisticComponent::IncreaseAttribute()
{
	if (NewTotalAttributePointsSpent < GetTotalAttributePoints())
	{
		switch (SelectedAttributeType)
		{
			case EAttributeType::EAT_Strength:
				NewStrengthAttribute++;
				break;

			case EAttributeType::EAT_Dexterity:
				NewDexterityAttribute++;
				break;

			case EAttributeType::EAT_Vitality:
				NewVitalityAttribute++;
				break;

			case EAttributeType::EAT_Grit:
				NewGritAttribute++;
				break;

			case EAttributeType::EAT_Intelligence:
				NewIntelligenceAttribute++;
				break;

			case EAttributeType::EAT_Wisdom:
				NewWisdomAttribute++;
				break;

			case EAttributeType::EAT_Charisma:
				NewCharismaAttribute++;
				break;

			case EAttributeType::EAT_Luck:
				NewLuckAttribute++;
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("CharacterStatisticComponent::IncreaseAttribute No attribute type has been selected."));
				break;
		}

		if (!GetWorld()->GetTimerManager().IsTimerActive(AttributeButtonTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(AttributeButtonTimerHandle, this, &UCharacterStatisticComponent::IncreaseAttribute, 0.1f, true, 1.0f);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
	}
}

void UCharacterStatisticComponent::DecreaseAttribute()
{
	if (NewTotalAttributePointsSpent > GetTotalAttributePointsSpent())
	{
		switch (SelectedAttributeType)
		{
		case EAttributeType::EAT_Strength:
			if (NewStrengthAttribute == StrengthAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			} 
			else
			{
				NewStrengthAttribute--;
			}
			
			break;

		case EAttributeType::EAT_Dexterity:
			if (NewDexterityAttribute == DexterityAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewDexterityAttribute--;
			}
			break;

		case EAttributeType::EAT_Vitality:
			if (NewVitalityAttribute == VitalityAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewVitalityAttribute--;
			}
			break;

		case EAttributeType::EAT_Grit:
			if (NewGritAttribute == GritAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewGritAttribute--;
			}
			break;

		case EAttributeType::EAT_Intelligence:
			if (NewIntelligenceAttribute == IntelligenceAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewIntelligenceAttribute--;
			}
			break;

		case EAttributeType::EAT_Wisdom:
			if (NewWisdomAttribute == WisdomAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewWisdomAttribute--;
			}
			break;

		case EAttributeType::EAT_Charisma:
			if (NewCharismaAttribute == CharismaAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewCharismaAttribute--;
			}
			break;

		case EAttributeType::EAT_Luck:
			if (NewLuckAttribute == LuckAttribute)
			{
				GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
			}
			else
			{
				NewLuckAttribute--;
			}
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("CharacterStatisticComponent::DecreaseAttribute No attribute type has been selected."));
			break;
		}

		if (!GetWorld()->GetTimerManager().IsTimerActive(AttributeButtonTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(AttributeButtonTimerHandle, this, &UCharacterStatisticComponent::DecreaseAttribute, 0.1f, true, 1.0f);
		}

	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(AttributeButtonTimerHandle);
	}
}

void UCharacterStatisticComponent::ApplyNewAttributes()
{
	StrengthAttribute = NewStrengthAttribute;
	DexterityAttribute = NewDexterityAttribute;
	VitalityAttribute = NewVitalityAttribute;
	GritAttribute = NewGritAttribute;
	IntelligenceAttribute = NewIntelligenceAttribute;
	WisdomAttribute = NewWisdomAttribute;
	CharismaAttribute = NewCharismaAttribute;
	LuckAttribute = NewLuckAttribute;

	bHaveStatisticsChanged = true;
}

void UCharacterStatisticComponent::ResetNewAttributes()
{
	NewStrengthAttribute = StrengthAttribute;
	NewDexterityAttribute = DexterityAttribute;
	NewVitalityAttribute = VitalityAttribute;
	NewGritAttribute = GritAttribute;
	NewIntelligenceAttribute = IntelligenceAttribute;
	NewWisdomAttribute = WisdomAttribute;
	NewCharismaAttribute = CharismaAttribute;
	NewLuckAttribute = LuckAttribute;
}

int UCharacterStatisticComponent::GetAttribute(EAttributeType Attribute)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_Strength:
		return StrengthAttribute;
		break;

	case EAttributeType::EAT_Dexterity:
		return DexterityAttribute;
		break;

	case EAttributeType::EAT_Vitality:
		return VitalityAttribute;
		break;

	case EAttributeType::EAT_Grit:
		return GritAttribute;
		break;

	case EAttributeType::EAT_Intelligence:
		return IntelligenceAttribute;
		break;

	case EAttributeType::EAT_Wisdom:
		return WisdomAttribute;
		break;

	case EAttributeType::EAT_Charisma:
		return CharismaAttribute;
		break;

	case EAttributeType::EAT_Luck:
		return LuckAttribute;
		break;

	default:
		return 0;
		break;
	}
}

int UCharacterStatisticComponent::GetNewAttribute(EAttributeType Attribute)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_Strength:
		return NewStrengthAttribute;
		break;

	case EAttributeType::EAT_Dexterity:
		return NewDexterityAttribute;
		break;

	case EAttributeType::EAT_Vitality:
		return NewVitalityAttribute;
		break;

	case EAttributeType::EAT_Grit:
		return NewGritAttribute;
		break;

	case EAttributeType::EAT_Intelligence:
		return NewIntelligenceAttribute;
		break;

	case EAttributeType::EAT_Wisdom:
		return NewWisdomAttribute;
		break;

	case EAttributeType::EAT_Charisma:
		return NewCharismaAttribute;
		break;

	case EAttributeType::EAT_Luck:
		return NewLuckAttribute;
		break;

	default:
		return 0;
		break;
	}
}

void UCharacterStatisticComponent::SetAttribute(EAttributeType Attribute, int NewValue)
{
	switch (Attribute)
	{
	case EAttributeType::EAT_Strength:
		StrengthAttribute = NewValue;
		break;

	case EAttributeType::EAT_Dexterity:
		DexterityAttribute = NewValue;
		break;

	case EAttributeType::EAT_Vitality:
		VitalityAttribute = NewValue;
		break;

	case EAttributeType::EAT_Grit:
		GritAttribute = NewValue;
		break;

	case EAttributeType::EAT_Intelligence:
		IntelligenceAttribute = NewValue;
		break;

	case EAttributeType::EAT_Wisdom:
		WisdomAttribute = NewValue;
		break;

	case EAttributeType::EAT_Charisma:
		CharismaAttribute = NewValue;
		break;

	case EAttributeType::EAT_Luck:
		LuckAttribute = NewValue;
		break;

	default:
		break;
	}
}

void UCharacterStatisticComponent::UpdateTotalAttributePointsSpent()
{
	TotalAttributePointsSpent = StrengthAttribute + DexterityAttribute + VitalityAttribute + GritAttribute + IntelligenceAttribute + WisdomAttribute + CharismaAttribute + LuckAttribute;
}

void UCharacterStatisticComponent::UpdateCharacterStatistics()
{

	AdditionalCharacterStatistics.HealthPoints = (10 * StrengthAttribute) + (10 * DexterityAttribute) + (20 * VitalityAttribute) + (10 * GritAttribute);
	AdditionalCharacterStatistics.StaminaPoints = (5 * StrengthAttribute) + (10 * DexterityAttribute) + (5 * VitalityAttribute) + (5 * GritAttribute);
	AdditionalCharacterStatistics.CarryWeight = (0.5f * StrengthAttribute) + (0.1f * DexterityAttribute) + (0.1f * VitalityAttribute) + (0.1f * GritAttribute);

	CharacterStatistics.HealthPoints = BaseCharacterStatistics.HealthPoints + AdditionalCharacterStatistics.HealthPoints;
	CharacterStatistics.StaminaPoints = BaseCharacterStatistics.StaminaPoints + AdditionalCharacterStatistics.StaminaPoints;
	CharacterStatistics.CarryWeight = BaseCharacterStatistics.CarryWeight + AdditionalCharacterStatistics.CarryWeight;
}

void UCharacterStatisticComponent::UpdateDisplayedCharacterStatistics()
{
	NewTotalAttributePointsSpent = NewStrengthAttribute + NewDexterityAttribute + NewVitalityAttribute + NewGritAttribute + NewIntelligenceAttribute + NewWisdomAttribute + NewCharismaAttribute + NewLuckAttribute;

	DisplayedCharacterStatistics.HealthPoints = BaseCharacterStatistics.HealthPoints + (10 * NewStrengthAttribute) + (10 * NewDexterityAttribute) + (20 * NewVitalityAttribute) + (10 * NewGritAttribute);
	DisplayedCharacterStatistics.StaminaPoints = BaseCharacterStatistics.StaminaPoints + (5 * NewStrengthAttribute) + (10 * NewDexterityAttribute) + (5 * NewVitalityAttribute) + (5 * NewGritAttribute);
	DisplayedCharacterStatistics.CarryWeight = BaseCharacterStatistics.CarryWeight + (0.5f * NewStrengthAttribute) + (0.1f * NewDexterityAttribute) + (0.1f * NewVitalityAttribute) + (0.1f * NewGritAttribute);
}

void UCharacterStatisticComponent::ClampNewAttributes()
{
	NewStrengthAttribute = FMath::Clamp(NewStrengthAttribute, StrengthAttribute, 9999);
	NewDexterityAttribute = FMath::Clamp(NewDexterityAttribute, DexterityAttribute, 9999);
	NewVitalityAttribute = FMath::Clamp(NewVitalityAttribute, VitalityAttribute, 9999);
	NewGritAttribute = FMath::Clamp(NewGritAttribute, GritAttribute, 9999);
	NewIntelligenceAttribute = FMath::Clamp(NewIntelligenceAttribute, IntelligenceAttribute, 9999);
	NewWisdomAttribute = FMath::Clamp(NewWisdomAttribute, WisdomAttribute, 9999);
	NewCharismaAttribute = FMath::Clamp(NewCharismaAttribute, CharismaAttribute, 9999);
	NewLuckAttribute = FMath::Clamp(NewLuckAttribute, LuckAttribute, 9999);
}