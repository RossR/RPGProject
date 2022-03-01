// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/SlateBrush.h"
#include "Components/Border.h"
#include "Components/SizeBox.h"
#include "Components/NamedSlot.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Actors/Components/EquipmentComponent.h"
#include "Actors/Components/InventoryComponent.h"
#include "EquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	bool Initialize() override;

	virtual void NativeConstruct() override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	

protected:

	UPROPERTY(BlueprintReadWrite, Category = "Blueprint References")
	APlayerController* PC;

	UPROPERTY(BlueprintReadWrite, Category = "Blueprint References")
	ACharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Component References")
	UInventoryComponent* CharacterInventoryComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Component References")
	UEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment Slot Widgets")
	UBorder* RootBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment Slot Widgets")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment Slot Widgets")
	UBorder* ContentBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment Slot Widgets")
	USizeBox* ContentSizeBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Equipment Slot Widgets")
	UNamedSlot* EquipmentSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot Settings")
	FSlateBrush RootBorderAppearance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot Settings")
	float SlotWidth = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot Settings")
	float SlotHeight = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot Settings")
	EEquipmentSlot WidgetEquipmentSlot;

	float PaddingValue = 2.0f;
	float ClampSlotWidth;
	float ClampSlotHeight;
};
