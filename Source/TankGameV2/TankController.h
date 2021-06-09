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

	virtual void PlayerTick(float DeltaTime) override;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "TankShell")
		TSubclassOf<class ATankShell> ProjectileClass;

protected:
	// Function that handles firing projectiles.
	UFUNCTION()
		void FireShell();

	//Input variables
	float ForwardInput;
	FRotator BodyRotationInput;

	//Input functions
	void MoveForward(float AxisValue);
	void RotateBody(float AxisValue);

private:
	class ATank* OwnerTank;
};
