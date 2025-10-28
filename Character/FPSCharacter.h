// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Public/FPSInteractionInterface.h"
#include "Public/FPSInventoryComponent.h"
#include "FPSHUD.h"
#include "Logging/LogMacros.h"
#include "FPSProjectile.h"
#include "FPSPlayerController.h"
#include "FPSMainUI.h"
#include "FPSCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class UInputComponent;
struct FInputActionValue;

USTRUCT()
struct FInteractionData 
{
	GENERATED_USTRUCT_BODY()

	FInteractionData() : 
	CurrentInteractable(nullptr), 
	LastInteractionCheckTime(0.0f)
	{

	};

	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, float, NewStamina);

UCLASS(config=Game)
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	//Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	//Jump
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	//Look
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	//Fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	//Interaction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	//Menu
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleMenuAction;

	//Run
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RunAction;

	//QuickSlot
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* QuickSlotAction;

protected:
	//-----------------------변수----------------------//
	UPROPERTY()
	AFPSHUD* HUD;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AFPSProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
	TScriptInterface<IFPSInteractionInterface> TargetInteractable;

	UPROPERTY(VisibleAnywhere, Category = "Character | Inventory")
	UFPSInventoryComponent* PlayerInventory;

	float InteractionCheckFrequency;

	float InteractionCheckDiatance;

	APlayerController* PlayerController;

	FTimerHandle TimerHandle_Interaction;
	FInteractionData InteractionData;
	
	//----------------------함수--------------------------//
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void StartRunning();
	void EndRunning();
	void StartJump();
	void StopJump();
	void Look(const FInputActionValue& Value);
	void Fire();


	void ToggleMenu();

	void PerformInteractionCheck();
	void FoundInteractable(AActor* NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();

public:	
	//-----------------------변수----------------------//
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* FPSMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	bool IsRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
	bool IsRegnerate;

	//Stamina
	float CurrentStamina = 1.0f;
	float MaxStamina = 1.0f;
	float StaminaDecreaseRate = 0.5f;
	float StaminaRegenRate = 0.3f;

	FTimerHandle StaminaRegenTimer;

	UPROPERTY(BlueprintAssignable, Category="Move")
	FOnStaminaChanged OnstaminaChanged;

	//QuickSlot
	UFPSMainUI* QuickSlotManager;
	UFPSItemBase* EquippedItem;

	//----------------------함수--------------------------//
	AFPSCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction); };

	FORCEINLINE UFPSInventoryComponent* GetInventory() const { return PlayerInventory; };

	void UpdateInteractionWidget() const;

	void DropItem(UFPSItemBase* ItemToDrop, const int32 QuantityToDrop);

	void DecreaseStamina(float DelatTime);

	void RegenerateStamina();

	void ScrollQuickSlot(const FInputActionValue& InputValue);

	void UseCurrentItem();

	void Interact();
};
