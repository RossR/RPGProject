// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DataAssets/ItemData.h"
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
	int DraggedItemKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
	UItemData* DraggedItemData;

};
