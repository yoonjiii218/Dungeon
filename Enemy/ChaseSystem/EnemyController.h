// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	
public:
	static const FName TargetKey;
	static const FName HomePosKey;
	static const FName PatrolPosKey;

	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* aPawn) override;

protected:
	AEnemyController();

private:
	UPROPERTY(EditAnywhere)
	class UBehaviorTree* EnemyBehaviorTree;
};
