// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

#include "AmmoItem.h"
#include "SpawnPoint.h"
#include "Cockroach.h"
#include "ProjectCastleDoctrinCharacter.h"

#include "SpawnManager.generated.h"

UCLASS(Blueprintable)
class PROJECTCASTLEDOCTRIN_API ASpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void startRound();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACockroach> enemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _timeInBetweenRounds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _initialRoundStartDelay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int _baseNumberToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxSpeed = 650.0f;

	UFUNCTION(BlueprintCallable)
	void spawnEnemy(ASpawnPoint* tp);

	UPROPERTY(BlueprintReadOnly)
	int _round = 0;

private:

	int calculateNumberOfEnemiesToSpawn();
	void startSpawningTimers();

	UFUNCTION()
	void enemyDied(ACockroach* enemy);

private:

	struct SpawnPointInfo
	{
		int numToSpawnThisRound = 0;
		int numSpawnedThisRound = 0;
		int totalNumSpawned = 0;
		FTimerHandle spawnTimerHandle;
		FTimerDelegate spawnTimerDelegate;
	};

	TMap<ASpawnPoint*, TSharedPtr<SpawnPointInfo>> _spawnPoints;
	TSet<ACockroach*> _spawnedEnemies;

	FTimerHandle _startRoundTimerHandle;
	FTimerDelegate _startRoundTimerDelegate;

	float walkSpeed = (maxSpeed - minSpeed) / 2.0f;

	AProjectCastleDoctrinCharacter* _playerRef;

	TSet<AAmmoItem*> _spawnedAmmo;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	UStaticMesh* ammoDrop;
};
