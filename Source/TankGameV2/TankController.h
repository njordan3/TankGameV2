// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankController.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API ATankController : public APlayerController
{
	GENERATED_BODY()

public:
    ATankController(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaTime) override;

	// Projectile class to spawn.
	//UPROPERTY(EditDefaultsOnly, Category = "TankShell")
	//	TSubclassOf<class ATankShell> ProjectileClass;

protected:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartShellFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopShellFire();

	//RPC function for spawning projectiles.
	//UFUNCTION(Server, Reliable)
	//	void HandleShellFire();

	bool bIsFiring;
	FTimerHandle FiringTimer;

	//Input variables
	float ForwardInput;
	float RotationInput;

	//Input functions
	void MoveForward(float AxisValue);
	void RotateBody(float AxisValue);
};
