// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Cockroach.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDiedDelegate, ACockroach*, ActorRef);

UCLASS()
class PROJECTCASTLEDOCTRIN_API ACockroach : public ACharacter
{
	GENERATED_BODY()

// Custom functions
public:
	// Sets default values for this pawn's properties
	ACockroach();

	UFUNCTION(BlueprintCallable)
	void applyDamage(double damage);

	float damageDealt();

	void setSpeed(float newSpeed);

	void setMovementPlayRate(float rate);

	UFUNCTION(BlueprintCallable)
	float getMovementRate();

	void setHealth(float round);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

// Virtual functions
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Variables
public:
	FOnEnemyDiedDelegate OnEnemyDied;

private:

	float movementPlayRate = 1.0f; // Treated like a percentage

	float _health = 50.0;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float _damageDealing = 22.5;

private:

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void actorKilled();

};
