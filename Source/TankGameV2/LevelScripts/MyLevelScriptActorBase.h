// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "../Utilities/TankGameV2Colors.h"
#include "MyLevelScriptActorBase.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API AMyLevelScriptActorBase : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	AMyLevelScriptActorBase();

	FVector GetDefaultSpawn();

	virtual void BeginPlay() override;

protected:
	UFUNCTION(BluePrintCallable, Category = "SetDefaultPlayerSpawn")
		void SetDefaultSpawn(FVector NewDefaultSpawn);

	FVector DefaultSpawn;
};
