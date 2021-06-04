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

	virtual void SetupInputComponent() override;

	virtual void PlayerTick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Movement")
		float BodySpeed;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		float BodyRotationScale;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		float BodyYaw;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		float BodyPitch;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		float BodyRoll;

	//Input variables
	FVector2D MovementInput;
	FRotator BodyRotationInput;

	//Input functions
	void MoveForward(float AxisValue);
	void RotateBody(float AxisValue);
};
