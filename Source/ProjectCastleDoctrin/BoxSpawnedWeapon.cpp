// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxSpawnedWeapon.h"

// Sets default values
ABoxSpawnedWeapon::ABoxSpawnedWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mesh = CreateDefaultSubobject<USkeletalMeshComponent>("MeshComponent");
}

// Called when the game starts or when spawned
void ABoxSpawnedWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxSpawnedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoxSpawnedWeapon::setGunData(TSubclassOf<UBaseGun> data)
{
	_gunData = data;

	if (mesh)
	{
		mesh->SetSkeletalMesh(_gunData.GetDefaultObject()->getGunMesh());
	}
}

TSubclassOf<UBaseGun> ABoxSpawnedWeapon::getGunData()
{
	return _gunData;
}