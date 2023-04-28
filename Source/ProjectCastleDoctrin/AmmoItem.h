// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GenericPlatform/GenericPlatformMisc.h"
#include "Components/SphereComponent.h"


#include "AmmoItem.generated.h"

DECLARE_DELEGATE_RetVal(bool, FAmmoChecker)

UCLASS()
class PROJECTCASTLEDOCTRIN_API AAmmoItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmoItem();

	UFUNCTION()
	int getAmntOfAmmo();

	UFUNCTION()
	void setMesh(UStaticMesh* newMesh);

	UFUNCTION()
	void startLifetimeTimer();

	void setAmmoCheckCB(TFunction<bool()> func);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void lifetimeOver();

	UStaticMeshComponent* meshComponent;

	USphereComponent* collision;

	int amntOfAmmo = 0;

	int minAmmo = 10;
	int maxAmmo = 50;

	FTimerHandle _lifeTimeTimer;
	FTimerDelegate _lifeTimeDelegate;

	float _lifeTimeAmnt = 0.0f;
	float _lifeTimeMaxAmnt = 60.0f; // Seconds
	float _lifeTimeMinAmnt = 15.0f; // Seconds

	FAmmoChecker _playerAmmoCheck = nullptr;
};
