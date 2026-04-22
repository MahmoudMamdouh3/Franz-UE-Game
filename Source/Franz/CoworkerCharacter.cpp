#include "CoworkerCharacter.h"

ACoworkerCharacter::ACoworkerCharacter()
{
	// Coworkers are hollowed out, so they have less health than Franz
	MaxHealth = 75.0f;
	CurrentHealth = MaxHealth;
}

void ACoworkerCharacter::Die()
{
	// Super::Die() calls the original function in BaseCharacter which disables their collision
	Super::Die(); 

	// For now, we will print a purple message. 
	// Later, we will trigger the ragdoll/death animation here!
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, TEXT("A Coworker has been defeated."));
	}
}