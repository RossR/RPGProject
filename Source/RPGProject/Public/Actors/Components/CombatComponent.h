// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "CombatComponent.generated.h"

class ACharacter;
//class UEquipmentComponent;
class UHealthComponent;
class UStaminaComponent;
class AItemWeapon;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	AT_None			UMETA(DisplayName = "None"),
	AT_LightAttack	UMETA(DisplayName = "Light Attack"),
	AT_HeavyAttack	UMETA(DisplayName = "Heavy Attack"),

	AT_MAX			UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDodgeType : uint8
{
	DT_None				UMETA(DisplayName = "None"),
	DT_Light			UMETA(DisplayName = "Light"),
	DT_Medium			UMETA(DisplayName = "Medium"),
	DT_Heavy			UMETA(DisplayName = "Heavy"),
	DT_OverEncumbered	UMETA(DisplayName = "Over-Encumbered"),

	DT_MAX				UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	CS_AtEase UMETA(DisplayName = "At Ease"),
	CS_CombatReady UMETA(DisplayName = "Combat Ready"),

	CS_Max UMETA(Hidden)
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	// --- FUNCTIONS --- //

	UFUNCTION(BlueprintCallable)
	// Set the combat state
	void SetCombatState(ECombatState NewState) { CombatState = NewState; }

	UFUNCTION(BlueprintPure)
	// Get the combat state
	const ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable)
	void SetAttackType(EAttackType NewAttackType) { AttackType = NewAttackType; }

	UFUNCTION(BlueprintPure)
	const EAttackType GetAttackType() { return AttackType; }

	UFUNCTION(BlueprintCallable)
	void CharacterAttack(EAttackType CharacterAttackType);

	UFUNCTION(BlueprintCallable)
	void StartAttackSequence(EAttackType SequenceAttackType);
	
	UFUNCTION(BlueprintCallable)
	void ToggleCombatState();

	UFUNCTION(BlueprintCallable)
	bool UnSheathWeapon();
	
	UFUNCTION(BlueprintCallable)
	bool SheathWeapon();

	// --- VARIABLES --- // 

protected:

	// --- FUNCTIONS --- //

	UFUNCTION()
	void OnSheathingMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnSheathingMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnSheathingNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnSheathingNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);
	UFUNCTION()
	void OnAttackNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UFUNCTION()
	void UnbindSheathingMontage();
	UFUNCTION()
	void UnbindAttackMontage();


	// --- VARIABLES --- // 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animations")
	UAnimMontage* SheathMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animations")
	UAnimMontage* UnSheathMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Animations")
	UAnimMontage* WeaponMontage;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;

	// Store values of the curves used in the attack montages
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Curves")
	float QueueAttackWindowCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Curves")
	float EnableHitCurve;

private:

	// --- FUNCTIONS --- //

	void UpdateCurveFloats();
	void UpdateCombatBooleans();

	void SetupNextAttack(); // Formerly EndAttackWindow
	void EndAttackSequence();
	void AttackTracing();
	UItemWeaponData* GetEquippedWeaponData();

	// --- VARIABLES --- // 


	// Reference to owning character
	ACharacter* CharacterRef;

	// Reference to the owning character's equipment component (if one exists)
	UEquipmentComponent* EquipmentComponentRef;

	UHealthComponent* HealthComponentRef;

	UStaminaComponent* StaminaComponentRef;

	// Reference to the owning character's mesh
	USkeletalMeshComponent* MeshComponentRef;

	// Reference to the owning character's animinstance
	UAnimInstance* CharacterAnimInstance;

	// Tracks how many attacks the character has done in the current attack sequence
	uint8 AttackCount;

	// Stores what attack type the current attack is
	EAttackType AttackType;

	// Stores what type of dodge the character can currently perform
	EDodgeType DodgeType;

	// Stores value for current combat state
	ECombatState CombatState;

	// Stores all actors hit when attacking
	TArray<TWeakObjectPtr<class AActor>> HitActorArray;

	// Stores name of the current montage section being played in the attack sequence
	FName CurrentSectionInMontage;

	// Stores name of the next montage section to play in the attack sequence
	FName NextSectionInMontage;

	// Stores montage section for sheathing the currently equipped weapon
	FName EquippedWeaponSheathSection;
	// Stores montage section for unsheathing the currently equipped weapon
	FName EquippedWeaponUnSheathSection;

	

	bool bIsInCombat;
	bool bIsAttackQueued;
	bool bIsInAttackSequence;
	bool bIsInAttackWindow;
	bool bIsInAttackWindUp;
	
};
