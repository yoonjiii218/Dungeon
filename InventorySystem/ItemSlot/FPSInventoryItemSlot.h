// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "FPSInventoryItemSlot.generated.h"

class UFPSDragItemVisual;
class UFPSInventoryTooltip;
class UFPSItemBase;
/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetItemReference(UFPSItemBase* ItemIn) { ItemReference = ItemIn; };
	FORCEINLINE UFPSItemBase* GetItemReference() const { return ItemReference; };
protected:
	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	TSubclassOf<UFPSDragItemVisual> DragItemVisualClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory Slot")
	TSubclassOf<UFPSInventoryTooltip> ToolTipClass;

	UPROPERTY(VisibleAnyWhere, Category="Inventory Slot")
	UFPSItemBase* ItemReference;
	
	UPROPERTY(VisibleAnyWhere, Category = "Inventory Slot", meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(VisibleAnyWhere, Category = "Inventory Slot", meta=(BindWidget))
	UImage* ItemIcon;

	UPROPERTY(VisibleAnyWhere, Category = "Inventory Slot", meta = (BindWidget))
	UTextBlock* ItemQuantity;

	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
