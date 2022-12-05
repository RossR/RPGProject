// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EquipmentSlotWidget.h"
#include "Widgets/ItemDragDropOperation.h"
#include "Framework/Application/SlateApplication.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/BorderSlot.h"
#include "Components/OverlaySlot.h"
#include "Components/ScaleBoxSlot.h"
#include "Components/SizeBoxSlot.h"

bool UEquipmentSlotWidget::Initialize()
{
	bool b = Super::Initialize();

	if (WidgetTree)
	{
		RootBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Root_Border"));

		WidgetTree->RootWidget = RootBorder;

	}

	BackgroundBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Background_Border"));
	ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("Content_SizeBox"));
	BackgroundOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Background_Overlay"));
	BackgroundOutlineScaleBox = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("BackgroundImage_ScaleBox"));
	ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Content_Border"));
	BackgroundImageOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("BackgroundImage_Overlay"));
	BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Background_Image"));
	BackgroundOutlineImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("BackgroundOutline_Image"));
	BackgroundIconScaleBox = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("BackgroundIcon_ScaleBox"));
	BackgroundIconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("BackgroundIcon_Image"));
	EquipmentSlot = WidgetTree->ConstructWidget<UNamedSlot>(UNamedSlot::StaticClass(), TEXT("Equipment_Slot"));

	RootBorder->AddChild(BackgroundBorder);
	BackgroundBorder->AddChild(ContentSizeBox);
	ContentSizeBox->AddChild(BackgroundOverlay);

	BackgroundOverlay->AddChild(BackgroundOutlineScaleBox);
	BackgroundOverlay->AddChild(ContentBorder);

	BackgroundOutlineScaleBox->AddChild(BackgroundImageOverlay);
	BackgroundImageOverlay->AddChild(BackgroundImage);
	BackgroundImageOverlay->AddChild(BackgroundOutlineImage);
	BackgroundImageOverlay->AddChild(BackgroundIconScaleBox);
	BackgroundIconScaleBox->AddChild(BackgroundIconImage);

	ContentBorder->AddChild(EquipmentSlot);

	RootBorder->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RootBorder->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	RootBorder->SetPadding(0.f);
	RootBorder->SetBrushColor(FLinearColor{ 0.0f, 0.0f, 0.0f, 1.f });
	RootBorder->SetVisibility(ESlateVisibility::Visible);

	BackgroundBorder->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	BackgroundBorder->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	BackgroundBorder->SetPadding(PaddingValue);
	BackgroundBorder->SetBrushColor(FLinearColor{ 0.0f, 0.0f, 0.0f, 0.f });
	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);

	if (USizeBoxSlot* BackgroundOverlayAsSizeBoxSlot = UWidgetLayoutLibrary::SlotAsSizeBoxSlot(BackgroundOverlay))
	{
		BackgroundOverlayAsSizeBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundOverlayAsSizeBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundOverlay->SetVisibility(ESlateVisibility::Visible);
	
	if (UOverlaySlot* BackgroundImageScaleBoxAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(BackgroundOutlineScaleBox))
	{
		BackgroundImageScaleBoxAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundImageScaleBoxAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundOutlineScaleBox->SetStretch(EStretch::ScaleToFit);
	BackgroundOutlineScaleBox->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UScaleBoxSlot* BackgroundImageOverlayAsScaleBoxSlot = UWidgetLayoutLibrary::SlotAsScaleBoxSlot(BackgroundImageOverlay))
	{
		BackgroundImageOverlayAsScaleBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundImageOverlayAsScaleBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundOverlay->SetVisibility(ESlateVisibility::Visible);

	if (UOverlaySlot* BackgroundOutlineImageAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(BackgroundImage))
	{
		BackgroundOutlineImageAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundOutlineImageAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundImage->SetBrush(BackgroundSlateBrush);
	BackgroundImage->SetColorAndOpacity(FLinearColor{ 0.f, 0.f, 0.f, 1.f });
	BackgroundImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	BackgroundImage->SetOpacity(1.f);

	if (UOverlaySlot* BackgroundOutlineImageAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(BackgroundOutlineImage))
	{
		BackgroundOutlineImageAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundOutlineImageAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundOutlineImage->SetBrush(BackgroundOutlineSlateBrush);
	BackgroundOutlineImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	BackgroundOutlineImage->SetOpacity(1.f);

	if (UOverlaySlot* BackgroundIconScaleBoxAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(BackgroundIconScaleBox))
	{
		BackgroundIconScaleBoxAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundIconScaleBoxAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundIconScaleBox->SetStretch(EStretch::ScaleToFit);
	BackgroundIconScaleBox->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UScaleBoxSlot* BackgroundIconImageAsScaleBoxSlot = UWidgetLayoutLibrary::SlotAsScaleBoxSlot(BackgroundIconImage))
	{
		BackgroundIconImageAsScaleBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		BackgroundIconImageAsScaleBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
	BackgroundIconImage->SetBrush(BackgroundIconSlateBrush);
	BackgroundIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	BackgroundIconImage->SetOpacity(.25f);

	ContentBorder->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	ContentBorder->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	ContentBorder->SetPadding(0.f);
	ContentBorder->SetBrushColor(FLinearColor{ 0.0f, 0.0f, 0.0f, 0.f });
	ContentBorder->SetVisibility(ESlateVisibility::Visible);

	ClampSlotWidth = SlotWidth - (PaddingValue * 2.f);
	ClampSlotWidth = FMath::Clamp(ClampSlotWidth, 0.f, 9999.f);

	ContentSizeBox->SetMinDesiredWidth(ClampSlotWidth);
	ContentSizeBox->SetMaxDesiredWidth(ClampSlotWidth);
	
	ClampSlotHeight = SlotHeight - (PaddingValue * 2.f);
	ClampSlotHeight = FMath::Clamp(ClampSlotHeight, 0.f, 9999.f);

	ContentSizeBox->SetMinDesiredHeight(ClampSlotHeight);
	ContentSizeBox->SetMaxDesiredHeight(ClampSlotHeight);
	if (UBorderSlot* ContentSizeBoxAsBorderSlot = UWidgetLayoutLibrary::SlotAsBorderSlot(ContentSizeBox))
	{
		ContentSizeBoxAsBorderSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		ContentSizeBoxAsBorderSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
		ContentSizeBoxAsBorderSlot->SetPadding(0.f);
	}

	if (UOverlaySlot* ContentBorderAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(ContentBorder))
	{
		ContentBorderAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		ContentBorderAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	if (UBorderSlot* EquipmentSlotAsBorderSlot = UWidgetLayoutLibrary::SlotAsBorderSlot(EquipmentSlot))
	{
		EquipmentSlotAsBorderSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		EquipmentSlotAsBorderSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}

	return b;
}

void UEquipmentSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// RootBorder->SetBrush(RootBorderSlateBrush);

	PC = GetOwningPlayer();
	PlayerCharacter = PC->GetCharacter();

	//InventoryComponentRef = Cast<UInventoryComponent>(PlayerCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
	//EquipmentComponentRef = Cast<UEquipmentComponent>(PlayerCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));

}

void UEquipmentSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (EquipmentSlot->HasAnyChildren())
	{
		BackgroundIconImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		BackgroundIconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	if (!UWidgetBlueprintLibrary::GetDragDroppingContent())
	{
		BackgroundImage->SetColorAndOpacity(FLinearColor{ 0.f, 0.f, 0.f, 1.f });
	}
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("EquipmentSlotWidget::NativeOnDrop called."));

	//BackgroundImage->SetColorAndOpacity(FLinearColor{ 0.f, 0.f, 0.f, 1.f });

	if (!EquipmentComponentRef) 
	{

		return false; 
	}

	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (!ItemDragDrop)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlotWidget::NativeOnDrop Cast to UItemDragDropOperation failed."));
		return false;
	}
	
	UItemEquipmentData* PayloadEquipmentData = Cast<UItemEquipmentData>(ItemDragDrop->DraggedItemData);

	if (!PayloadEquipmentData) { return false; }
	/*if (PayloadEquipmentData)
	{
		if (WidgetEquipmentSlot != EquipmentComponentRef->GetEquipmentSlotForItem(PayloadEquipmentData))
		{
			return false;
		}
	}*/

	// If slot is empty, equip the item
	if (!EquipmentComponentRef->GetWornEquipmentDataMap().Contains(WidgetEquipmentSlot))
	{
		// Dragged item is from an inventory
		if (ItemDragDrop->DraggedItemKey != -1)
		{
			if (ItemDragDrop->DraggedInventoryComponentRef->GetInventoryItemDataMap().Contains(ItemDragDrop->DraggedItemKey))
			{
				return EquipmentComponentRef->Equip(ItemDragDrop->DraggedInventoryComponentRef->GetInventoryItemData(ItemDragDrop->DraggedItemKey), WidgetEquipmentSlot, ItemDragDrop->DraggedInventoryComponentRef, ItemDragDrop->DraggedItemKey, true);
			}
		}
		// Dragged item is from an equipment component
		else if (WidgetEquipmentSlot == ItemDragDrop->DraggedEquipmentSlot)
		{
			if (ItemDragDrop->DraggedEquipmentComponentRef->GetWornEquipmentDataMap().Contains(ItemDragDrop->DraggedEquipmentSlot))
			{
				bool bSuccessfullyEquipped = EquipmentComponentRef->Equip(ItemDragDrop->DraggedItemData, ItemDragDrop->DraggedEquipmentSlot, nullptr, -1, false);

				if (bSuccessfullyEquipped)
				{
					return ItemDragDrop->DraggedEquipmentComponentRef->RemoveEquipmentInSlot(ItemDragDrop->DraggedEquipmentSlot);
				}
			}
		}	
	}



	return false;
}


