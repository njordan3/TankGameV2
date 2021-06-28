// Copyright Epic Games, Inc. All Rights Reserved.


#include "TankGameV2GameModeBase.h"
#include "Tank.h"
#include "TankController.h"
#include "TankState.h"

ATankGameV2GameModeBase::ATankGameV2GameModeBase()
{
	PlayerControllerClass = ATankController::StaticClass();
	DefaultPawnClass = ATank::StaticClass();
	PlayerStateClass = ATankState::StaticClass();

	NextBotNameID = 1;
}

void ATankGameV2GameModeBase::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TankGameV2 Gamemode Initialized!"));

}

void ATankGameV2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Player Joined"));
}