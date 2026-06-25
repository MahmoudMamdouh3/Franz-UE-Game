#include "EnemySpawner.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CoworkerCharacter.h"
#include "TimerManager.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create the invisible bounding box
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	RootComponent = SpawnVolume;

	// Default to dropping a new enemy every 5 seconds
	SpawnInterval = 5.0f; 
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
	if (EnemyClass)
	{
		// 1. Calculate a completely random X,Y,Z coordinate inside our Box Component
		FVector SpawnOrigin = SpawnVolume->Bounds.Origin;
		FVector SpawnExtent = SpawnVolume->Bounds.BoxExtent;
		FVector SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnExtent);

		// 2. Set spawning rules so they don't get stuck in the floor
		FRotator SpawnRotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		// 3. Drop them into the arena
		GetWorld()->SpawnActor<ACoworkerCharacter>(EnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
	}
}