// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSItemBase.h"
#include "FPSMainUI.generated.h"

class UFPSQuickSlotWidget;

UCLASS()
class FPSPROJECT_API UFPSMainUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UFPSQuickSlotWidget* QuickSlot1;

	UPROPERTY(meta = (BindWidget))
	UFPSQuickSlotWidget* QuickSlot2;

	UPROPERTY(meta = (BindWidget))
	UFPSQuickSlotWidget* QuickSlot3;

	UFPSItemBase* Test;

	void UpdateQuickSlotUI();
	void Scroll(int32 Direction);
	UFPSItemBase* GetCurrentItem();
	void AddItem(int index, UFPSItemBase* inputItem);
	void RemoveItem(int index);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY()
	TArray<UFPSQuickSlotWidget*> Slots;

	UPROPERTY()
	TArray<UFPSItemBase*> Items;
	
	int32 CurrentIndex;
};

