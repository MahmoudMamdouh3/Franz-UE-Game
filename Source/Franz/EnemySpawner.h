#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class FRANZ_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();

protected:
	virtual void BeginPlay() override;

	// The invisible 3D box where enemies are allowed to spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* SpawnVolume;

	// The exact blueprint class to spawn (BP_Coworker)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawner")
	TSubclassOf<class ACoworkerCharacter> EnemyClass;

	// How many seconds between each spawn
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnInterval;

	FTimerHandle SpawnTimerHandle;

	void SpawnEnemy();
};