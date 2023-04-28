// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectCastleDoctrinGameMode.h"
#include "ProjectCastleDoctrinCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectCastleDoctrinGameMode::AProjectCastleDoctrinGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
