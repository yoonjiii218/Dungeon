// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSInventoryPanel.h"
#include "FPSItemDragDropOperation.h"


void UFPSInventoryPanel::NativeConstruct()
{
	Slots.Empty();
	if (QuickSlot1)
	{
		Slots.Add(QuickSlot1);
		QuickSlot1->SetIndex(0);
	}
	if (QuickSlot2)
	{
		Slots.Add(QuickSlot2);
		QuickSlot2->SetIndex(1);
	}
	if (QuickSlot3)
	{
		Slots.Add(QuickSlot3);
		QuickSlot3->SetIndex(2);
	}
}

void UFPSInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			//구독 설정
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UFPSInventoryPanel::RefreshInventory);
			SetInfoText();
		}
	}
}

void UFPSInventoryPanel::SetInfoText() const
{
	/*
	* 	WeightInfo->SetText(FText::Format(FText::FromString("{0} / {1}"),
		InventoryReference->GetInventoryTotalWeight(),
		InventoryReference->GetWeightCapacity()));
	*/

	const FString CapacityInfoValue{
		FString::FromInt(InventoryReference->GetInventoryContents().Num()) + "/"
		+ FString::FromInt(InventoryReference->GetSlotCapacity())
	};

	CapacityInfo->SetText(FText::FromString(CapacityInfoValue));
}


void UFPSInventoryPanel::RefreshInventory()
{
	if (InventoryReference && InventorySlotClass)
	{
		InventoryPanel->ClearChildren();
		for (UFPSItemBase* const& InventoryItem : InventoryReference->GetInventoryContents())
		{
			UFPSInventoryItemSlot* ItemSlot = CreateWidget<UFPSInventoryItemSlot>(this, InventorySlotClass);
			ItemSlot->SetItemReference(InventoryItem);

			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}

		SetInfoText();
	}
}

bool UFPSInventoryPanel::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UFPSItemDragDropOperation* ItemDragDrop = Cast<UFPSItemDragDropOperation>(InOperation);

	if (ItemDragDrop->SourceItem && InventoryReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected an item drop on InvnetoryPanel"));

		// true를 반환하면 드롭 작업 중단
		return true;
	}

	return false;
}
