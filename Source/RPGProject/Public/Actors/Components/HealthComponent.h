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

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float Damage) { CurrentHealth -= Damage; }
	UFUNCTION(BlueprintCallable)
	void HealDamage(float HealAmount) { CurrentHealth += HealAmount; }

	UFUNCTION(BlueprintCallable)
	bool IsDead() { return CurrentHealth <= FLT_EPSILON; }

	UFUNCTION(BlueprintCallable)
	float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentHealth() { return CurrentHealth; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(float NewHealth) { CurrentHealth = NewHealth; }
	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float NewMaxHealth) { MaxHealth = NewMaxHealth; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float MaxHealth = 1000;

	UPROPERTY(VisibleAnywhere)
	float CurrentHealth = 0.0f;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
