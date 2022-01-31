// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EItemCategory : uint8
{
	EIC_Default UMETA(DisplayName="Default"),
	EIC_Consumable UMETA(DisplayName="Consumable"),
	EIC_Weapon UMETA(DisplayName="Weapon"),

	EIC_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Default UMETA(DisplayName="Default"),
	EIR_Common UMETA(DisplayName="Common"),
	EIR_Uncommon UMETA(DisplayName="Uncommon"),
	EIR_Rare UMETA(DisplayName="Rare"),
	EIR_Mythic UMETA(DisplayName="Mythic"),
	EIR_Legendary UMETA(DisplayName="Legendary"),

	EIR_Max UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FItemInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemCategory ItemCategory;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	EItemRarity ItemRarity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	int ItemQualityRating;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	UTexture2D* ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Info")
	TSubclassOf<AActor> ClassToSpawn;

};


UCLASS()
class RPGPROJECT_API AItemBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	FItemInfo GetItemInfo() { return ItemInfo; }

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	FItemInfo ItemInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	UStaticMeshComponent* ItemMesh;
	

};
