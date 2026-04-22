#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class FRANZ_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Required to tell the engine which variables to sync over the network
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// The core combat function that handles brutal damage
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	// --- HEALTH SYSTEM ---
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxHealth;

	// ReplicatedUsing means: "Whenever the server changes this, tell all clients to run OnRep_CurrentHealth"
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UFUNCTION()
	void OnRep_CurrentHealth();

	// --- DEATH SYSTEM ---
	UPROPERTY(ReplicatedUsing = OnRep_IsDead, VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	bool bIsDead;

	UFUNCTION()
	void OnRep_IsDead();

	// The function where we will eventually play the heavy death animations
	virtual void Die();
};