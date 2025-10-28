// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSItemBase.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "FPSQuickSlotWidget.generated.h"

class UFPSMainUI;
class AFPSHUD;

UCLASS()
class FPSPROJECT_API UFPSQuickSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	int index;
	UFPSMainUI* QuickSlotManager;
	AFPSHUD* HUD;

	void SetIndex(int input) { index = input; };
	int GetIndex() { return index; };
	void SetItem(UFPSItemBase* Item);
	void InitializeQuickSlotManager();

protected:
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnInitialized() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY()
	UFPSItemBase* OwnItem;
};
