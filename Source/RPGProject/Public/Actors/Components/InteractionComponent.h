// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class AActor;
class UCapsuleComponent;
class UPrimitiveComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionSuccess);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	// This is broadcasted from children, they know when an interaction has successfully finished
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionSuccess InteractionSuccess;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UCapsuleComponent* GetTriggerCapsule() const { return TriggerCapsule; }

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void InteractionStart();

	UFUNCTION(BlueprintCallable)
	virtual void InteractionCancel();

protected:

	UPROPERTY(EditAnywhere)
	FText InteractionPrompt;

	UPROPERTY(EditAnywhere, NoClear, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* TriggerCapsule = nullptr;

	FVector PromptOffset;
	
	UPROPERTY(VisibleAnywhere)
	AActor* InteractingActor;

};
