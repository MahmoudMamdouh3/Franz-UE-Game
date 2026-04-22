#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CoworkerAIController.generated.h"

class UBehaviorTree;

UCLASS()
class FRANZ_API ACoworkerAIController : public AAIController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// This allows us to assign a Behavior Tree in the Blueprint editor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* AIBehavior;
};