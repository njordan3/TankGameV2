// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"
#include "Tank.h"

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BodyRotationScale = 0.3f;
	BodySpeed = 300.0f;

	bShowMouseCursor = true;
};

// Called to bind functionality to input
void ATankController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ATankController::MoveForward);
	InputComponent->BindAxis("RotateBody", this, &ATankController::RotateBody);

}

void ATankController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	{
		if (!BodyRotationInput.IsZero())
		{	
			SetControlRotation(GetPawn()->GetActorRotation() + (BodyRotationInput * BodyRotationScale));
			//GetPawn()->AddActorWorldRotation(FQuat(BodyRotationInput * BodyRotationScale));
			
		}
	}

	{
		if (!MovementInput.IsZero())
		{
			MovementInput = MovementInput.GetSafeNormal() * BodySpeed;
			FVector NewLocation = GetPawn()->GetActorLocation();
			NewLocation += GetPawn()->GetActorForwardVector() * MovementInput.X * DeltaTime;
			NewLocation += GetPawn()->GetActorRightVector() * MovementInput.Y * DeltaTime;
			GetPawn()->SetActorLocation(NewLocation);
		}
	}

	{	//Get the angle between mouse position and the current forward vector and rotate the Gun mesh
		float MouseX, MouseY;
		GetMousePosition(MouseX, MouseY);

		//Player is centered in the viewport, so just grab the middle viewport coordinates for the Tank position
		FVector2D PawnPosition = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY()) / 2;

		//Get vector from the center of the viewport to the mouse position
		FVector2D MouseVector = FVector2D(PawnPosition.X - MouseX, PawnPosition.Y - MouseY);

		//Get current forward vector of the Tank
		FVector2D ForwardVector = FVector2D(GetPawn()->GetActorForwardVector());
		
		/* Forward Vector is 90 degrees off for whatever reason, so rotate it -90 degrees to compensate
		* Rotation Method Derivation:
		*	- Simplified from the rotation matrix where the angle is -90 resulting in the following matrix:
		*			|  0  1  |
		*			| -1  0  | -OR- FMatrix2x2 RotMatrix = FMatrix2x2(0.0f, 1.0f, -1.0f, 0.0f);
		* 
		*	- Multiplying any 2D vector by this matrix effectively just flips the sign of the vector's X value, so doing this is overkill:
		*			FVector2D ForwardVector = RotMatrix.TransformVector(FVector2D(GetPawn()->GetActorForwardVector()));
		*/
		ForwardVector.X = -ForwardVector.X;

		//2D vector math to calculate angle
		float Dot = ForwardVector.X * MouseVector.X + ForwardVector.Y * MouseVector.Y;	//Dot Product, proportional to cosine, or X
		float Det = ForwardVector.X * MouseVector.Y - ForwardVector.Y * MouseVector.X;	//Determinate, proportional to sine, or Y
		float Angle = FMath::RadiansToDegrees(FMath::Atan2(Det, Dot));	//Atan2(sin, cos), or Atan2(Y, X)

		Cast<ATank>(GetPawn())->SetRelativeGunRotation(FRotator(0.0f, Angle, 0.0f));
	}
}

void ATankController::MoveForward(float Value)
{
	MovementInput.X = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void ATankController::RotateBody(float Value)
{
	BodyRotationInput.Yaw = FMath::Clamp<float>(Value, -1.0f, 1.0f);
	//BodyRotationInput.Clamp();
}