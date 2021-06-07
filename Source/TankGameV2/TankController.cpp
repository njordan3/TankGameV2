// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SuspensionLength = 30.0f;
	SpringConstant = 1500.0f;
	DampeningCoefficient = 1.0f;

	BodyRotationScale = 0.3f;
	BodySpeed = 1000.0f;

	FrontRightSpringLength = 0.0f;
	FrontLeftSpringLength = 0.0f;
	BackRightSpringLength = 0.0f;
	BackLeftSpringLength = 0.0f;

	ProjectileClass = ATankShell::StaticClass();

	bShowMouseCursor = true;
};

// Called to bind functionality to input
void ATankController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ATankController::MoveForward);
	InputComponent->BindAxis("RotateBody", this, &ATankController::RotateBody);

	InputComponent->BindAction("FireShell", IE_Pressed, this, &ATankController::FireShell);
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
		/*
		if (!MovementInput.IsZero())
		{
			MovementInput = MovementInput.GetSafeNormal() * BodySpeed;
			FVector NewLocation = GetPawn()->GetActorLocation();
			NewLocation += GetPawn()->GetActorForwardVector() * MovementInput.X * DeltaTime;
			NewLocation += GetPawn()->GetActorRightVector() * MovementInput.Y * DeltaTime;
			GetPawn()->SetActorLocation(NewLocation);
		}
		*/
		
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

		//2D vector math to calculate angle
		float Dot = ForwardVector.X * MouseVector.X + ForwardVector.Y * MouseVector.Y;	//Dot Product, proportional to cosine, or X
		float Det = ForwardVector.X * MouseVector.Y - ForwardVector.Y * MouseVector.X;	//Determinate, proportional to sine, or Y
		float Yaw = FMath::RadiansToDegrees(FMath::Atan2(Det, Dot)) - 90;	//Atan2(sin, cos), or Atan2(Y, X). -90 to move the Yaw 90 degrees counter clockwise

		Cast<ATank>(GetPawn())->SetRelativeGunRotation(FRotator(0.0f, Yaw, 0.0f));
	}

	{	//Ray cast the four corners downward
		UWorld* World = GetWorld();

		{	//Front Right
			FVector FrontRightLocation = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("FrontRight"));
			FVector FrontRightEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontRight")).Vector() * SuspensionLength) + FrontRightLocation;
			
			float PrevSpringLength = FrontRightSpringLength;
			FrontRightSpringLength = RayCastCurrentLengthOfSpring(World, FrontRightLocation, FrontRightEnd);
			
			float SpringVelocity = (FrontRightSpringLength - PrevSpringLength) / DeltaTime;
			
			FVector FrontRightForce = CalculateDampenedSpringForce(Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontRight")).Vector(), FrontRightSpringLength, SpringVelocity);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), FrontRightSpringLength));
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FrontRightForce.ToString());
			Cast<ATank>(GetPawn())->BodyStaticMesh->AddForceAtLocation(FrontRightForce, FrontRightLocation, TEXT("None"));
		}

		{	//Front Left
			FVector FrontLeftLocation = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("FrontLeft"));
			FVector FrontLeftEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontLeft")).Vector() * SuspensionLength) + FrontLeftLocation;

			float PrevSpringLength = FrontLeftSpringLength;
			FrontLeftSpringLength = RayCastCurrentLengthOfSpring(World, FrontLeftLocation, FrontLeftEnd);

			float SpringVelocity = (FrontLeftSpringLength - PrevSpringLength) / DeltaTime;

			FVector FrontLeftForce = CalculateDampenedSpringForce(Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontLeft")).Vector(), FrontLeftSpringLength, SpringVelocity);

			Cast<ATank>(GetPawn())->BodyStaticMesh->AddForceAtLocation(FrontLeftForce, FrontLeftLocation, TEXT("None"));
		}


		{	//Back Right
			FVector BackRightLocation = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("BackRight"));
			FVector BackRightEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackRight")).Vector() * SuspensionLength) + BackRightLocation;

			float PrevSpringLength = BackRightSpringLength;
			BackRightSpringLength = RayCastCurrentLengthOfSpring(World, BackRightLocation, BackRightEnd);

			float SpringVelocity = (BackRightSpringLength - PrevSpringLength) / DeltaTime;

			FVector BackRightForce = CalculateDampenedSpringForce(Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackRight")).Vector(), BackRightSpringLength, SpringVelocity);

			Cast<ATank>(GetPawn())->BodyStaticMesh->AddForceAtLocation(BackRightForce, BackRightLocation, TEXT("None"));
		}

		{	//BackLeft
			FVector BackLeftLocation = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("BackLeft"));
			FVector BackLeftEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackLeft")).Vector() * SuspensionLength) + BackLeftLocation;

			float PrevSpringLength = BackLeftSpringLength;
			BackLeftSpringLength = RayCastCurrentLengthOfSpring(World, BackLeftLocation, BackLeftEnd);

			float SpringVelocity = (BackLeftSpringLength - PrevSpringLength) / DeltaTime;

			FVector BackLeftForce = CalculateDampenedSpringForce(Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackLeft")).Vector(), BackLeftSpringLength, SpringVelocity);

			Cast<ATank>(GetPawn())->BodyStaticMesh->AddForceAtLocation(BackLeftForce, BackLeftLocation, TEXT("None"));
		}
	}
}

float ATankController::RayCastCurrentLengthOfSpring(UWorld* World, FVector Start, FVector End)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	DrawDebugLine(World, Start, End, FColor::Green, false, 0.01f, 0, 1);
	return (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams) ? HitResult.Distance : SuspensionLength);
}

FVector ATankController::CalculateDampenedSpringForce(FVector ForwardVector, float SpringLength, float SpringVelocity)
{
	return -ForwardVector * ((-DampeningCoefficient * SpringVelocity) - (SpringConstant * (SpringLength - SuspensionLength)));
}

void ATankController::MoveForward(float Value)
{
	//MovementInput.X = FMath::Clamp<float>(Value, -1.0f, 1.0f);
	FVector ForwardVector = Cast<ATank>(GetPawn())->GetActorForwardVector();

	Cast<ATank>(GetPawn())->BodyStaticMesh->AddForce(ForwardVector * Value * BodySpeed);
}

void ATankController::RotateBody(float Value)
{
	BodyRotationInput.Yaw = FMath::Clamp<float>(Value, -1.0f, 1.0f);
	//BodyRotationInput.Clamp();
}

void ATankController::FireShell()
{
	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		FRotator MuzzleRotation = Cast<ATank>(GetPawn())->GunStaticMesh->GetSocketRotation(TEXT("GunMuzzle"));
		FVector MuzzleLocation = Cast<ATank>(GetPawn())->GunStaticMesh->GetSocketLocation(TEXT("GunMuzzle"));

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// Spawn the projectile at the muzzle.
			ATankShell* Projectile = World->SpawnActor<ATankShell>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = MuzzleRotation.Vector();
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
}