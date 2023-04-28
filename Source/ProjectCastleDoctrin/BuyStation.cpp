// Fill out your copyright notice in the Description page of Project Settings.


#include "BuyStation.h"

// Sets default values
ABuyStation::ABuyStation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
}

// Called when the game starts or when spawned
void ABuyStation::BeginPlay()
{
	Super::BeginPlay();

	spawnedWeaponTimerDelegate.BindUFunction(this, "weaponTimedOut");
	
}

// Called every frame
void ABuyStation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuyStation::interact()
{
	// Spawn Actor
	if (GetWorld())
	{
		// Would be different if it wasn't limited to just spawning one
		//  Would need a data struct mapping the BaseClass, spawnedWeapon actor, timer handle, and timer delegate
		TArray<TSubclassOf<UBaseGun>> gunOpts = getGunOptions(1).Array();

		UE_LOG(LogTemp, Warning, TEXT("buy station interact"));
		for (int i = 0; i < gunOpts.Num(); i++)
		{
			// Get Socket Location for spawn point
			spawnedWeapon = GetWorld()->SpawnActor<ABoxSpawnedWeapon>(ABoxSpawnedWeapon::StaticClass(), mesh->GetSocketLocation("GunSpawnLocation"), GetActorRotation());
			spawnedWeapon->setGunData(gunOpts[i]);

			// Start timer in weapon for it to disappear
			GetWorld()->GetTimerManager().SetTimer(spawnedWeaponTimerHandler, spawnedWeaponTimerDelegate, timeUntilWeaponDisappears, false);
		}
	}
}

TSet<TSubclassOf<UBaseGun>> ABuyStation::getGunOptions(int numOfOptionsToReturn)
{
	TSet<TSubclassOf<UBaseGun>> gunSet;

	if (gunOptions.Num() > 0)
	{
		TArray<TSubclassOf<UBaseGun>> gunOpts = gunOptions.Array();

		for (int i = 0; i < numOfOptionsToReturn; i++)
		{
			if (gunOpts.Num() > 0)
			{
				// Random number between 0 and gunOpts Num - 1
				int index = FMath::RandHelper(gunOpts.Num());
				
				gunSet.Emplace(gunOpts[index]);

				gunOpts.RemoveAt(index);
			}
			else
			{
				break;
			}
		}
	}

	return gunSet;
}

void ABuyStation::weaponTimedOut()
{
	if (spawnedWeapon)
	{
		if (GetWorld())
		{
			GetWorld()->DestroyActor(spawnedWeapon);
			spawnedWeapon = nullptr;
		}
	}
}

bool ABuyStation::usable()
{
	// If the spawned weapon is null that means the station can be rolled again
	return spawnedWeapon == nullptr;
}

ABoxSpawnedWeapon* ABuyStation::getSpawnedWeapon()
{
	return spawnedWeapon;
}

void ABuyStation::cancelGunTimer()
{
	if (spawnedWeapon)
	{
		GetWorld()->GetTimerManager().ClearTimer(spawnedWeaponTimerHandler);
		spawnedWeaponTimerHandler.Invalidate();
	}
}

void ABuyStation::destroySpawnedWeapon()
{
	if (spawnedWeapon && GetWorld())
	{
		GetWorld()->DestroyActor(spawnedWeapon);
		spawnedWeapon = nullptr;
	}
}