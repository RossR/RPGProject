// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "RPGPlayerCameraManager.generated.h"

UENUM(BlueprintType)
enum class ECameraView : uint8
{
	None			UMETA(DisplayName = "None"),
	Exploration		UMETA(DisplayName = "Exploration"),
	Action			UMETA(DisplayName = "Action"),
	Aim				UMETA(DisplayName = "Aim"),
	LockOn			UMETA(DisplayName = "Lock-On"),
	Skill			UMETA(DisplayName = "Skill"),

	MAX				UMETA(Hidden),
};

class ARPGProjectPlayerController;
class ARPGProjectPlayerCharacter;
class USpringArmComponent;
class UArrowComponent;
class UCameraComponent;
class UCombatComponent;

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

	//------------------
	// Aiming
	//------------------

	/**
	 * Executes a trace from the camera to the centre of the screen, returns true if the trace was blocked
	 * @param CrosshairHitResult Properties of the trace hit
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Aiming")
	bool GetCrosshairTarget(FHitResult& CrosshairHitResult);

	//------------------
	// Camera View
	//------------------

	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Camera View")
	ECameraView GetCameraView() const { return CurrentCameraView; }
	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Camera View")
	ECameraView GetLastCameraView() const { return LastCameraView; }
	/**
	 * Sets the value of CameraView, returns true if the camera view was successfully changed
	 * @param NewCameraView	The new state of CameraView
	 * @param bOverrideLockOn If true, will set the value of CameraView even if the current value is "Lock-On"
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Camera View")
	bool SetCameraView(ECameraView NewCameraView, bool bOverrideLockOn = false);	

	// Camera Arrows
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Camera View|Camera Arrows")
	UArrowComponent* GetArrowFromCameraView(ECameraView CameraViewIndex);
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Camera View|Camera Arrows")
	bool SetArrowForCameraView(ECameraView CameraViewIndex, UArrowComponent* ArrowComp);

	// Spring Arms
	//------------------

	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Camera View|Spring Arms")
	USpringArmComponent* GetSpringArmFromCameraView(ECameraView CameraViewIndex);
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Camera View|Spring Arms")
	bool SetSpringArmForCameraView(ECameraView CameraViewIndex, USpringArmComponent* SpringArmComp);

	//------------------
	// Lock-On
	//------------------


	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On")
	void DisableLockOn();
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On")
	void EnableLockOn();

	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Lock-On")
	AActor* GetLockOnTargetActor() { return LockOnTargetActor; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On")
	void SetLockOnTargetActor(AActor* NewTargetActor) { LockOnTargetActor = NewTargetActor; }
	
	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Lock-On")
	FRotator GetTargetActorAngle() { return TargetActorAngle; }

	// Used by the HUD widget to place the lock-on crosshair over the lock-on target's "LockOn_MainTarget" socket
	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Lock-On")
	FVector GetLocationOfLockOnTargetActorsMainTarget();

	/**
	 * Gets all the actors in the world that are within the view of the viewing actor
	 * @param ViewingActor The actor that is viewing the rendered actors, typically the camera manager
	 * @param CurrentlyRenderedActors The array of actors that are within the view of the viewing actor
	 * @param ActorTag Actors without this tag will be ignored
	 * @param MinRecentTime Actors that were last rendered before this period of time will be ignored
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On")
	void GetRenderedActorsInView(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float MinRecentTime = 0.01f);

	/**
	 * Gets all the actors in the world that are within a sector the viewing actor's view
	 * @param ViewingActor The actor that is viewing the rendered actors, typically the camera manager
	 * @param CurrentlyRenderedActors The array of actors that are within the view of the viewing actor
	 * @param ActorTag Actors without this tag will be ignored
	 * @param TargetAngle The direction of the viewport to search for actors (0° = Up, 90° = Right, 180° = Down, 270° = Left)
	 * @param SearchSectorSize The width of the search sector 
	 * @param MinRecentTime Actors that were last rendered before this period of time will be ignored
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On")
	void GetRenderedActorsInViewportCircularSector(AActor* ViewingActor, TArray<AActor*>& CurrentlyRenderedActors, FName ActorTag, float TargetAngle, float SearchSectorSize, float MinRecentTime = 0.01f);
	
	// Swapping Target
	//------------------
	
	UFUNCTION(BlueprintPure, Category = "RPG Player Camera Manager|Lock-On|Swapping Target")
	bool GetCanSwapTarget() { return bCanSwapTarget; }
	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On|Swapping Target")
	void SetCanSwapTarget(bool bIsActive) { bCanSwapTarget = bIsActive; }

	UFUNCTION(BlueprintCallable, Category = "RPG Player Camera Manager|Lock-On|Swapping Target")
	void SwapLockOnTarget(float InYawValue, float InPitchValue);

public:		// --- VARIABLES --- \\



protected:	// --- FUNCTIONS --- \\

	// If the camera is not in position, interpolate it to the current camera view
	void InterpToView();

	// Returns the dot product of the screen position from the centre of the viewport
	UFUNCTION(BlueprintPure)
	float GetViewportAngleFromScreenPosition(FVector2D ScreenPosition);
	
	//Returns the dot product of the 2D vector from the centre of the viewport.
	UFUNCTION(BlueprintPure)
	float GetViewportAngleFromVector2D(FVector2D InVector2D);

	void ResetSwapTargetCooldown() { bSwapTargetOnCooldown = false; }

	// Checks all potential lock-on targets and sets the actor closest to the centre of the viewport as the lock-on target
	void EvaluateSwapLockOnTargets();

	// Updates the rotation of the camera if the current camera view is "Lock-On" and disables lock-on if certain conditions are met
	void LockOnUpdate();

protected:	// --- VARIABLES --- \\

	//------------------
	// Camera Views
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Camera Views")
	ECameraView CurrentCameraView = ECameraView::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Camera Views")
	ECameraView LastCameraView = CurrentCameraView;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Player Camera Manager|Camera Views|Interp Speeds")
	TMap<ECameraView, float> InterpSpeedMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Camera Views|Arrows")
	TMap<ECameraView, UArrowComponent*> CameraArrowMap;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Camera Views|Spring Arms")
	TMap<ECameraView, USpringArmComponent*> CameraSpringArmMap;

	//------------------
	// Lock-On
	//------------------

	// Trace Settings
	//------------------

	UPROPERTY(EditAnywhere, Category = "RPG Player Camera Manager|Lock-On|Trace Settings")
	bool bShowTrace = false;

	UPROPERTY(EditAnywhere, Category = "RPG Player Camera Manager|Lock-On|Trace Settings")
	TEnumAsByte<ETraceTypeQuery> ActorInViewTraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category = "RPG Player Camera Manager|Lock-On|Trace Settings")
	bool bUseSphereTrace = false;

	UPROPERTY(EditAnywhere, Category = "RPG Player Camera Manager|Lock-On|Trace Settings", meta = (EditCondition = "bUseSphereTrace", DisplayAfter = "bUseSphereTrace", EditConditionHides))
	float TargetTraceRadius = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RPG Player Camera Manager|Lock-On|Trace Settings")
	FRotator FirstLockAllowedViewAngle = { 5.f, 5.f, 5.f };

	// Swap Targets
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	float SwapCooldown = 0.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	bool bSwapTargetOnCooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	float SwapTargetSearchSectorSize = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	float SwapTargetPreciseSearchAngleRange = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	float SwapTargetSecondSearchSectorSize = 90.f;

	//------------------
	// References
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|References")
	ARPGProjectPlayerController* PC;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|References")
	ARPGProjectPlayerCharacter* PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG Player Camera Manager|References")
	UCombatComponent* CombatComponentRef;
	
private:	// --- FUNCTIONS --- \\



private:	// --- VARIABLES --- \\

	ECameraView CameraViewOnLastUpdate = ECameraView::None;

	UArrowComponent* CurrentCameraArrow;

	FTimerHandle InterpToViewTimerHandle;
	FTimerHandle SwapTargetCooldownTimerHandle;
	FTimerHandle NoLineOfSightOnTargetTimerHandle;

	//------------------
	// Lock-On
	//------------------

	// Current Target
	//------------------

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|Lock-On|Current Target")
	AActor* LockOnTargetActor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|Lock-On|Current Target")
	FName TargetSocket = "";

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|Lock-On|Current Target")
	FRotator TargetActorAngle = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|Lock-On|Current Target")
	TArray<AActor*> LockOnActorArray;

	// Swap Targets
	//------------------

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|Lock-On|Swap Targets")
	bool bCanSwapTarget = true;

	//------------------
	// References
	//------------------

	UPROPERTY(VisibleAnywhere, Category = "RPG Player Camera Manager|References")
	UCameraComponent* ViewTargetCameraRef;

};
