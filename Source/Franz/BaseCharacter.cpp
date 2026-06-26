#include "BaseCharacter.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

		
		
		if (GEngine)
		{
			FString HealthMsg = FString::Printf(TEXT("%s Health: %f / %f"), *GetName(), CurrentHealth, MaxHealth);
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, HealthMsg);
		}
		if (HasAuthority() && !bIsDead)
		{
			CurrentHealth -= ActualDamage;
			CurrentHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);

			// --- THE CRUNCH: Play the physical impact sound ---
			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
			}

			if (GEngine)
			{
				FString HealthMsg = FString::Printf(TEXT("%s Health: %f / %f"), *GetName(), CurrentHealth, MaxHealth);
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, HealthMsg);
			}
		if (CurrentHealth <= 0.0f)
		{
			Die();
		}
		else 
		{
			// --- NEW: Play the flinch animation if they survived the hit! ---
			if (HitReactMontage)
			{
				PlayAnimMontage(HitReactMontage);
			}
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

	// 1. Turn off the invisible collision cylinder so we don't block the floor
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	// 2. Turn ON the physics skeleton so the body collapses
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	// 3. Kill the movement engine
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();

	// 4. Disconnect the brain (Whether it's Franz's keyboard or the Coworker's AI)
	if (Controller)
	{
		Controller->UnPossess();
	}
}