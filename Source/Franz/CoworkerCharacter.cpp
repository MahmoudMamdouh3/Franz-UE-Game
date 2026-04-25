#include "CoworkerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"  // Add this line!

ACoworkerCharacter::ACoworkerCharacter()
{

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    
	// Force the mesh to sit correctly inside the capsule
	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, -96.0f),      // Z = negative capsule half-height
		FRotator(0.0f, -90.0f, 0.0f)       // -90 yaw so he faces forward
	);

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

void ACoworkerCharacter::MeleeAttack()
{
	if (AttackMontage)
	{
		PlayAnimMontage(AttackMontage);
		
		FTimerHandle AttackTimerHandle;
		GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &ACoworkerCharacter::Server_PerformMeleeHit, 0.4f, false);
	}
}

void ACoworkerCharacter::Server_PerformMeleeHit_Implementation()
{
	FVector StartLoc = GetActorLocation();
	FVector ForwardVec = GetActorForwardVector();
	FVector EndLoc = StartLoc + (ForwardVec * 100.0f); 

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); 

	bool bHit = GetWorld()->SweepSingleByChannel(
		HitResult, StartLoc, EndLoc, FQuat::Identity, ECC_Pawn, 
		FCollisionShape::MakeSphere(40.0f), QueryParams
	);

	DrawDebugSphere(GetWorld(), EndLoc, 40.0f, 12, FColor::Red, false, 2.0f);

	if (bHit && HitResult.GetActor())
	{
		// Coworkers hit a bit weaker than Franz (20 damage)
		UGameplayStatics::ApplyDamage(HitResult.GetActor(), 20.0f, GetController(), this, UDamageType::StaticClass());
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Franz took damage from Coworker!")));
		}
	}
}