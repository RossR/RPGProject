// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/SlateBrush.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/NamedSlot.h"
#include "Components/Overlay.h"
#include "Components/SizeBox.h"
#include "Components/ScaleBox.h"
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

	// Called every frame
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(BlueprintCallable, Category = "Equipment Slot")
	void SetEquipmentComponentRef(UEquipmentComponent* InEquipmentComponentRef) { EquipmentComponentRef = InEquipmentComponentRef; }

protected:

	float PaddingValue = 2.0f;
	float ClampSlotWidth;
	float ClampSlotHeight;

	//------------------
	// Equipment Slot
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
	EEquipmentSlot SlotEquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
	float SlotWidth = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
	float SlotHeight = 50.f;

	// References
	//------------------

	UPROPERTY(BlueprintReadWrite, Category = "Equipment Slot|References")
	APlayerController* PC;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment Slot|References")
	ACharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadWrite, Category = "Equipment Slot|References")
	UEquipmentComponent* EquipmentComponentRef;

	// Slate Brushes
	//------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot|Slate Brushes")
	FSlateBrush RootBorderSlateBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot|Slate Brushes")
	FSlateBrush BackgroundSlateBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot|Slate Brushes")
	FSlateBrush BackgroundOutlineSlateBrush;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot|Slate Brushes")
	FSlateBrush BackgroundIconSlateBrush;
	
	// Widgets
	//------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UBorder* RootBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UBorder* BackgroundBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UOverlay* BackgroundOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UOverlay* BackgroundImageOverlay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UScaleBox* BackgroundOutlineScaleBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UScaleBox* BackgroundIconScaleBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UImage* BackgroundImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UImage* BackgroundOutlineImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UImage* BackgroundIconImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UBorder* ContentBorder;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	USizeBox* ContentSizeBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Instanced, Category = "Equipment Slot|Widgets")
	UNamedSlot* EquipmentSlot;
	
};
