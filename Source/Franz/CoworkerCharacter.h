#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CoworkerCharacter.generated.h"

UCLASS()
class FRANZ_API ACoworkerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ACoworkerCharacter();

	// --- NEW: Expose this so the Behavior Tree can trigger it! ---
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void MeleeAttack();

protected:
	virtual void Die() override;

	// --- NEW: Coworker Attack variables ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	class UAnimMontage* AttackMontage;

	UFUNCTION(Server, Reliable)
	void Server_PerformMeleeHit();
};