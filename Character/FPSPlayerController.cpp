// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"


void AFPSPlayerController::BeginPlay() {
	Super::BeginPlay();

	//subsystem에 mappingcontext 넣기
	//이때 subsystem은 inputsystem 전체를 관리하며 상황에 맞는 mappingcontext를 연결함
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer())) {
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
