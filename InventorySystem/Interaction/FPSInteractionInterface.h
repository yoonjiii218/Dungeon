// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPSInteractionInterface.generated.h"

class AFPSCharacter;

UENUM()
enum class EInteractableType : uint8
{
	Pickup UMETA(DisplayName = "Pickup"),
	NonPlayerCharacter UMETA(DisplayName = "NonPlayerCharacter"),
	Device UMETA(DisplayName = "Device"), //��ġ�� ��ȣ�ۿ� ex) ���̳� ���ڰ��� ��ġ
	Toggle UMETA(DisplayName = "Toggle"), //��ư ��ȣ�ۿ�
	Container UMETA(DisplayName = "Container")
};

USTRUCT()
struct FInteractableData
{
	GENERATED_USTRUCT_BODY()

	FInteractableData() : 
	InteractableType(EInteractableType::Pickup),
	Name(FText :: GetEmpty()),
	Action(FText :: GetEmpty()),
	Quantity(0),
	InteractionDuration(0.0f)
	{

	};

	UPROPERTY(EditInstanceOnly)
	EInteractableType InteractableType;

	UPROPERTY(EditInstanceOnly)
	FText Name;

	UPROPERTY(EditInstanceOnly)
	FText Action;

	UPROPERTY(EditInstanceOnly)
	int8 Quantity;

	UPROPERTY(EditInstanceOnly)
	float InteractionDuration;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFPSInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FPSPROJECT_API IFPSInteractionInterface
{
	GENERATED_BODY()

public:
	virtual void BeginFocus();
	virtual void EndFocus();
	virtual void BeginInteract();
	virtual void EndInteract();
	virtual void Interact(AFPSCharacter* PlayerCharacter);
	

	FInteractableData InteractableData;
};
