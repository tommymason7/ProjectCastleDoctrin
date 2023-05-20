// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Cockroach.h"

#include "BaseGun.generated.h"

/**
 * 
 */

UENUM()
enum class EFireType : uint8 
{
	SEMI,
	AUTO
};

UCLASS(Blueprintable)
class PROJECTCASTLEDOCTRIN_API UBaseGun : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USkeletalMesh* gunMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString gunName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool persistentLineTrace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float timeInBetweenShots = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float reloadTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int projectileDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double lineTraceMultiplier = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* gunShotSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* emptyGunShotSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundBase* gunShotHitSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int magSize = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ammoInMag = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int ammoInReserve = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int maxAmmoInReserve = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ShootAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Particles, meta = (AllowPrivateAccess = "true"))
	FName muzzleFlashSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = "true"))
	class UClass* animBP;

	float TimeLastShot = 0.0f;

public:

	UBaseGun();

	// Returns if the shot hit or not
	UFUNCTION(BlueprintCallable)
	bool Shoot(USceneComponent* gun, FVector fwdVector, AActor* actor);

	USkeletalMesh* getGunMesh();

	UFUNCTION()
	void reload();

	UFUNCTION()
	void ammoReceived(int amntOfAmmo);

	UFUNCTION()
	float getTimeInBetweenShots();

	UFUNCTION()
	class UInputAction* getShootAction();

	UFUNCTION()
	FString getName();

	UFUNCTION()
	float getReloadTime();

	UFUNCTION()
	int getMagSize();

	UFUNCTION()
	int getAmmoInMag();

	UFUNCTION()
	int getAmmoInReserve();

	UFUNCTION(BlueprintCallable)
	float getTimeLastShot();

	UFUNCTION()
	class UClass* getAnimBP();

	UFUNCTION()
	int getMaxAmmoSize();
};
