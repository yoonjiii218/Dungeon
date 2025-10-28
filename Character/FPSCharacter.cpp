// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "InputACtionValue.h"
#include "PickUp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSMainUI.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//InputActionMove��� ������ �����̱� ���� action �־��ֱ�
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMove = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'");
	if (InputActionMove.Object) MoveAction = InputActionMove.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJump = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'");
	if (InputActionJump.Object) JumpAction = InputActionJump.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionLook = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Look.IA_Look'");
	if (InputActionLook.Object) LookAction = InputActionLook.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionInteract = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Interaction.IA_Interaction'");
	if (InputActionInteract.Object) InteractAction = InputActionInteract.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionIToggle = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_ToggleMenu.IA_ToggleMenu'");
	if (InputActionIToggle.Object) ToggleMenuAction = InputActionIToggle.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionRun = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Run.IA_Run'");
	if (InputActionRun.Object) RunAction = InputActionRun.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionQuickSlot = TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_QuickSlotScroll.IA_QuickSlotScroll'");
	if (InputActionQuickSlot.Object) QuickSlotAction = InputActionQuickSlot.Object;

	//����Ī ī�޶� ������Ʈ
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);

	//ĸ�� ������Ʈ�� ī�޶� ������Ʈ ���̱�
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	//ī�޶� �� �ణ ���� ��ġ�ϰ�
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	//���� ī�޶� ȸ���� ������ �� �ֵ���
	FPSCameraComponent->bUsePawnControlRotation = true;

	// ���� �÷��̾��� ����Ī �޽� ������Ʈ�� �����մϴ�.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// ���� �÷��̾ �� �޽ø� �� �� �ֽ��ϴ�.
	FPSMesh->SetOnlyOwnerSee(true);

	// FPS �޽ø� FPS ī�޶� ����ġ�մϴ�.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// �Ϻ� �ι��̷���Ʈ ������ ��Ȱ��ȭ�Ͽ� ���� �޽� ���� ������ �����մϴ�.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	InteractionCheckFrequency = 0.1f;
	InteractionCheckDiatance = 500.0f;

	PlayerInventory = CreateDefaultSubobject<UFPSInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotCapacity(12);
	PlayerInventory->SetWeightCapacity(12.0f); //��������
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//fpscharacter�� �� ��ȯ�Ǿ��ٴ� log
	//check(GEngine != nullptr);
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));

	HUD = Cast<AFPSHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
	if (HUD)
	{
		QuickSlotManager = HUD->MainUIWidget;
	}
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

	if (IsRunning)
	{
		DecreaseStamina(DeltaTime);
	}
}


void AFPSCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();
	PlayerController = GetWorld()->GetFirstPlayerController();

	FVector TraceStart;
	FVector WorldLocation;
	FVector WorldDirection;

	if (PlayerController)
	{
		int32 ScreenWidth, ScreenHeight;
		PlayerController->GetViewportSize(ScreenWidth, ScreenHeight);

		float ScreenX = ScreenWidth / 2.0f;
		float ScreenY = ScreenHeight / 2.0f;

		//2D ��ũ����ǥ�� ������ǥ�� ��ȯ
		if (PlayerController->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection))
		{
			TraceStart = WorldLocation;
		}
	}
	FVector TraceEnd{ TraceStart + (GetViewRotation().Vector() * InteractionCheckDiatance) };

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0) 
		//�÷��̾ ���� ����� ȭ�� �� pawn�� ���� �ִ� ������ ���� ������ ��ȣ�ۿ� Ž��
		//FPS������ ���� �Ű� �Ⱦ��ϰ� ������ �׳� �� �־
	{
		//���� �׸���
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this); //�ڱ� �ڽ��� �������� ����
		FHitResult TraceHit;

		//TraceStart���� TraceEnd���� ���� ���ο� ���� �����Ǹ�
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			//Hit�� Actor�� interface�� ������
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UFPSInteractionInterface::StaticClass()))
			{

				//������ ������ ���� �ƴϰ�, ��ȣ�ۿ��� �� �ִ� �Ÿ��� ���� ���� ��
				if (TraceHit.GetActor() != InteractionData.CurrentInteractable)
				{
					FoundInteractable(TraceHit.GetActor());
					return;
				}

				if (TraceHit.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}

	NoInteractableFound();
}

void AFPSCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting()) //��ȣ�ۿ����� ���� ���� �� Ÿ�̸� ����
	{
		EndInteract();
	}

	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}

	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;
	
	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);

	TargetInteractable->BeginFocus();
}

void AFPSCharacter::NoInteractableFound()
{
	if (IsInteracting()) 
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable) 
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		HUD->HideInteractionWidget();

		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AFPSCharacter::BeginInteract()
{
	// verify nothing has changed with the interactable state since beginning interaction
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable) 
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
				Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction,
					this,
					&AFPSCharacter::Interact,
					TargetInteractable->InteractableData.InteractionDuration,
					false);
			}
		}
	}
}

void AFPSCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void AFPSCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AFPSCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AFPSCharacter::ToggleMenu()
{
	HUD->ToggleMenu();
}

void AFPSCharacter::DropItem(UFPSItemBase* ItemToDrop, const int32 QuantityToDrop)
{
	if (PlayerInventory->FindMatchingItem(ItemToDrop))
	{
		//������ ������ ��ġ �� ���� �ʱ�ȭ
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() + 100.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		
		const int32 RemoveQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		PlayerInventory->RemoveSingleInstanceOfItem(ItemToDrop); //�κ��丮���� ����
		APickUp* Pickup = GetWorld()->SpawnActor<APickUp>(APickUp::StaticClass(), SpawnTransform, SpawnParams);

		// ���� ������ü�� ������ ���� �ʱ�ȭ
		Pickup->InitializeDrop(ItemToDrop, RemoveQuantity);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item to drop was somehow null!"));
	}
}


// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPSCharacter::StartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopJump);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFPSCharacter::Fire);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPSCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AFPSCharacter::EndInteract);

		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &AFPSCharacter::ToggleMenu);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &AFPSCharacter::StartRunning);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AFPSCharacter::EndRunning);

		EnhancedInputComponent->BindAction(QuickSlotAction, ETriggerEvent::Triggered, this, &AFPSCharacter::ScrollQuickSlot);
	}
}

//ĳ���͸� �����̴� �Լ�
void AFPSCharacter::Move(const FInputActionValue& Value) {
	//�Է��� vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		//AddMovementInput�� �̿��ؼ� ���ϴ� ������ ��ǥ�� ���ؼ� �̵��Ѵ�.
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSCharacter::StartRunning()
{
	if (CurrentStamina > 0.0f && !IsRunning && !IsRegnerate) {
		IsRunning = true;
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
		GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
		UE_LOG(LogTemp, Warning, TEXT("Pressed Shift, %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
}

void AFPSCharacter::EndRunning()
{
	if (IsRunning) {
		IsRunning = false;
		GetCharacterMovement()->MaxWalkSpeed = 500.0f;
		GetWorld()->GetTimerManager().SetTimer(StaminaRegenTimer, this, &AFPSCharacter::RegenerateStamina, 0.1f, true);
		UE_LOG(LogTemp, Warning, TEXT("Released Shift, %f"), GetCharacterMovement()->MaxWalkSpeed);
	}
}

void AFPSCharacter::DecreaseStamina(float DelatTime)
{
	CurrentStamina  = FMath::Clamp(CurrentStamina - StaminaDecreaseRate * DelatTime, 0.0f, MaxStamina);

	if (CurrentStamina <= 0.0f)
	{
		IsRegnerate = true;
		EndRunning();
	}

	OnstaminaChanged.Broadcast(CurrentStamina);
}

void AFPSCharacter::RegenerateStamina()
{
	CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRegenRate * 0.1f, 0.0f, MaxStamina);

	if (CurrentStamina >= MaxStamina)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
		IsRegnerate = false;
	}

	OnstaminaChanged.Broadcast(CurrentStamina);
}

void AFPSCharacter::StartJump() {
	Jump();
}

void AFPSCharacter::StopJump() {
	StopJumping();
}

void AFPSCharacter::Look(const FInputActionValue& Value) {
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSCharacter::Fire() {
	if (ProjectileClass) {
		//ī�޶��� transform
		FVector CameraLocation;
		FRotator CameraRotation;
		//�÷��̾� ��, ĳ������ �� ��ġ�� �ü��� ��ȯ�ϴ� �Լ�
		//GetPlayerViewPoint�� ī�޶� ������ ��ȯ�ϴ� �Լ�
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		//ī�޶� ��¦ �տ��� �߻�ü ����
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		//ī�޶� �����̽����� ���� �����̽��� ��ȯ
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		//������ ��¦ ���� ���ϵ��� �ְ�
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			//�ѱ����� �߻�ü ����
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile) {
				//�߻�ü�� �ʱ� ź�� ����
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}


void AFPSCharacter::ScrollQuickSlot(const FInputActionValue& InputValue)
{
	int32 Direction = InputValue.GetMagnitude();
	QuickSlotManager->Scroll(Direction);
}

void AFPSCharacter::UseCurrentItem()
{
	UFPSItemBase* CurrentItem;
	CurrentItem = QuickSlotManager->GetCurrentItem();
	CurrentItem->Use(this);
}
