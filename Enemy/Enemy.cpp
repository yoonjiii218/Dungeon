// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyAttack.h"
#include "Components/WidgetComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	EnemyHP = CreateDefaultSubobject<UWidgetComponent>(L"WidgetComponent");
	//EnemyHP->SetupAttachment(GetCapsuleComponent());
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}


void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Attack"));
	PlayAttackMontage();
}

void AEnemy::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(AttackMontage);
}

