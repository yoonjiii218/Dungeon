// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy.h"

const FName AEnemyController::TargetKey(TEXT("Target"));
const FName AEnemyController::HomePosKey(TEXT("HomePos"));
const FName AEnemyController::PatrolPosKey(TEXT("PatrolPos"));

AEnemyController::AEnemyController()
{
	ConstructorHelpers::FObjectFinder<UBehaviorTree>aiBehavior(TEXT("BehaviorTree'/Game/AI/BT_Enemy.BT_Enemy'"));
	if (aiBehavior.Succeeded())
	{
		EnemyBehaviorTree = aiBehavior.Object;
	}
}

void AEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (EnemyBehaviorTree != nullptr)
	{
		RunBehaviorTree(EnemyBehaviorTree);
		APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (player)
		{
			GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), player->GetActorLocation());
		}
		GetBlackboardComponent()->SetValueAsVector(TEXT("HomeLocation"), GetPawn()->GetActorLocation());
	}
}
