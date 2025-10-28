// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSQuickSlotWidget.h"
#include "FPSItemDragDropOperation.h"
#include "FPSMainUI.h"
#include "../FPSHUD.h"
#include "../FPSCharacter.h"


void UFPSQuickSlotWidget::NativeOnInitialized()
{
    HUD = Cast<AFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
    if (HUD)
    {
        HUD->OnMainUIInitialized.AddUObject(this, &UFPSQuickSlotWidget::InitializeQuickSlotManager);
    }

    //UE_LOG(LogTemp, Warning, TEXT("QuickSlotWidget QuickSlotManager Initialize"));
}

void UFPSQuickSlotWidget::InitializeQuickSlotManager()
{
    QuickSlotManager = HUD->MainUIWidget; 
}

void UFPSQuickSlotWidget::SetItem(UFPSItemBase* Item)
{
    if (!Item)
    {
        ItemIcon->SetBrush(FSlateBrush());
        return; 
    }

    OwnItem = Item;
	ItemIcon->SetBrushFromTexture(Item->ItemAssetData.Icon);
}


bool UFPSQuickSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UFPSItemDragDropOperation* ItemDragDrop = Cast< UFPSItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem)
	{
		SetItem(ItemDragDrop->SourceItem);
        QuickSlotManager->AddItem(index, ItemDragDrop->SourceItem);

        AActor* player = GetWorld()->GetFirstPlayerController()->GetPawn();
        if (AFPSCharacter* Player = Cast<AFPSCharacter>(player))
        {
            Player->GetInventory()->RemoveSingleInstanceOfItem(ItemDragDrop->SourceItem);
        }

		return true;
	}

	return false;
}

FReply UFPSQuickSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        AActor* player = GetWorld()->GetFirstPlayerController()->GetPawn();
        if (AFPSCharacter* Player = Cast<AFPSCharacter>(player))
        {
            Player->GetInventory()->HandleAddItem(OwnItem);
            SetItem(nullptr);
            QuickSlotManager->RemoveItem(index);
        }
    }

    return Reply.Unhandled();
}
