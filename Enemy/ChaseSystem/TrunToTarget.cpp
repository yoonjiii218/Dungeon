// Fill out your copyright notice in the Description page of Project Settings.


#include "TrunToTarget.h"
#include "EnemyController.h"
#include "../FPSCharacter.h"
#include "Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"

UTrunToTarget::UTrunToTarget()
{
	NodeName = TEXT("Turn");
}

EBTNodeResult::Type UTrunToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto Enemy = Cast<AEnemy>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == Enemy) return EBTNodeResult::Failed;

	auto Target = Cast<AFPSCharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AEnemyController::TargetKey));
	if (nullptr == Target) return EBTNodeResult::Failed;

	FVector LookVector = Target->GetActorLocation() - Enemy->GetActorLocation();
	LookVector.Z = 0.0f;
	FRotator TargetRotaion = FRotationMatrix::MakeFromX(LookVector).Rotator();
	Enemy->SetActorRotation(FMath::RInterpTo(Enemy->GetActorRotation(), TargetRotaion, GetWorld()->GetDeltaSeconds(), 2.0f));

	return EBTNodeResult::Succeeded;
}
