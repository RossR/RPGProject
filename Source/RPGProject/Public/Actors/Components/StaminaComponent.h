// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

class URPGProjectAnimInstance;

UENUM(BlueprintType)
enum class EStaminaDrainType : uint8
{
	None		UMETA(DisplayName = "None"),
	Sprint		UMETA(DisplayName = "Sprint"),
	MAX			UMETA(Hidden)
};

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

public:		// --- FUNCTIONS --- \\

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const { return MaxStamina; }
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetMaxStamina(float NewMaxStamina) { MaxStamina = NewMaxStamina; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetCurrentStamina(float Stamina) { CurrentStamina = Stamina; }
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void ReduceCurrentStamina(float Damage);
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void DrainStaminaPerSecond(EStaminaDrainType StaminaDrainType);

	//------------------
	// Stamina Exhaustion
	//------------------

	UFUNCTION(BlueprintPure, Category = "Stamina|Stamina Exhaustion")
	bool IsStaminaExhausted();

	//------------------
	// Stamina Regeneration
	//------------------

	UFUNCTION(BlueprintPure, Category = "Stamina|Stamina Regeneration")
	bool IsStaminaRegenerating() { return bIsStaminaRegenerating; }

	UFUNCTION(BlueprintPure, Category = "Stamina|Stamina Regeneration")
	float GetStaminaRegenMultiplier() const { return StaminaRegenMultiplier; }
	UFUNCTION(BlueprintCallable, Category = "Stamina|Stamina Regeneration")
	void SetStaminaRegenMultiplier(float NewFloat) { StaminaRegenMultiplier = NewFloat; }

	UFUNCTION(BlueprintPure, Category = "Stamina|Stamina Regeneration")
	float GetStaminaRegenDelayMultiplier() const { return StaminaRegenDelayMultiplier; }
	UFUNCTION(BlueprintCallable, Category = "Stamina|Stamina Regeneration")
	void SetStaminaRegenDelayMultiplier(float NewFloat) { StaminaRegenDelayMultiplier = NewFloat; }

	UFUNCTION(BlueprintCallable, Category = "Stamina|Stamina Regeneration")
	void StartStaminaRegeneration();
	UFUNCTION(BlueprintCallable, Category = "Stamina|Stamina Regeneration")
	void StopStaminaRegeneration();

public: 	// --- VARIABLES --- \\



protected:	// --- FUNCTIONS --- \\

	void StaminaRegenTimerFinished();

	void RegenerateStamina();

protected:	// --- VARIABLES --- \\

	URPGProjectAnimInstance* OwnerAnimInstance;

	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float CurrentStamina = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Stamina")
	float MaxStamina = 400;

	//------------------
	// Stamina Drain
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Stamina Drain")
	TMap<EStaminaDrainType, float> StaminaDrainTypeMap;

	//------------------
	// Stamina Exhaustion
	//------------------

	UPROPERTY(VisibleAnywhere, Category = "Stamina|Stamina Exhaustion")
	bool bHasStaminaBeenExhausted = false;

	UPROPERTY(EditAnywhere, Category = "Stamina|Stamina Exhaustion")
	float ExhaustionRecoveryPercentage = 0.25f;

	//------------------
	// Stamina Regeneration
	//------------------

	FTimerHandle StaminaRegenTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Stamina|Stamina Regeneration")
	bool bIsStaminaRegenerating = true;

	UPROPERTY(VisibleAnywhere, Category = "Stamina|Stamina Regeneration")
	float StaminaRegenPerFrame = 0.f;

	UPROPERTY(EditAnywhere, Category = "Stamina|Stamina Regeneration")
	float StaminaRegenDelay = 2.5f;

	UPROPERTY(EditAnywhere, Category = "Stamina|Stamina Regeneration")
	float TimeToFullyRegenStamina = 5.f;

	UPROPERTY(VisibleAnywhere, Category = "Stamina|Stamina Regeneration")
	float StaminaRegenMultiplier = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Stamina|Stamina Regeneration")
	float StaminaRegenDelayMultiplier = 1.f;
		
};
