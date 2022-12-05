// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaminaComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void ReduceCurrentStamina(float Damage);

	UFUNCTION(BlueprintPure)
	bool IsStaminaExhausted() { return CurrentStamina <= FLT_EPSILON; }

	UFUNCTION(BlueprintPure)
	float GetMaxStamina() { return MaxStamina; }
	UFUNCTION(BlueprintPure)
	float GetCurrentStamina() { return CurrentStamina; }
	UFUNCTION(BlueprintCallable)
	void SetMaxStamina(float NewMaxStamina) { MaxStamina = NewMaxStamina; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentStamina(float Stamina) { CurrentStamina = Stamina; }
	UFUNCTION(BlueprintCallable)
	void RegenerateStamina();
	UFUNCTION(BlueprintCallable)
	void ResetStaminaRegenDelay();

	UFUNCTION(BlueprintPure)
	float GetStaminaRegenMultiplier() { return StaminaRegenMultiplier; }
	UFUNCTION(BlueprintCallable)
	void SetStaminaRegenMultiplier(float NewFloat) { StaminaRegenMultiplier = NewFloat; }

	UFUNCTION(BlueprintPure)
	float GetStaminaRegenDelayMultiplier() { return StaminaRegenDelayMultiplier; }
	UFUNCTION(BlueprintCallable)
	void SetStaminaRegenDelayMultiplier(float NewFloat) { StaminaRegenDelayMultiplier = NewFloat; }

protected:

	UPROPERTY(EditAnywhere)
	float MaxStamina = 400;

	UPROPERTY(VisibleAnywhere)
	float CurrentStamina = 0.f;

	UPROPERTY(EditAnywhere)
	float TimeToFullyRegenStamina = 5.f;

	UPROPERTY(VisibleAnywhere)
	float StaminaRegenMultiplier = 1.f;

	UPROPERTY(EditAnywhere)
	float StaminaRegenDelay = 2.5f;

	UPROPERTY(VisibleAnywhere)
	float StaminaRegenDelayMultiplier = 1.f;

	UPROPERTY(VisibleAnywhere)
	bool IsRegeneratingStamina = true;

	FTimerHandle StaminaRegenTimerHandle;

	UPROPERTY(VisibleAnywhere)
	float StaminaRegenPerFrame = 0.f;
		
};
