// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "FPSItemDragDropOperation.generated.h"


class UFPSItemBase;
class UFPSInventoryComponent;
/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UFPSItemBase* SourceItem;

	UPROPERTY()
	UFPSInventoryComponent* SourceInventory;
};
