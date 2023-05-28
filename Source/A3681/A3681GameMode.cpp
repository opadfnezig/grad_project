// Copyright Epic Games, Inc. All Rights Reserved.

#include "A3681GameMode.h"
#include "A3681Character.h"
#include "UObject/ConstructorHelpers.h"

AA3681GameMode::AA3681GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/StarterContent/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
