// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSItemBase.h"


UFPSItemBase::UFPSItemBase() : bIsCopy(false), bIsPickup(false), Quantity(1) {

}

void UFPSItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

UFPSItemBase* UFPSItemBase::CreateItemCopy()
{
	UFPSItemBase* ItemCopy = NewObject< UFPSItemBase>(StaticClass());
	
	ItemCopy->ID = this->ID;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemStatistic = this->ItemStatistic;
	ItemCopy->ItemTextData = this->ItemTextData;
	ItemCopy->ItemAssetData = this->ItemAssetData;
	ItemCopy->ItemNumericData = this->ItemNumericData;
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UFPSItemBase::SetQuantity(const int32 NewQuantity)
{
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, ItemNumericData.bIsStackable ? ItemNumericData.MaxStackSize : 1);

		if (OwningInventory)
		{
			if (Quantity <= 0)
			{
				OwningInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}

void UFPSItemBase::Use(AFPSCharacter* Character) 
{
}