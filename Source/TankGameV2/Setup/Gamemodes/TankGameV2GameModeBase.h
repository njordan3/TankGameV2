// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TankGameV2GameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API ATankGameV2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	ATankGameV2GameModeBase();

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void Respawn(AController* Controller);

	/** Used for naming bots */
	int NextBotNameID;

	float RespawnTime;

protected:
	TArray<class ATankSpawnPoint*> SpawnPoints;

	class ATankSpawnPoint* GetSpawnPoint();
	
	FTimerHandle RespawnHandle;

	UFUNCTION()
		void Spawn(APlayerController* Controller);

private:
	bool bHasBegun;
};
