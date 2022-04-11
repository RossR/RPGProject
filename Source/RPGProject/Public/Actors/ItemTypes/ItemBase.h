// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DataAssets/ItemData.h"
#include "Interfaces/HighlightInterface.h"
#include "ItemBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class RPGPROJECT_API AItemBase : public AActor, public IHighlightInterface
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
	virtual UItemData* GetItemData() { return ItemData; }

	UFUNCTION(BlueprintCallable)
	virtual void SetItemData(UItemData* NewItemData) { ItemData = NewItemData; }

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetItemMesh() { return ItemMesh; }

	virtual void EnableHighlight(bool bActive, int Colour = -1) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Info")
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	UItemData* ItemDataDefault;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, Category = "Item Data")
	UItemData* ItemData;

};
