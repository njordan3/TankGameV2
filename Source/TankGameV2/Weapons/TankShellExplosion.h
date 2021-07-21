// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Tank/Tank.h"
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

	void FireImpulse(float Radius = 400.f, float ImpulseForce = 500.f, ERadialImpulseFalloff Falloff = ERadialImpulseFalloff::RIF_Linear);

	//Returns true if a player is damaged
	bool FireImpulseWithDamage(float Damage, TSubclassOf<class UDamageType> DamageType, AActor* DamageCauser, AController* EventInstigator, AActor* IgnoreActor, TArray<FDamageNumberInfo>& DamageInfo, float OuterRadius = 400.f, float InnerRadius = 100.f, float ImpulseForce = 500.f, ERadialImpulseFalloff Falloff = ERadialImpulseFalloff::RIF_Linear);

protected:
	UStaticMeshComponent* PrimitiveComp;
};
