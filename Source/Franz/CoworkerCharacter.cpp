#include "CoworkerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"  // Add this line!
#include "FranzCharacter.h"

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


void ACoworkerCharacter::MeleeAttack()
{
	// FIX: Find Franz's physical body in the world directly
	if (AFranzCharacter* Player = Cast<AFranzCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AFranzCharacter::StaticClass())))
	{
		if (Player->GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			// The exact moment Franz dies, sever the AI Controller. They will freeze completely.
			if (Controller)
			{
				Controller->UnPossess();
			}
			return; 
		}
	}

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
		// 2. FRIENDLY FIRE FIX: We cast directly to Franz. If the fist hits another Coworker, this fails and no damage is dealt!
		if (AFranzCharacter* HitPlayer = Cast<AFranzCharacter>(HitResult.GetActor()))
		{
			// 3. DAMAGE NERF: Dropped from 20 to 5 damage per punch
			UGameplayStatics::ApplyDamage(HitPlayer, 5.0f, GetController(), this, UDamageType::StaticClass());
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Franz took 5 damage!")));
			}
		}
	}
}
void ACoworkerCharacter::Die()
{
	// 1. Let the master class trigger the physical ragdoll
	Super::Die();

	// 2. Find the player and grant them 1 kill
	if (AFranzCharacter* Player = Cast<AFranzCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		Player->AddKill();
	}
}