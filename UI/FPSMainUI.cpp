// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSMainUI.h"
#include "FPSQuickSlotWIdget.h"

void UFPSMainUI::NativeOnInitialized()
{
	Slots.Empty();
	Items.SetNum(3);
	CurrentIndex = 0;

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

void UFPSMainUI::UpdateQuickSlotUI()
{
	for (int32 i = 0; i < 3; i++)
	{
		int32 ItemIndex = (CurrentIndex + i) % 3;
		Slots[i]->SetItem(Items[ItemIndex]);
	}
}

void UFPSMainUI::Scroll(int32 Direction)
{
	CurrentIndex = (CurrentIndex + Direction + 3) % 3; //현재 선택되어 있는 item의 index 
	UpdateQuickSlotUI();
}

UFPSItemBase* UFPSMainUI::GetCurrentItem()
{
	return Items[CurrentIndex];
}

void UFPSMainUI::AddItem(int index, UFPSItemBase* inputItem)
{
	if (index >= 0 && index < 3)
	{
		FString mystr = inputItem->ItemTextData.Name.ToString();
		UE_LOG(LogTemp, Warning, TEXT("Index: %d, inputItem: %s"), index, *mystr);
		Items[index] = inputItem;
		UpdateQuickSlotUI();
	}
}

void UFPSMainUI::RemoveItem(int index)
{
	if (index >= 0 && index < 3)
	{
		UE_LOG(LogTemp, Warning, TEXT("Rmove Index: %d"), index);
		Items[index] = nullptr;
		UpdateQuickSlotUI();
	}
}


