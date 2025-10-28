// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSInteractionInterface.h"
#include "../FPSCharacter.h"
#include "PickUp.generated.h"

class UFPSItemBase;

UCLASS()
class FPSPROJECT_API APickUp : public AActor, public IFPSInteractionInterface
{
	GENERATED_BODY()
	
public:	
	APickUp();

	void InitializePickup(const TSubclassOf<UFPSItemBase> BaseClass, const int32 InQuantity);

	void InitializeDrop(UFPSItemBase* ItemDrop, const int32 InQuantity);

	FORCEINLINE UFPSItemBase* GetItemData() { return ItemReference; }

	virtual void BeginFocus() override;
	virtual void EndFocus() override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Components")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Database")
	UDataTable* ItemDataTable;

	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Database")
	FName DesiredItemID;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Database")
	UFPSItemBase* ItemReference;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Database")
	int32 ItemQuantity;

	UPROPERTY(VisibleAnywhere, Category = "Pickup | Item Database")
	FInteractableData InstanceInteractableData;

	virtual void BeginPlay() override;

	virtual void Interact(AFPSCharacter* PlayerCharacter) override;
	void UpdateInteractableData();

	void TakePickup(const AFPSCharacter* Taker);
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
