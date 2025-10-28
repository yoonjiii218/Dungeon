// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "FPSStamina.generated.h"

class AFPSCharacter;

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UFPSStamina : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaBar;

	UPROPERTY()
	AFPSCharacter* PlayerCharacter;

	UFUNCTION()
	void UpdateStamina(float StaminaPercent);

protected:
	virtual void NativeOnInitialized() override;
};
