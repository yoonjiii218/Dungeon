// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttack.h"
#include "EnemyController.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UEnemyAttack::UEnemyAttack()
{
	bNotifyTick = true;
	bIsAttacking = false;
}

EBTNodeResult::Type UEnemyAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy) return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		bIsAttacking = BlackboardComp->GetValueAsBool(TEXT("IsAttacking"));
		if (!bIsAttacking)
		{
			Enemy->Attack();
		}

		BlackboardComp->SetValueAsBool(TEXT("IsAttacking"), true);
	}

	return EBTNodeResult::InProgress;
}

void UEnemyAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	
	if (BlackboardComp)
	{
		bIsAttacking = BlackboardComp->GetValueAsBool(TEXT("IsAttacking"));
		if (!bIsAttacking)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
