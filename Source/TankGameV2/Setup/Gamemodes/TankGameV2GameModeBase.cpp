// Copyright Epic Games, Inc. All Rights Reserved.


#include "TankGameV2GameModeBase.h"
#include "../../Tank/Tank.h"
#include "../TankController.h"
#include "../TankState.h"
#include "../TankHUD.h"
#include "../../LevelScripts/TankSpawnPoint.h"
#include "../../LevelScripts/MyLevelScriptActorBase.h"
#include "EngineUtils.h"

ATankGameV2GameModeBase::ATankGameV2GameModeBase()
{
	PlayerControllerClass = ATankController::StaticClass();
	DefaultPawnClass = ATank::StaticClass();
	PlayerStateClass = ATankState::StaticClass();
	HUDClass = ATankHUD::StaticClass();

	NextBotNameID = 1;
	RespawnTime = 3.0f;

	bHasBegun = false;
}

void ATankGameV2GameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("TankGameV2 Gamemode Initialized!"));

	bHasBegun = true;
}

void ATankGameV2GameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	check(GEngine != nullptr);
	if (!bHasBegun)	//Listen server host seems to join before BeginPlay() is called
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Host Joined"));

		for (TActorIterator<AActor> Actor(GetWorld(), ATankSpawnPoint::StaticClass()); Actor; ++Actor)	//Loop through each Tank Spawn Point
		{
			SpawnPoints.Add(Cast<ATankSpawnPoint>(*Actor));
		}

		check(GEngine != nullptr);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%d Spawn Points Found"), SpawnPoints.Num()));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Client Joined"));
	}

	Spawn(NewPlayer);
}

ATankSpawnPoint* ATankGameV2GameModeBase::GetSpawnPoint()
{
	int32 SpawnPointCount = SpawnPoints.Num();
	for (int32 i = 0; i < SpawnPointCount; i++)
	{
		int32 SpawnIndex = FMath::RandRange(0, SpawnPointCount - 1);
		if (SpawnPoints[SpawnIndex])
			return SpawnPoints[SpawnIndex];
	}
	
	return nullptr;
}

void ATankGameV2GameModeBase::Respawn(AController* Controller)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Here"));

	if (Controller && GetLocalRole() == ROLE_Authority && !GetWorld()->GetTimerManager().IsTimerActive(RespawnHandle))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("There"));

		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUFunction(this, FName("Spawn"), Controller);
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, RespawnDelegate, RespawnTime, false);
	}
}

void ATankGameV2GameModeBase::Spawn(APlayerController* Controller)
{
	if (Controller && GetLocalRole() == ROLE_Authority)
	{
		if (ATankSpawnPoint* SpawnPoint = GetSpawnPoint())
		{
			FVector Location = SpawnPoint->GetActorLocation();
			FRotator Rotation = SpawnPoint->GetActorRotation();
			if (APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation))
			{
				Controller->Possess(NewPawn);
			}
		}
		else
		{
			if (ALevelScriptActor* LevelScript = GetLevel()->GetLevelScriptActor())
			{
				FVector Location = Cast<AMyLevelScriptActorBase>(LevelScript)->GetDefaultSpawn();
				if (APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, FRotator::ZeroRotator))
				{
					Controller->Possess(NewPawn);
				}
			}
		}
	}
}