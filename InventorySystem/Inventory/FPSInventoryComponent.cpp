// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSInventoryComponent.h"
#include "FPSItemBase.h"

// Sets default values for this component's properties
UFPSInventoryComponent::UFPSInventoryComponent()
{
	// 필요하지 않은 경우 성능 향상을 위해서 끄기
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
	//무게 제한 계산용 함수
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
	//입력 아이템이 이미 인벤토리에 존재하고, 아직 풀스택이 아닌지 검사
	UFPSItemBase* ExistingItemStack = FindNextPartialStack(InputItem);

	// 기존 스택에 아이템을 분배
	while (ExistingItemStack)
	{
        // 풀스택을 만들기 위해 필요한 수량 계산 
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);

		// 남은 아이템이 용량을 초과하지 않는 경우
		if (AmountToMakeFullStack > 0)
		{
			// 기존 스택 수량 증가
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + AmountToMakeFullStack);
			
			// 남은 분배량 갱신
			AmountToDistribute -= AmountToMakeFullStack;

			InputItem->SetQuantity(AmountToDistribute);
		}
		else
		{
			//일부만 추가된 경우
			if (AmountToDistribute != RequestedAddAmount)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute;
			}

			return 0;
		}

		// 분배 완료인 경우
		if (AmountToDistribute <= 0)
		{
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount;
		}

		// 추가로 분배할 수 있는 다른 스택 검사
		ExistingItemStack = FindNextPartialStack(InputItem);
	}

	// 남은 아이템이 있고, 새로운 스택을 추가할 수 있는 경우
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(InputItem, AmountToDistribute);

		if (AmountToMakeFullStack > 0)
		{
			// 남은 아이템이 전부 추가되지 못한 경우
			if (AmountToMakeFullStack < AmountToDistribute)
			{
				AmountToDistribute -= AmountToMakeFullStack;
				InputItem->SetQuantity(AmountToDistribute);

				// 일부만 추가되므로 복제 아이템 생성
				AddNewItem(InputItem->CreateItemCopy(), AmountToDistribute);
				return RequestedAddAmount - AmountToDistribute;
			}

			// 남은 양 전체 추가 가능할 경우
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


