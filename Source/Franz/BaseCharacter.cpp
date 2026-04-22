#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Default starting stats
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	bIsDead = false;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// The server sets the initial health when spawning in
	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
	}
}

// This registers our variables for Multiplayer syncing
void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, CurrentHealth);
	DOREPLIFETIME(ABaseCharacter, bIsDead);
}

// The core logic when Franz or a Coworker gets punched
float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (HasAuthority() && !bIsDead)
	{
		CurrentHealth -= ActualDamage;
		CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

		// --- NEW: Print the Health to the screen! ---
		if (GEngine)
		{
			FString HealthMsg = FString::Printf(TEXT("%s Health: %f / %f"), *GetName(), CurrentHealth, MaxHealth);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, HealthMsg);
		}

		if (CurrentHealth <= 0.0f)
		{
			Die();
		}
	}

	return ActualDamage;
}

// Triggered on clients when their health changes (Useful for updating the UI later)
void ABaseCharacter::OnRep_CurrentHealth()
{
	// We will add UI update logic here in Phase 5
}

// Triggered on clients when the server says someone died
void ABaseCharacter::OnRep_IsDead()
{
	if (bIsDead)
	{
		Die(); 
	}
}

void ABaseCharacter::Die()
{
	if (bIsDead) return; 
	
	bIsDead = true;
	SetActorEnableCollision(false);
}