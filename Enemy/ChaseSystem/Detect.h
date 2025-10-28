// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Detect.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UDetect : public UBTService
{
	GENERATED_BODY()

public:
	UDetect();

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
