// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (DontUseGenericSpawnObject = "True"))
class RPGPROJECT_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drag and Drop", meta = (ExposeOnSpawn = "true"))
		int DraggedItemKey;
	
};
