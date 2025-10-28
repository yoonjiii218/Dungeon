// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"
#include "EnemyController.h"
#include "EnemyAttack.h"
#include "BehaviorTree/BlackboardComponent.h"


void UEnemyAnimInstance::AnimNotify_AttackEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("EndAttack notify Triggered"));

	APawn* PawnOwner = Cast<APawn>(TryGetPawnOwner());
	if (PawnOwner)
	{
		AEnemyController* AIController = Cast<AEnemyController>(PawnOwner->GetController());
		if (AIController)
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool(TEXT("IsAttacking"), false);
			}
		}
	}
}
