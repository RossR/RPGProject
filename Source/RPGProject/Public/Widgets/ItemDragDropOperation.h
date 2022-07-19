// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DataAssets/ItemData.h"
#include "Actors/Components/InventoryComponent.h"
#include "Actors/Components/EquipmentComponent.h"
#include "ItemDragDropOperation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class RPGPROJECT_API UItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	int DraggedItemKey = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	EEquipmentSlot DraggedEquipmentSlot = EEquipmentSlot::EES_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	UItemData* DraggedItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	UInventoryComponent* DraggedInventoryComponentRef = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	UEquipmentComponent* DraggedEquipmentComponentRef = nullptr;

};
