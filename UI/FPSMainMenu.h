// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSMainMenu.generated.h"

class AFPSCharacter;
/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSMainMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AFPSCharacter* PlayerCharacter;
	
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
