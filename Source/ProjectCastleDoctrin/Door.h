// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "InteractableParent.h"
#include "SpawnPoint.h"

#include "Door.generated.h"

UCLASS()
class PROJECTCASTLEDOCTRIN_API ADoor : public AInteractableParent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	bool usable() override;

	void interact() override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TSet<ASpawnPoint*> _spawnsToActivate;

};
