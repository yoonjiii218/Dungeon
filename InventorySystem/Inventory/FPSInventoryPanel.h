// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "Components/TextBlock.h"
#include "../FPSCharacter.h"
#include "FPSInventoryItemSlot.h"
#include "FPSQuickSlotWidget.h"
#include "FPSInventoryPanel.generated.h"

class UFPSInventoryComponent;

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RefreshInventory();

	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryPanel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY(meta = (BindWidget))
	UFPSQuickSlotWidget* QuickSlot1;

	UPROPERTY(meta = (BindWidget))
	UFPSQuickSlotWidget* QuickSlot2;

	UPROPERTY(meta = (BindWidget))
	UFPSQuickSlotWidget* QuickSlot3;

	UPROPERTY()
	AFPSCharacter* PlayerCharacter;

	UPROPERTY()
	UFPSInventoryComponent* InventoryReference;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UFPSInventoryItemSlot> InventorySlotClass;

protected:
	TArray<UFPSQuickSlotWidget*> Slots;

	void SetInfoText() const;
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
