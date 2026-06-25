#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CoworkerCharacter.h"
#include "TimerManager.h"
#include "FranzCharacter.h"
#include "Components/CapsuleComponent.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the invisible bounding box
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;

	// Default to dropping a new enemy every 5 seconds
	SpawnInterval = 8.0f; 
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	// Only let the Server spawn enemies, and make sure we assigned a class!
	if (HasAuthority() && EnemyClass)
	{
		GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true);
	}
}

void AEnemySpawner::SpawnEnemy()
{
	// 1. Check if Franz is dead. If he is, kill the Spawner timer permanently!
	if (AFranzCharacter* Player = Cast<AFranzCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (Player->GetCapsuleComponent()->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
		{
			GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
			return;
		}
	}

	if (EnemyClass)
	{
		FVector SpawnOrigin = SpawnVolume->Bounds.Origin;
		FVector SpawnExtent = SpawnVolume->Bounds.BoxExtent;
		FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);

		FRotator SpawnRotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<ACoworkerCharacter>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
}