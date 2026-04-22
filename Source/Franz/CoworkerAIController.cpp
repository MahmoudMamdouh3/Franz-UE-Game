#include "CoworkerAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ACoworkerAIController::BeginPlay()
{
	Super::BeginPlay();

	// If we assigned a Behavior Tree in the Blueprint, run it!
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		
		// Find Franz in the world...
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		
		// ...and burn his location into the Coworker's memory!
		if (PlayerPawn && GetBlackboardComponent())
		{
			GetBlackboardComponent()->SetValueAsObject(FName("TargetActor"), PlayerPawn);
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Coworker locked onto Franz!"));
			}
		}
	}
}