#include "CoworkerCharacter.h"

// Notice the ACoworkerCharacter:: prefix here!
ACoworkerCharacter::ACoworkerCharacter()
{
	MaxHealth = 75.0f;
	CurrentHealth = MaxHealth;
}

// Notice the ACoworkerCharacter:: prefix here too!
void ACoworkerCharacter::Die()
{
	Super::Die(); 

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, TEXT("A Coworker has been defeated."));
	}

	Destroy();
}