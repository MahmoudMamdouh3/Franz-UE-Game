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

protected:
	// We override the base Die function to add custom Coworker logic
	virtual void Die() override;
};