// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGun.h"

UBaseGun::UBaseGun()
{
	ammoInMag = magSize;
}

bool UBaseGun::Shoot(USceneComponent* gun, FVector fwdVector, AActor* actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Gun be shooting"));

	// Get the end of the barrel
	FVector loc = gun->GetSocketLocation(muzzleFlashSocketName);

	if (ammoInMag > 0)
	{
		// Play Muzzle Flash
		UNiagaraFunctionLibrary::SpawnSystemAttached(NiagaraSystem, gun, muzzleFlashSocketName, FVector(0.f), FRotator(0.f), EAttachLocation::KeepRelativeOffset, true);

		// Store off time last shot
		TimeLastShot = GetWorld()->GetTimeSeconds();

		ammoInMag--;

		FVector endLoc = loc + fwdVector * lineTraceMultiplier;
		FCollisionQueryParams queryParams;

		// Line trace from the end of the barrel to were the player is pointed
		if (actor->GetWorld())
		{
			if (gunShotSound)
			{
				// Play gun shot sound
				UGameplayStatics::PlaySoundAtLocation(actor->GetWorld(), gunShotSound, loc);
			}

			FHitResult hit;
			actor->GetWorld()->LineTraceSingleByChannel(hit, loc, endLoc, ECollisionChannel::ECC_Pawn, queryParams);

			// Draw Debug
			//DrawDebugLine(actor->GetWorld(), loc, endLoc, hit.bBlockingHit ? FColor::Blue : FColor::Red, persistentLineTrace, 5.0f, 0, 10.0f);

			if (hit.bBlockingHit && IsValid(hit.GetActor()))
			{
				// Cast to Enemy
				ACockroach* roach = Cast<ACockroach>(hit.GetActor());
				if (roach)
				{
					roach->applyDamage(projectileDamage);
					return true;
				}
			}
		}
	}
	else
	{
		// Play Clicking Sound?
		if (emptyGunShotSound)
		{
			UGameplayStatics::PlaySoundAtLocation(actor->GetWorld(), emptyGunShotSound, loc);
		}
	}

	return false;
}

float UBaseGun::getTimeLastShot()
{
	return TimeLastShot;
}

float UBaseGun::getTimeInBetweenShots()
{
	return timeInBetweenShots;
}

USkeletalMesh* UBaseGun::getGunMesh()
{
	return gunMesh;
}

void UBaseGun::reload()
{
	// Determine if we have enough ammo in reserve to just replenish mag
	int ammoDiff = magSize - ammoInMag;

	if (ammoInReserve > ammoDiff)
	{
		ammoInReserve -= ammoDiff;
		ammoInMag = magSize;
	}
	else
	{
		ammoInMag += ammoInReserve;
		ammoInReserve = 0;
	}
}

void UBaseGun::ammoReceived(int amntOfAmmo)
{
	ammoInReserve += amntOfAmmo;
}

class UInputAction* UBaseGun::getShootAction()
{
	return ShootAction;
}

FString UBaseGun::getName()
{
	return gunName;
}

float UBaseGun::getReloadTime()
{
	return reloadTime;
}

int UBaseGun::getMagSize()
{
	return magSize;
}

int UBaseGun::getAmmoInMag()
{
	return ammoInMag;
}

int UBaseGun::getAmmoInReserve()
{
	return ammoInReserve;
}

class UClass* UBaseGun::getAnimBP()
{
	return animBP;
}

int UBaseGun::getMaxAmmoSize()
{
	return maxAmmoInReserve;
}