// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableParent.h"

// Sets default values
AInteractableParent::AInteractableParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableParent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
int AInteractableParent::getCost()
{
	return cost;
}

bool AInteractableParent::usable()
{
	return true;
}

void AInteractableParent::interact()
{

}

