// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSInventoryTooltip.h"
#include "FPSItemBase.h"

void UFPSInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	const UFPSItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	ItemName->SetText(ItemBeingHovered->ItemTextData.Name);
	ItemDescription->SetText(ItemBeingHovered->ItemTextData.Description);

	if (ItemBeingHovered->ItemNumericData.bIsStackable)
	{
		const FString StackInfo =
		{ "Max stack size: " + FString::FromInt(ItemBeingHovered->ItemNumericData.MaxStackSize) };
		
		MaxStackSize->SetText(FText::FromString(StackInfo));
	}
	else
	{
		MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
	}
}
