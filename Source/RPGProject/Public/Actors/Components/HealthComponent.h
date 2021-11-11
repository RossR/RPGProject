// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	void TakeDamage(float Damage) { CurrentHealth -= Damage; }
	bool IsDead() { return CurrentHealth <= FLT_EPSILON; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float Health) { CurrentHealth = Health; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth = 0.0f;

public:	

		
};
