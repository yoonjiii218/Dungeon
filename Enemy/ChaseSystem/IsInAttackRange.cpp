// Fill out your copyright notice in the Description page of Project Settings.


#include "IsInAttackRange.h"
#include "EnemyController.h"
#include "../FPSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UIsInAttackRange::UIsInAttackRange()
{
	NodeName = TEXT("CanAttack");
}

bool UIsInAttackRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	bool bResult = Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
	
	APawn* ContorllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ContorllingPawn) return false;

	AFPSCharacter* Target = Cast<AFPSCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyController::TargetKey));
	if (nullptr == Target) return false;

	bResult = (Target->GetDistanceTo(ContorllingPawn) <= 300.0f);
	return bResult;
}
