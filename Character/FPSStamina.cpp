// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSStamina.h"
#include "../FPSCharacter.h"

void UFPSStamina::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PlayerCharacter = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	if (PlayerCharacter)
	{
		PlayerCharacter->OnstaminaChanged.AddDynamic(this, &UFPSStamina::UpdateStamina);
	}
}

void UFPSStamina::UpdateStamina(float StaminaPercent)
{
	if (StaminaBar)
	{
		float ClampedStamina = FMath::Clamp(StaminaPercent, 0.0f, 1.0f);
		StaminaBar->SetPercent(ClampedStamina);
	}
}
