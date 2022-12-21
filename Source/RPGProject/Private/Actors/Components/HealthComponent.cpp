// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealthPoints = MaxHealthPoints;
	BodyPartUpdate();

	// ...
	
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentHealthPointsUpdate();

	BodyPartUpdate();
}

void UHealthComponent::TakeDamage(float Damage, FName InBodyPartName, bool bUseDamageModifier)
{
	if (!BodyPartMap.IsEmpty())
	{
		if (InBodyPartName == "All")
		{
			TArray<FName> BodyPartNameArray;
			BodyPartMap.GenerateKeyArray(BodyPartNameArray);
			TArray<FName> FunctioningBodyPartsNameArray;
			BodyPartMap.GenerateKeyArray(FunctioningBodyPartsNameArray);

			float FunctioningBodyPartHPPercentage = 0.f;

			for (auto& BodyPartName : BodyPartNameArray)
			{
				if (BodyPartMap[BodyPartName].BodyPartCurrentHP > 0.f) { FunctioningBodyPartHPPercentage += BodyPartMap[BodyPartName].MaxHPPercentage; }
				else { FunctioningBodyPartsNameArray.Remove(BodyPartName); }
			}

			for (auto& BodyPartName : FunctioningBodyPartsNameArray)
			{
				if (!BodyPartMap.Contains(BodyPartName)) { continue; }

				if (BodyPartMap[BodyPartName].BodyPartCurrentHP > 0.f)
				{
					float DamagePortion = Damage * (BodyPartMap[BodyPartName].MaxHPPercentage / FunctioningBodyPartHPPercentage);
					TakeDamage(DamagePortion, BodyPartName, false);
					//BodyPartMap[BodyPartName].BodyPartCurrentHP -= SplitDamage;
				}
			}
		}
		else if (BodyPartMap.Contains(InBodyPartName))
		{
			const float ModifiedDamage = bUseDamageModifier ? truncf(Damage * BodyPartMap[InBodyPartName].DamageModifier) : Damage;

			if (BodyPartMap[InBodyPartName].BodyPartCurrentHP > 0.f)
			{
				
				BodyPartMap[InBodyPartName].BodyPartCurrentHP -= ModifiedDamage;
			}
			else
			{
				TakeDamage(ModifiedDamage, "All", false);
			}
		}
	}
	else
	{
		CurrentHealthPoints -= Damage;
	}
}

void UHealthComponent::HealDamage(float HealAmount, FName InBodyPartName)
{
	if (InBodyPartName == "All")
	{
		TArray<FName> BodyPartNameArray;
		BodyPartMap.GenerateKeyArray(BodyPartNameArray);

		for (int i = 0; i < BodyPartNameArray.Max(); i++)
		{
			BodyPartMap[BodyPartNameArray[i]].BodyPartCurrentHP += (HealAmount * BodyPartMap[BodyPartNameArray[i]].MaxHPPercentage);
		}
	}
	else if (BodyPartMap.Contains(InBodyPartName))
	{
		BodyPartMap[InBodyPartName].BodyPartCurrentHP += HealAmount;
	}
}

TMap<FName, FBodyPartInfo> UHealthComponent::GetBodyPartMap()
{
	return BodyPartMap;
}

bool UHealthComponent::AddToBodyPartMap(FName BoneName, FBodyPartInfo DamageModifier)
{
	BodyPartMap.Emplace(BoneName, DamageModifier);
	return BodyPartMap.Contains(BoneName);
}

bool UHealthComponent::RemoveFromBodyPartMap(FName BoneName)
{
	BodyPartMap.Contains(BoneName) ? BodyPartMap.Remove(BoneName) : NULL;
	return !BodyPartMap.Contains(BoneName);
}

FName UHealthComponent::GetBodyPartFromBone(FName BoneName)
{
	TArray<FName> BodyPartNameArray;
	BodyPartMap.GenerateKeyArray(BodyPartNameArray);

	for (int i = 0; i < BodyPartMap.Num(); i++)
	{
		if (BodyPartMap[BodyPartNameArray[i]].BoneArray.Contains(BoneName))
		{
			return BodyPartNameArray[i];
		}
	}
	return "";
}

void UHealthComponent::BodyPartUpdate()
{
	TArray<FName> BodyPartNameArray;
	BodyPartMap.GenerateKeyArray(BodyPartNameArray);
	if (!BodyPartNameArray.IsEmpty())
	{
		for (int i = 0; i < BodyPartNameArray.Num(); i++)
		{
			if (BodyPartMap[BodyPartNameArray[i]].MaxHPPercentage > 0.f)
			{
				BodyPartMap[BodyPartNameArray[i]].BodyPartMaxHP = MaxHealthPoints * BodyPartMap[BodyPartNameArray[i]].MaxHPPercentage;
			}
			BodyPartMap[BodyPartNameArray[i]].BodyPartCurrentHP = FMath::Clamp(BodyPartMap[BodyPartNameArray[i]].BodyPartCurrentHP, 0.0f, BodyPartMap[BodyPartNameArray[i]].BodyPartMaxHP);
		}
	}
}

void UHealthComponent::CurrentHealthPointsUpdate()
{
	TArray<FName> BodyPartNameArray;
	BodyPartMap.GenerateKeyArray(BodyPartNameArray);

	
	if (!BodyPartNameArray.IsEmpty())
	{
		float NewCurrentHealth = 0.f;

		for (int i = 0; i < BodyPartNameArray.Num(); i++)
		{
			if (BodyPartMap[BodyPartNameArray[i]].bIsVitalBodyPart && BodyPartMap[BodyPartNameArray[i]].BodyPartCurrentHP <= 0.f)
			{
				CurrentHealthPoints = 0.f;
				return;
			}
			NewCurrentHealth += BodyPartMap[BodyPartNameArray[i]].BodyPartCurrentHP;
		}
		

		CurrentHealthPoints = FMath::Clamp(NewCurrentHealth, 0.0f, MaxHealthPoints);
	}
	
}