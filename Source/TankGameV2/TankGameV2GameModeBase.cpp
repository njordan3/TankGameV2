// Copyright Epic Games, Inc. All Rights Reserved.


#include "TankGameV2GameModeBase.h"
#include "Tank.h"
#include "TankController.h"

ATankGameV2GameModeBase::ATankGameV2GameModeBase()
{
	PlayerControllerClass = ATankController::StaticClass();
	DefaultPawnClass = ATank::StaticClass();
}

void ATankGameV2GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TankGameV2 Gamemode Initialized!"));

}

void ATankGameV2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}