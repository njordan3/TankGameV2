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

	virtual void StartPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Used for naming bots */
	int NextBotNameID;
};
