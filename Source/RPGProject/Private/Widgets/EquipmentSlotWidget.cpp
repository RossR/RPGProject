// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EquipmentSlotWidget.h"
#include "Widgets/ItemDragDropOperation.h"

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

	ContentBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("Content_Border"));
	BackgroundBorder->AddChild(ContentBorder);

	ContentSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("Content_SizeBox"));
	ContentBorder->AddChild(ContentSizeBox);

	EquipmentSlot = WidgetTree->ConstructWidget<UNamedSlot>(UNamedSlot::StaticClass(), TEXT("Equipment_Slot"));
	ContentSizeBox->AddChild(EquipmentSlot);

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

	return b;
}

void UEquipmentSlotWidget::NativeConstruct()
{
	// RootBorder->SetBrush(RootBorderAppearance);

	PC = GetOwningPlayer();
	PlayerCharacter = PC->GetCharacter();

	CharacterInventoryComponent = Cast<UInventoryComponent>(PlayerCharacter->GetComponentByClass(UInventoryComponent::StaticClass()));
	CharacterEquipmentComponent = Cast<UEquipmentComponent>(PlayerCharacter->GetComponentByClass(UEquipmentComponent::StaticClass()));

}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("EquipmentSlotWidget::NativeOnDrop called."));

	const UItemDragDropOperation* ItemDragDrop = Cast<UItemDragDropOperation>(InOperation);

	if (!ItemDragDrop)
	{
		UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlotWidget::NativeOnDrop Cast to UItemDragDropOperation failed."));
		return false;
	}
	
	UItemEquipmentData* PayloadEquipmentData = Cast<UItemEquipmentData>(ItemDragDrop->DraggedItemData);

	if (PayloadEquipmentData)
	{
		if (WidgetEquipmentSlot != CharacterEquipmentComponent->GetEquipmentSlotForItem(PayloadEquipmentData))
		{
			return false;
		}
	}

	// If slot is empty, equip the item
	if (!CharacterEquipmentComponent->GetWornEquipmentData().Contains(WidgetEquipmentSlot))
	{
		if (CharacterInventoryComponent->GetInventoryItemDataMap().Contains(ItemDragDrop->DraggedItemKey))
		{
			bool bSuccessfullyEquipped = CharacterEquipmentComponent->Equip(CharacterInventoryComponent->GetInventoryItemData(ItemDragDrop->DraggedItemKey));

			if (bSuccessfullyEquipped)
			{
				CharacterInventoryComponent->RemoveItemFromInventory(ItemDragDrop->DraggedItemKey);
				return true;
			}
		}
	}

	// if equipping is successful, delete item from inventory

	return false;
}