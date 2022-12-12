// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "RPGPlayerCameraManager.generated.h"

UENUM(BlueprintType)
enum class ECameraView : uint8
{
	CV_None			UMETA(DisplayName = "None"),
	CV_Exploration	UMETA(DisplayName = "Exploration"),
	CV_Action		UMETA(DisplayName = "Action"),
	CV_Aim			UMETA(DisplayName = "Aim"),
	CV_LockOn		UMETA(DisplayName = "Lock-On"),
	CV_Skill		UMETA(DisplayName = "Skill"),

	CV_MAX			UMETA(Hidden),
};

class ARPGProjectPlayerController;
class ARPGProjectPlayerCharacter;
class USpringArmComponent;
class UArrowComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class RPGPROJECT_API ARPGPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:

	ARPGPlayerCameraManager();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void UpdateCamera(float DeltaTime) override;

public:		// --- FUNCTIONS --- \\

	UFUNCTION(BlueprintCallable)
	bool SetCameraView(ECameraView NewCameraView, bool bOverrideLockOn = false);
	UFUNCTION(BlueprintPure)
	ECameraView GetCameraView() const { return CurrentCameraView; } 
	UFUNCTION(BlueprintPure)
	ECameraView GetLastCameraView() const { return LastCameraView; }

	UFUNCTION(BlueprintCallable)
	bool SetCameraSpringArmMap(ECameraView CameraViewIndex, USpringArmComponent* SpringArmComp);
	UFUNCTION(BlueprintCallable)
	USpringArmComponent* GetValueFromCameraSpringArmMap(ECameraView CameraViewIndex);

	UFUNCTION(BlueprintCallable)
	bool SetCameraArrowMap(ECameraView CameraViewIndex, UArrowComponent* ArrowComp);
	UFUNCTION(BlueprintCallable)
	UArrowComponent* GetValueFromCameraArrowMap(ECameraView CameraViewIndex);

	UFUNCTION(BlueprintCallable)
	void SetLockOnTargetActor(AActor* NewTargetActor) { LockOnTargetActor = NewTargetActor; }
	UFUNCTION(BlueprintPure)
	AActor* GetLockOnTargetActor() { return LockOnTargetActor; }

	UFUNCTION(BlueprintPure)
	FRotator GetTargetActorAngle() { return TargetActorAngle; }

	UFUNCTION(BlueprintPure)
	bool GetCanSwapTarget() { return bCanSwapTarget; }
	UFUNCTION(BlueprintCallable)
	void SetCanSwapTarget(bool bIsActive) { bCanSwapTarget = bIsActive; }

	UFUNCTION(BlueprintCallable)
	void GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float MinRecentTime = 0.01f, FRotator MinAllowedViewAngle = FRotator(0.f, 0.f, 0.f), FRotator MaxAllowedViewAngle = FRotator(0.f, 0.f, 0.f));

	UFUNCTION(BlueprintCallable)
	void GetRenderedActorsInViewportCircularSector(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float TargetAngle , float SearchAngleRange, float MinRecentTime = 0.01f);

	void EnableLockOn();
	void DisableLockOn();

	void SwapLockOnTarget(float InYawValue, float InPitchValue);

	UFUNCTION(BlueprintCallable)
	bool GetCrosshairTarget(FHitResult& CrosshairHitResult);

public:		// --- VARIABLES --- \\



protected:	// --- FUNCTIONS --- \\

	void InterpToView();

	UFUNCTION(BlueprintPure)
	float GetViewportAngleFromVector2D(FVector2D InVector2D);

	void ResetSwapTargetCooldown() { bSwapTargetOnCooldown = false; }

protected:	// --- VARIABLES --- \\

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	ARPGProjectPlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera View State")
	ECameraView CurrentCameraView = ECameraView::CV_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera View State")
	ECameraView LastCameraView = CurrentCameraView;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera Views - Spring Arms")
	TMap<ECameraView, USpringArmComponent*> CameraSpringArmMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera Views - Arrows")
	TMap<ECameraView, UArrowComponent*> CameraArrowMap;

	UPROPERTY(EditAnywhere, Category = "Interaction Trace Settings")
	TEnumAsByte<ETraceTypeQuery> ActorInViewTraceCollisionChannel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Interp Speeds")
	TMap<ECameraView, float> InterpSpeedMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lock-On - Allowed Angles")
	FRotator FirstLockAllowedViewAngle {5.f, 5.f, 5.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float SwapCooldown = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	bool bSwapTargetOnCooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float SwapTargetSearchRange = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float SwapTargetSearchAngleRange = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float SwapTargetSecondSearchAngleRange = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float SwapTargetPreciseSearchAngleRange = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock-On - Swap Targets")
	float ClosestTargetPriorityDistance = 150.f;

	UPROPERTY(EditAnywhere, Category = "Crosshair Target Settings")
	bool bShowTrace = false;

	UPROPERTY(EditAnywhere, Category = "Crosshair Target Settings")
	bool bUseSphereTrace = false;

	UPROPERTY(EditAnywhere, Category = "Crosshair Target Settings")
	float TargetTraceRadius = 2.5f;

private:	// --- FUNCTIONS --- \\



private:	// --- VARIABLES --- \\

	UPROPERTY(VisibleAnywhere, Category = "View Target - Camera")
	UCameraComponent* ViewTargetCameraRef;

	UPROPERTY(VisibleAnywhere, Category = "Lock-On Targets")
	TArray<AActor*> LockOnActorArray;
	UPROPERTY(VisibleAnywhere, Category = "Lock-On Targets")
	AActor* LockOnTargetActor = nullptr;
	UPROPERTY(VisibleAnywhere, Category = "Lock-On Targets")
	FName TargetSocket = "";
	UPROPERTY(VisibleAnywhere, Category = "Lock-On Targets")
	FRotator TargetActorAngle = FRotator::ZeroRotator;

	ECameraView CameraViewLastUpdate = ECameraView::CV_None;
	
	UArrowComponent* CurrentCameraArrow;

	FTimerHandle InterpToViewTimerHandle;
	FTimerHandle SwapTargetCooldownTimerHandle;
	FTimerHandle NoLineOfSightOnTargetTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "Lock-On - Swap Targets")
	bool bCanSwapTarget = true;

};
