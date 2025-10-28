// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "IsInAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UIsInAttackRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	UIsInAttackRange();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
