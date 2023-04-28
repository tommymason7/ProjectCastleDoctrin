// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/TargetPoint.h"

#include "InteractableParent.h"

#include "BoxSpawnedWeapon.h"
#include "BaseGun.h"

#include "BuyStation.generated.h"

UCLASS()
class PROJECTCASTLEDOCTRIN_API ABuyStation : public AInteractableParent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuyStation();

	UFUNCTION(BlueprintCallable)
	TSet<TSubclassOf<UBaseGun>> getGunOptions(int numOfOptionsToReturn);

	void interact() override;

	UFUNCTION()
	void weaponTimedOut();

	UFUNCTION()
	bool usable();

	UFUNCTION()
	ABoxSpawnedWeapon* getSpawnedWeapon();

	UFUNCTION()
	void cancelGunTimer();

	UFUNCTION()
	void destroySpawnedWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	float timeUntilWeaponDisappears = 10.0f;

	FTimerHandle spawnedWeaponTimerHandler;
	FTimerDelegate spawnedWeaponTimerDelegate;

	ABoxSpawnedWeapon* spawnedWeapon = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<TSubclassOf<class UBaseGun>> gunOptions;
};