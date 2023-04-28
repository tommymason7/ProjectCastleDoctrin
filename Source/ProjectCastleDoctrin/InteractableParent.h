// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"

#include "InteractableParent.generated.h"

UCLASS()
class PROJECTCASTLEDOCTRIN_API AInteractableParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableParent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void interact();

	UFUNCTION()
	int getCost();

	virtual bool usable();

// Properties
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int cost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	UStaticMeshComponent* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced)
	USphereComponent* collision;

};
