// Fill out your copyright notice in the Description page of Project Settings.


#include "Detect.h"
#include "EnemyController.h"
#include "../FPSCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/OverlapResult.h"

UDetect::UDetect()
{
	NodeName = TEXT("Detect");
	Interval = 1.0f;
}

void UDetect::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (nullptr == ControllingPawn) return;

	UWorld* World = ControllingPawn->GetWorld();
	if (nullptr == World) return;

	FVector Center = ControllingPawn->GetActorLocation();
	float DetectRadius = 600.0f;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, ControllingPawn);

	bool bResult = World->OverlapMultiByChannel(
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(DetectRadius),
		CollisionQueryParam
	);

	if (bResult)
	{
		for (auto const& result : OverlapResults)
		{
			AActor* OverlappedActor = result.GetActor();

			if (OverlappedActor)
			{
				AFPSCharacter* Player = Cast<AFPSCharacter>(OverlappedActor);
				if (Player)
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyController::TargetKey, Player);
					DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Green, false, 0.2f);
					DrawDebugLine(World, ControllingPawn->GetActorLocation(), Player->GetActorLocation(), FColor::Blue, false, 0.2f);
					return;
				}
			}
		}
	}

	DrawDebugSphere(World, Center, DetectRadius, 16, FColor::Red, false, 0.2f);
	OwnerComp.GetBlackboardComponent()->SetValueAsObject(AEnemyController::TargetKey, nullptr);
}
