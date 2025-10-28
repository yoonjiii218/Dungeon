// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSInventoryItemSlot.h"
#include "FPSInventoryTooltip.h"
#include "FPSItemBase.h"
#include "FPSDragItemVisual.h"
#include "FPSItemDragDropOperation.h"

void UFPSInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (ToolTipClass)
	{
		UFPSInventoryTooltip* ToolTip = CreateWidget<UFPSInventoryTooltip>(this, ToolTipClass);
		ToolTip->InventorySlotBeingHovered = this;
		SetToolTip(ToolTip);
	}
	
}

void UFPSInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (ItemReference)
	{
		ItemBorder->SetBrushColor(FLinearColor::Yellow);
		if (ItemReference->ItemNumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
}


 FReply UFPSInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	 FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	 if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	 {
		 return Reply.Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	 }

	 return Reply.Unhandled();
}

void UFPSInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

bool UFPSInventoryItemSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	return false;
}

void UFPSInventoryItemSlot::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragItemVisualClass)
	{
		const TObjectPtr<UFPSDragItemVisual> DragVisual = CreateWidget<UFPSDragItemVisual>(this, DragItemVisualClass);
		DragVisual->ItemIcon->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);
		DragVisual->ItemBorder->SetBrushColor(ItemBorder->GetBrushColor());

		ItemReference->ItemNumericData.bIsStackable
			? DragVisual->ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity))
			: DragVisual->ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);

		UFPSItemDragDropOperation* DragItemOperation = NewObject<UFPSItemDragDropOperation>();
		DragItemOperation->SourceItem = ItemReference;
		DragItemOperation->SourceInventory = ItemReference->OwningInventory;

		DragItemOperation->DefaultDragVisual = DragVisual;
		DragItemOperation->Pivot = EDragPivot::TopLeft;

		OutOperation = DragItemOperation;
	}
}
