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

	//InputActionMove라는 변수에 움직이기 위한 action 넣어주기
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

	//일인칭 카메라 컴포넌트
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FPSCameraComponent != nullptr);

	//캡슐 컴포넌트에 카메라 컴포넌트 붙이기
	FPSCameraComponent->SetupAttachment(CastChecked<USceneComponent, UCapsuleComponent>(GetCapsuleComponent()));

	//카메라가 눈 약간 위로 위치하게
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));

	//폰이 카메라 회전을 제어할 수 있도록
	FPSCameraComponent->bUsePawnControlRotation = true;

	// 소유 플레이어의 일인칭 메시 컴포넌트를 생성합니다.
	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FPSMesh != nullptr);

	// 소유 플레이어만 이 메시를 볼 수 있습니다.
	FPSMesh->SetOnlyOwnerSee(true);

	// FPS 메시를 FPS 카메라에 어태치합니다.
	FPSMesh->SetupAttachment(FPSCameraComponent);

	// 일부 인바이런먼트 섀도를 비활성화하여 단일 메시 같은 느낌을 보존합니다.
	FPSMesh->bCastDynamicShadow = false;
	FPSMesh->CastShadow = false;

	GetMesh()->SetOwnerNoSee(true);

	InteractionCheckFrequency = 0.1f;
	InteractionCheckDiatance = 500.0f;

	PlayerInventory = CreateDefaultSubobject<UFPSInventoryComponent>(TEXT("PlayerInventory"));
	PlayerInventory->SetSlotCapacity(12);
	PlayerInventory->SetWeightCapacity(12.0f); //생략가능
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	//fpscharacter가 잘 소환되었다는 log
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

		//2D 스크린좌표를 월드좌표로 변환
		if (PlayerController->DeprojectScreenPositionToWorld(ScreenX, ScreenY, WorldLocation, WorldDirection))
		{
			TraceStart = WorldLocation;
		}
	}
	FVector TraceEnd{ TraceStart + (GetViewRotation().Vector() * InteractionCheckDiatance) };

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	if (LookDirection > 0) 
		//플레이어가 보는 방향과 화면 속 pawn이 보고 있는 방향이 같을 때에만 상호작용 탐지
		//FPS에서는 딱히 신경 안쓰일거 같지만 그냥 함 넣어봄
	{
		//라인 그리기
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 1.0f, 0, 2.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this); //자기 자신은 감지하지 않음
		FHitResult TraceHit;

		//TraceStart부터 TraceEnd까지 그은 라인에 무언가 감지되면
		if (GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{
			//Hit된 Actor에 interface가 있으면
			if (TraceHit.GetActor()->GetClass()->ImplementsInterface(UFPSInteractionInterface::StaticClass()))
			{

				//직전에 감지된 것이 아니고, 상호작용할 수 있는 거리에 놓인 것일 때
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
	if (IsInteracting()) //상호작용중인 것이 있을 때 타이머 종료
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
		//스폰할 아이템 위치 및 각도 초기화
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.bNoFail = true;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		const FVector SpawnLocation{ GetActorLocation() + (GetActorForwardVector() + 100.0f) };
		const FTransform SpawnTransform(GetActorRotation(), SpawnLocation);
		
		const int32 RemoveQuantity = PlayerInventory->RemoveAmountOfItem(ItemToDrop, QuantityToDrop);

		PlayerInventory->RemoveSingleInstanceOfItem(ItemToDrop); //인벤토리에서 제거
		APickUp* Pickup = GetWorld()->SpawnActor<APickUp>(APickUp::StaticClass(), SpawnTransform, SpawnParams);

		// 만든 스폰객체에 아이템 정보 초기화
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

//캐릭터를 움직이는 함수
void AFPSCharacter::Move(const FInputActionValue& Value) {
	//입력은 vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr) {
		//AddMovementInput를 이용해서 원하는 방향의 좌표를 더해서 이동한다.
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
		//카메라의 transform
		FVector CameraLocation;
		FRotator CameraRotation;
		//플레이어 즉, 캐릭터의 눈 위치와 시선을 반환하는 함수
		//GetPlayerViewPoint는 카메라 시점을 반환하는 함수
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		//카메라 살짝 앞에서 발사체 생성
		MuzzleOffset.Set(100.0f, 0.0f, 0.0f);

		//카메라 스페이스에서 월드 스페이스로 변환
		FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		//조준이 살짝 위를 향하도록 왜곡
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch += 10.0f;

		UWorld* World = GetWorld();
		if (World) {
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			//총구에서 발사체 스폰
			AFPSProjectile* Projectile = World->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile) {
				//발사체의 초기 탄도 설정
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
