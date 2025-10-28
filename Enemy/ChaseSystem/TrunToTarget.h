// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TrunToTarget.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UTrunToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UTrunToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
