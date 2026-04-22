#include "CoworkerCharacter.h"

ACoworkerCharacter::ACoworkerCharacter()
{
	MaxHealth = 75.0f;
	CurrentHealth = MaxHealth;
}

void ACoworkerCharacter::Die()
{
	Super::Die(); 

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Purple, TEXT("A Coworker has been defeated."));
	}

	Destroy();
}