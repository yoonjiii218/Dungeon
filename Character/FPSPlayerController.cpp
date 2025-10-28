// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"


void AFPSPlayerController::BeginPlay() {
	Super::BeginPlay();

	//subsystem�� mappingcontext �ֱ�
	//�̶� subsystem�� inputsystem ��ü�� �����ϸ� ��Ȳ�� �´� mappingcontext�� ������
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
