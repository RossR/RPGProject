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

	void TakeStaminaDamage(float Damage);
	bool IsStaminaExhausted() { return CurrentStamina <= FLT_EPSILON; }

	UFUNCTION(BlueprintCallable)
	float GetMaxStamina() { return MaxStamina; }
	UFUNCTION(BlueprintCallable)
	float GetCurrentStamina() { return CurrentStamina; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentStamina(float Stamina) { CurrentStamina = Stamina; }
	UFUNCTION(BlueprintCallable)
	void RegenerateStamina();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditAnywhere)
	float MaxStamina = 400;

	UPROPERTY(VisibleAnywhere)
	float CurrentStamina = 0.0f;

	UPROPERTY(EditAnywhere)
	float StaminaRegenPerInterval = 1.0f;

	float StaminaRegenInterval;

	UPROPERTY(EditAnywhere)
	float StaminaRegenDelay = 2.5f;

	UPROPERTY(VisibleAnywhere)
	bool IsRegeneratingStamina = true;

	FTimerHandle StaminaRegenTimerHandle;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
