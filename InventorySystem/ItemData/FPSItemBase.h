// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "../FPSDataTable.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FPSInventoryComponent.h"
#include "FPSItemBase.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSItemBase : public UObject
{
	GENERATED_BODY()
	
public:
	//-----------------------변수----------------------//
	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Quantity;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FName ID;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	EItemType ItemType;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemStatistics ItemStatistic;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemTextData ItemTextData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemNumericData ItemNumericData;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	FItemAssetData ItemAssetData;

	bool bIsCopy;
	bool bIsPickup;
	UFPSInventoryComponent* OwningInventory;

	//----------------------함수--------------------------//
	UFPSItemBase();

	void ResetItemFlags();

	UFUNCTION(Category = "Item")
	UFPSItemBase* CreateItemCopy();

	UFUNCTION(Category = "Item")
	FORCEINLINE bool IsFullItemStack() const { return Quantity == ItemNumericData.MaxStackSize; };

	UFUNCTION(Category = "Item")
	void SetQuantity(const int32 NewQuantity);

	UFUNCTION(Category = "Item")
	virtual void Use(AFPSCharacter* Character);

protected:
	bool operator==(const FName& OtherID) const
	{
		return this->ID == OtherID;
	}
};
