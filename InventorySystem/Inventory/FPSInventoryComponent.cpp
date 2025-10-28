// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSInventoryComponent.h"
#include "FPSItemBase.h"

// Sets default values for this component's properties
UFPSInventoryComponent::UFPSInventoryComponent()
{
	// �ʿ����� ���� ��� ���� ����� ���ؼ� ����
	PrimaryComponentTick.bCanEverTick = true;

}

UFPSItemBase* UFPSInventoryComponent::FindMatchingItem(UFPSItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (InventoryContents.Contains(ItemIn))
		{
			return ItemIn;
		}
	}
	return nullptr;
}

UFPSItemBase* UFPSInventoryComponent::FindNextItemByID(UFPSItemBase* ItemIn) const
{
	if (ItemIn)
	{
		if (const TArray<TObjectPtr<UFPSItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemIn))
		{
			return *Result;
		}
	}
	return nullptr;
}

UFPSItemBase* UFPSInventoryComponent::FindNextPartialStack(UFPSItemBase* ItemIn) const
{
	if (const TArray<TObjectPtr<UFPSItemBase>>::ElementType* Result =
		InventoryContents.FindByPredicate([&ItemIn](const UFPSItemBase* InventoryItem)
			{
				return InventoryItem->ID == ItemIn->ID && !InventoryItem->IsFullItemStack();
			}
		))
	{
		return *Result;
	}
	return nullptr;
}

int32 UFPSInventoryComponent::CalculateWeightAddAmount(UFPSItemBase*, int32 RequestedAddAmount)
{
	//���� ���� ���� �Լ�
	return int32();
}

int32 UFPSInventoryComponent::CalculateNumberForFullStack(UFPSItemBase* StackableItem, int32 InitialRequestAddAmount)
{
	const int32 AddAmountToMakeStack = StackableItem->ItemNumericData.MaxStackSize - StackableItem->Quantity;
	return FMath::Min(InitialRequestAddAmount, AddAmountToMakeStack);
}

void UFPSInventoryComponent::RemoveSingleInstanceOfItem(UFPSItemBase* ItemIn)
{
	InventoryContents.RemoveSingle(ItemIn);
	OnInventoryUpdated.Broadcast();
}

int32 UFPSInventoryComponent::RemoveAmountOfItem(UFPSItemBase* ItemIn, int32 DesiredAmountToRemove)
{
	const int32 ActualAmounToRemove = FMath::Min(DesiredAmountToRemove, ItemIn->Quantity);
	ItemIn->SetQuantity(ItemIn->Quantity - ActualAmounToRemove);
	OnInventoryUpdated.Broadcast();
	return ActualAmounToRemove;
}

void UFPSInventoryComponent::SplitExistingStack(UFPSItemBase* ItemIn, const int32 AmountToSplit)
{
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		RemoveAmountOfItem(ItemIn, AmountToSplit);
		AddNewItem(ItemIn, AmountToSplit);
	}
}

FItemAddResult UFPSInventoryComponent::HandleNonStackableItems(UFPSItemBase* InputItem, int32 RequestedAddAmount)
{
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity) {
		return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add {0} to the Inventory. All inventory slots are full."), InputItem->ItemTextData.Name
		));
	}

	AddNewItem(InputItem, 1);
	return FItemAddResult::AddAll(1, FText::Format(
		FText::FromString("Successfully added {0} to the inventory."), InputItem->ItemTextData.Name
	));
}

int32 UFPSInventoryComponent::HandleStackableItems(UFPSItemBase* InputItem, int32 RequestedAddAmount)
{
	if (RequestedAddAmount <= 0)
	{
		return 0;
	}

	int32 AmountToDistribute = RequestedAddAmount;
	//�Է� �������� �̹� �κ��丮�� �����ϰ�, ���� Ǯ������ �ƴ��� �˻�
	UFPSItemBase* ExistingItemStack = FindNextPartialStack(InputItem);

	// ���� ���ÿ� �������� �й�
	while (ExistingItemStack)
	{
        // Ǯ������ ����� ���� �ʿ��� ���� ��� 
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);

		// ���� �������� �뷮�� �ʰ����� �ʴ� ���
		if (AmountToMakeFullStack > 0)
		{
			// ���� ���� ���� ����
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + AmountToMakeFullStack);
			
			// ���� �й跮 ����
			AmountToDistribute -= AmountToMakeFullStack;

			InputItem->SetQuantity(AmountToDistribute);
		}
		else
		{
			//�Ϻθ� �߰��� ���
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			return 0;
		}

		// �й� �Ϸ��� ���
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// �߰��� �й��� �� �ִ� �ٸ� ���� �˻�
		ExistingItemStack = FindNextPartialStack(InputItem);
	}

	// ���� �������� �ְ�, ���ο� ������ �߰��� �� �ִ� ���
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(InputItem, AmountToDistribute);

		if (AmountToMakeFullStack > 0)
		{
			// ���� �������� ���� �߰����� ���� ���
			if (AmountToMakeFullStack < AmountToDistribute)
			{
				AmountToDistribute -= AmountToMakeFullStack;
				InputItem->SetQuantity(AmountToDistribute);

				// �Ϻθ� �߰��ǹǷ� ���� ������ ����
				AddNewItem(InputItem->CreateItemCopy(), AmountToDistribute);
				return RequestedAddAmount - AmountToDistribute;
			}

			// ���� �� ��ü �߰� ������ ���
			AddNewItem(InputItem, AmountToDistribute);
			return RequestedAddAmount;
		}
	}

	OnInventoryUpdated.Broadcast();
	return RequestedAddAmount - AmountToDistribute;
}

FItemAddResult UFPSInventoryComponent::HandleAddItem(UFPSItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;
		
		//handle non-stackable item
		if (!InputItem->ItemNumericData.bIsStackable)
		{
			return HandleNonStackableItems(InputItem, InitialRequestedAddAmount);
		}

		//handle stackable
		const int32 StackableAmountAdded = HandleStackableItems(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Successfully added {0} {1} to the inventory."),
				InitialRequestedAddAmount,
				InputItem->ItemTextData.Name));
		}

		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddPartial(StackableAmountAdded, FText::Format(
				FText::FromString("Partial amount of {0} added to the inventory. Number added = {1}"),
				InputItem->ItemTextData.Name, 
				StackableAmountAdded));
		}

		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
				FText::FromString("Couldn't add {0} to the inventory. No remaining inventory slots, or invalid item."),
				InputItem->ItemTextData.Name));
		}
	}

	return FItemAddResult::AddedNone(FText::Format(
		FText::FromString("Could not add {0} to the Inventory. All inventory slots are full."), InputItem->ItemTextData.Name
	));
}

void UFPSInventoryComponent::AddNewItem(UFPSItemBase* Item, const int32 AmountToAdd)
{
	UFPSItemBase* NewItem;

	if (Item->bIsCopy || Item->bIsPickup)
	{
		NewItem = Item;
		NewItem->ResetItemFlags();
	}
	else 
	{
		NewItem = Item->CreateItemCopy();
	}

	NewItem->OwningInventory = this;
	NewItem->SetQuantity(AmountToAdd);

	InventoryContents.Add(NewItem);
	OnInventoryUpdated.Broadcast();
}


