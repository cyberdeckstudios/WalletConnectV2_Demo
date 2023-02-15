// Copyright Epic Games, Inc. All Rights Reserved.

#include "WalletConnectV2_DemoGameMode.h"
#include "WalletConnectV2_DemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWalletConnectV2_DemoGameMode::AWalletConnectV2_DemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
