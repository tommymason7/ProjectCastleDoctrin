// Fill out your copyright notice in the Description page of Project Settings.


#include "Cockroach.h"

// Sets default values
ACockroach::ACockroach()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACockroach::BeginPlay()
{
	Super::BeginPlay();

	if (GetMesh())
	{
		GetMesh()->OnComponentHit.AddDynamic(this, &ACockroach::OnHit);
	}
	
}

// Called every frame
void ACockroach::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACockroach::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

// Not sure that we're going to be actually shooting projectiles. TODO: either remove if no projectiles or keep for hit detections with projectiles
void ACockroach::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	
}

void ACockroach::applyDamage(double damage)
{
	UE_LOG(LogTemp, Warning, TEXT("Cockroach apply damage"));
	_health -= damage;

	if (_health <= 0)
	{
		// Go through death sequence
		actorKilled();
	}
}

void ACockroach::actorKilled()
{
	OnEnemyDied.Broadcast(this);

	GetWorld()->DestroyActor(this);
}

float ACockroach::damageDealt()
{
	return _damageDealing;
}

void ACockroach::setSpeed(float newSpeed)
{
	GetCharacterMovement()->MaxWalkSpeed = newSpeed;
}

void ACockroach::setMovementPlayRate(float rate)
{
	movementPlayRate = rate;
}

float ACockroach::getMovementRate()
{
	return movementPlayRate;
}

void ACockroach::setHealth(float round)
{
	_health = _health * (1.15 + (round / 50.0));
}