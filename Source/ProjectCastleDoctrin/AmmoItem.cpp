// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoItem.h"

// Sets default values
AAmmoItem::AAmmoItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	meshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	collision = CreateDefaultSubobject<USphereComponent>("CollisionComponent");

	meshComponent->AttachToComponent(collision, rules);
	collision->AttachToComponent(this->RootComponent, rules);

	amntOfAmmo = FMath::RandHelper(maxAmmo) + minAmmo;
	_lifeTimeAmnt = FMath::RandRange(_lifeTimeMinAmnt, _lifeTimeMaxAmnt);
}

// Called when the game starts or when spawned
void AAmmoItem::BeginPlay()
{
	Super::BeginPlay();
	
	// Lifetime Timer
	_lifeTimeDelegate.BindUFunction(this, "lifetimeOver");
}

// Called every frame
void AAmmoItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAmmoItem::lifetimeOver()
{
	// Determine if player has enough ammo
	if (_playerAmmoCheck.IsBound() && _playerAmmoCheck.Execute())
	{
		GetWorld()->GetTimerManager().ClearTimer(_lifeTimeTimer);

		GetWorld()->DestroyActor(this);
	}
}

void AAmmoItem::startLifetimeTimer()
{
	GetWorld()->GetTimerManager().SetTimer(_lifeTimeTimer, _lifeTimeDelegate, _lifeTimeAmnt, true);
}

int AAmmoItem::getAmntOfAmmo()
{
	return amntOfAmmo;
}

void AAmmoItem::setMesh(UStaticMesh* newMesh)
{
	if (newMesh)
	{
		meshComponent->SetStaticMesh(newMesh);
	}
}

void AAmmoItem::setAmmoCheckCB(TFunction<bool()> func)
{
	_playerAmmoCheck.BindLambda(func);
}
