// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EquipmentSlotWidget.h"
#include "Widgets/ItemDragDropOperation.h"
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
	RootBorder->AddChild(BackgroundBorder);

	ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("Content_SizeBox"));
	BackgroundBorder->AddChild(ContentSizeBox);

	BackgroundOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("Background_Overlay"));
	ContentSizeBox->AddChild(BackgroundOverlay);

	BackgroundImageScaleBox = WidgetTree->ConstructWidget<UScaleBox>(UScaleBox::StaticClass(), TEXT("BackgroundImage_ScaleBox"));
	BackgroundOverlay->AddChild(BackgroundImageScaleBox);

	ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Content_Border"));
	BackgroundOverlay->AddChild(ContentBorder);

	BackgroundImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Background_Image"));
	BackgroundImageScaleBox->AddChild(BackgroundImage);

	EquipmentSlot = WidgetTree->ConstructWidget<UNamedSlot>(UNamedSlot::StaticClass(), TEXT("Equipment_Slot"));
	ContentBorder->AddChild(EquipmentSlot);

	RootBorder->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	RootBorder->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	RootBorder->SetPadding(0.f);
	RootBorder->SetBrushColor(FLinearColor{ 0.0f, 0.0f, 0.0f, 1.f });
	RootBorder->SetVisibility(ESlateVisibility::Visible);

	BackgroundBorder->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	BackgroundBorder->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	BackgroundBorder->SetPadding(PaddingValue);
	BackgroundBorder->SetBrush(RootBorderAppearance);
	BackgroundBorder->SetVisibility(ESlateVisibility::Visible);

	if (UBorderSlot* BackgroundOverlayAsBorderSlot = UWidgetLayoutLibrary::SlotAsBorderSlot(BackgroundOverlay))
	{
		BackgroundOverlayAsBorderSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundOverlayAsBorderSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundOverlay->SetVisibility(ESlateVisibility::Visible);
	
	if (UOverlaySlot* BackgroundImageScaleBoxAsOverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(BackgroundImageScaleBox))
	{
		BackgroundImageScaleBoxAsOverlaySlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
		BackgroundImageScaleBoxAsOverlaySlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);
	}
	BackgroundImageScaleBox->SetStretch(EStretch::ScaleToFit);
	BackgroundImageScaleBox->SetVisibility(ESlateVisibility::HitTestInvisible);

	BackgroundImageTexture ? BackgroundImage->SetBrushFromTexture(BackgroundImageTexture, true): false;
	BackgroundImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	BackgroundImage->SetOpacity(.25f);

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
	// RootBorder->SetBrush(RootBorderAppearance);

	PC = GetOwningPlayer();
	PlayerCharacter = PC->GetCharacter();

	//InventoryComponentRef = Cast<UInventoryComponent>(PlayerCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
	//EquipmentComponentRef = Cast<UEquipmentComponent>(PlayerCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));

}

void UEquipmentSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	if (EquipmentSlot->HasAnyChildren())
	{
		BackgroundImage->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		BackgroundImage->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("EquipmentSlotWidget::NativeOnDrop called."));

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

	if (PayloadEquipmentData)
	{
		if (WidgetEquipmentSlot != EquipmentComponentRef->GetEquipmentSlotForItem(PayloadEquipmentData))
		{
			return false;
		}
	}

	// If slot is empty, equip the item
	if (!EquipmentComponentRef->GetWornEquipmentDataMap().Contains(WidgetEquipmentSlot))
	{
		// Dragged item is from an inventory
		if (ItemDragDrop->DraggedItemKey != -1)
		{
			if (ItemDragDrop->DraggedInventoryComponentRef->GetInventoryItemDataMap().Contains(ItemDragDrop->DraggedItemKey))
			{
				return EquipmentComponentRef->Equip(ItemDragDrop->DraggedInventoryComponentRef->GetInventoryItemData(ItemDragDrop->DraggedItemKey), EEquipmentSlot::EES_None, ItemDragDrop->DraggedInventoryComponentRef, ItemDragDrop->DraggedItemKey, true);
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


