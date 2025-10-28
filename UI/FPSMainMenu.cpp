// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSMainMenu.h"
#include "../FPSCharacter.h"
#include "FPSItemDragDropOperation.h"
#include "FPSItemBase.h"

void UFPSMainMenu::NativeOnInitialized()
{
}

void UFPSMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AFPSCharacter>(GetOwningPlayerPawn());
}

bool UFPSMainMenu::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UFPSItemDragDropOperation* ItemDragDrop = Cast< UFPSItemDragDropOperation>(InOperation);

	if (PlayerCharacter && ItemDragDrop->SourceItem)
	{
		PlayerCharacter->DropItem(ItemDragDrop->SourceItem, ItemDragDrop->SourceItem->Quantity);
		return true;
	}
	return false;
}
