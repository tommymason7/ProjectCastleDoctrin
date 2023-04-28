// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnManager.h"

// Sets default values
ASpawnManager::ASpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnManager::BeginPlay()
{
	Super::BeginPlay();

	_playerRef = Cast<AProjectCastleDoctrinCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AProjectCastleDoctrinCharacter::StaticClass()));

	TArray<AActor*> tmpPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),  ASpawnPoint::StaticClass(), tmpPoints);

	for (AActor* entry : tmpPoints)
	{
		_spawnPoints.Add(Cast<ASpawnPoint>(entry), MakeShared<SpawnPointInfo>());

		// Assign restrictions to Play
		// Problem would be that the spawn points would play difficult each play through of the game
	}
	
	// Bind Round Timer
	_startRoundTimerDelegate.BindUFunction(this, "startRound");

	GetWorld()->GetTimerManager().SetTimer(_startRoundTimerHandle, _startRoundTimerDelegate, _initialRoundStartDelay, false);
}

// Called every frame
void ASpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int ASpawnManager::calculateNumberOfEnemiesToSpawn()
{
	// Probably should have some kind of round structure or use the number of enemies that have been killed
	int totalKilled = 0;
	for (auto it : _spawnPoints)
	{
		totalKilled += it.Value->totalNumSpawned;
	}

	return (_baseNumberToSpawn * _round) + (totalKilled / _baseNumberToSpawn);
}

void ASpawnManager::startRound()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Round"));

	_round++;

	int numToSpawn = calculateNumberOfEnemiesToSpawn();

	// Assign each target point a number of enemies to spawn
	for (auto it : _spawnPoints)
	{
		UE_LOG(LogTemp, Warning, TEXT("spawnpoint is active: "), it.Key->getIsActive());
		if (it.Value.Get() && it.Key->getIsActive())
		{
			// Definitely a buggy algorithm
			// Get number of enemies to assign to this spawn point
			if (numToSpawn > 0)
			{
				int spawnCount = FMath::RandRange(1, numToSpawn / 2);
				UE_LOG(LogTemp, Warning, TEXT("numToSpawn: %d, spawncoumt: %d"), numToSpawn, spawnCount);
				if (numToSpawn - spawnCount < 0)
				{
					it.Value.Get()->numToSpawnThisRound = numToSpawn - spawnCount;
					UE_LOG(LogTemp, Warning, TEXT("1 setting numToSpawnThisRound: %d"), it.Value.Get()->numToSpawnThisRound);
				}
				else
				{
					it.Value.Get()->numToSpawnThisRound = spawnCount;
					UE_LOG(LogTemp, Warning, TEXT("2 setting numToSpawnThisRound: %s"), it.Value.Get()->numToSpawnThisRound ? TEXT("true") : TEXT("false"));
				}
				numToSpawn -= spawnCount;
			}
		}
	}

	startSpawningTimers();
}

void ASpawnManager::startSpawningTimers()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Spawning Timers"));
	for (auto it : _spawnPoints)
	{
		UE_LOG(LogTemp, Warning, TEXT("Start Spawning Timers Loop"));
		it.Value->spawnTimerDelegate.BindUFunction(this, "spawnEnemy", it.Key);

		GetWorld()->GetTimerManager().SetTimer(it.Value->spawnTimerHandle, it.Value->spawnTimerDelegate, FMath::RandRange(0.1, 3.0), false);
	}
}

void ASpawnManager::spawnEnemy(ASpawnPoint* tp)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Enemy"));

	// Find spawn location in map
	auto it = _spawnPoints.Find(tp);

	if (it)
	{
		TSharedPtr<SpawnPointInfo> info = it->ToSharedRef();

		if (info)
		{
			UE_LOG(LogTemp, Warning, TEXT("numToSpawnThisRound: %d, numSpawnedThisRound: %d"), info.Get()->numToSpawnThisRound, info.Get()->numSpawnedThisRound);
			// Spawn Enemies because we haven't spawned enough yet
			if (info.Get()->numToSpawnThisRound > info.Get()->numSpawnedThisRound)
			{
				UE_LOG(LogTemp, Warning, TEXT("Should spawn the next line"));
				// Spawn enemy
				ACockroach* enemy = GetWorld()->SpawnActor<ACockroach>(enemyClass.Get(), tp->GetActorTransform());

				if (enemy)
				{
					// Bind to the Death Delegate
					enemy->OnEnemyDied.AddDynamic(this, &ASpawnManager::enemyDied);

					// Set Enemy Health
					enemy->setHealth(_round);

					// Set the speed of the Cockroach
					float formula = minSpeed + (_round * (FMath::Sqrt(maxSpeed)));
					enemy->setSpeed(FMath::RandRange(minSpeed, FMath::Clamp(formula, minSpeed, maxSpeed)));

					enemy->setMovementPlayRate(enemy->GetCharacterMovement()->GetMaxSpeed() / walkSpeed);

					_spawnedEnemies.Add(enemy);

					info.Get()->numSpawnedThisRound++;
					info.Get()->totalNumSpawned++;

					// Reset timer with new time
					GetWorld()->GetTimerManager().SetTimer(info.Get()->spawnTimerHandle, info.Get()->spawnTimerDelegate, FMath::RandRange(0.1, 3.0), false);
				}
			}
			else
			{
				// Do nothing because we can't assume round is over, enemies might still be alive
			}
		}
	}
}

void ASpawnManager::enemyDied(ACockroach* enemy)
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Killed"));

	AAmmoItem* ammo = nullptr;

	if (GetWorld())
	{
		FCollisionQueryParams params;
		params.AddIgnoredActor(enemy);

		FHitResult hit;
		// TODO: Potential bad data back?
		GetWorld()->LineTraceSingleByChannel(hit, enemy->GetActorLocation(), enemy->GetActorLocation() - FVector(0, 0, 10000), ECollisionChannel::ECC_WorldStatic, params);

		if (hit.bBlockingHit)
		{
			ammo = GetWorld()->SpawnActor<AAmmoItem>(AAmmoItem::StaticClass(), hit.ImpactPoint, enemy->GetActorRotation());
			ammo->setMesh(ammoDrop);
			ammo->setAmmoCheckCB([this]() {
				return _playerRef->hasEnoughAmmo();
			});
		}

	}

	if (ammo)
	{
		ammo->startLifetimeTimer();

		_spawnedAmmo.Add(ammo);
	}

	_playerRef->enemyKilled();

	_spawnedEnemies.Remove(enemy);

	// Start in between rounds
	if (_spawnedEnemies.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("All Enemies have been killed"));

		for (auto it : _spawnPoints)
		{
			// Resetting the spawn variables per spot
			it.Value->numSpawnedThisRound = 0;
			it.Value->numToSpawnThisRound = 0;
		}

		// Timer until next round starts
		GetWorld()->GetTimerManager().SetTimer(_startRoundTimerHandle, _startRoundTimerDelegate, _timeInBetweenRounds, false);
	}
	else
	{
		// More enemies need to be killed
	}
}