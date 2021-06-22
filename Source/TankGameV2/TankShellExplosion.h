// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TankShellExplosion.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API ATankShellExplosion : public AActor
{
	GENERATED_BODY()
	
public:
	ATankShellExplosion();

	void FireImpulse(float Radius = 400.f, float ImpulseForce = 500.f);

protected:
	UStaticMeshComponent* PrimitiveComp;
};
