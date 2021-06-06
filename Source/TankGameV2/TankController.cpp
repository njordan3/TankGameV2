// Fill out your copyright notice in the Description page of Project Settings.


#include "TankController.h"

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	GroundOffset = 40.0f;
	BodyRotationScale = 0.3f;
	BodySpeed = 300.0f;

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

		//2D vector math to calculate angle
		float Dot = ForwardVector.X * MouseVector.X + ForwardVector.Y * MouseVector.Y;	//Dot Product, proportional to cosine, or X
		float Det = ForwardVector.X * MouseVector.Y - ForwardVector.Y * MouseVector.X;	//Determinate, proportional to sine, or Y
		float Yaw = FMath::RadiansToDegrees(FMath::Atan2(Det, Dot)) - 90;	//Atan2(sin, cos), or Atan2(Y, X). -90 to move the Yaw 90 degrees counter clockwise

		Cast<ATank>(GetPawn())->SetRelativeGunRotation(FRotator(0.0f, Yaw, 0.0f));
	}

	{	//Ray cast the four corners downward
		UWorld* World = GetWorld();

		//Front Right
		FHitResult FrontRightHitResult;
		FVector FrontRightStart = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("FrontRight"));
		FVector FrontRightEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontRight")).Vector() * GroundOffset) + FrontRightStart;
		FCollisionQueryParams FrontRightCollisionParams;

		DrawDebugLine(World, FrontRightStart, FrontRightEnd, FColor::Green, false, 1, 0 , 1);

		World->LineTraceSingleByChannel(FrontRightHitResult, FrontRightStart, FrontRightEnd, ECC_Visibility, FrontRightCollisionParams);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("FrontRightHit: %s"), FrontRightHitResult.bBlockingHit));

		//Front Left
		FHitResult FrontLeftHitResult;
		FVector FrontLeftStart = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("FrontLeft"));
		FVector FrontLeftEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("FrontLeft")).Vector() * GroundOffset) + FrontLeftStart;
		FCollisionQueryParams FrontLeftCollisionParams;

		DrawDebugLine(World, FrontLeftStart, FrontLeftEnd, FColor::Green, false, 1, 0, 1);

		World->LineTraceSingleByChannel(FrontLeftHitResult, FrontLeftStart, FrontLeftEnd, ECC_Visibility, FrontLeftCollisionParams);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("FrontLeftHit: %d"), FrontLeftHitResult.bBlockingHit));

		//Back Right
		FHitResult BackRightHitResult;
		FVector BackRightStart = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("BackRight"));
		FVector BackRightEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackRight")).Vector() * GroundOffset) + BackRightStart;
		FCollisionQueryParams BackRightCollisionParams;

		DrawDebugLine(World, BackRightStart, BackRightEnd, FColor::Green, false, 0, 0, 1);

		World->LineTraceSingleByChannel(BackRightHitResult, BackRightStart, BackRightEnd, ECC_Visibility, BackRightCollisionParams);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("BackRightHit: %d"), BackRightHitResult.bBlockingHit));

		//BackLeft
		FHitResult BackLeftHitResult;
		FVector BackLeftStart = Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketLocation(TEXT("BackLeft"));
		FVector BackLeftEnd = (Cast<ATank>(GetPawn())->BodyStaticMesh->GetSocketRotation(TEXT("BackLeft")).Vector() * GroundOffset) + BackLeftStart;
		FCollisionQueryParams BackLeftCollisionParams;

		DrawDebugLine(World, BackLeftStart, BackLeftEnd, FColor::Green, false, 1, 0, 1);

		World->LineTraceSingleByChannel(BackLeftHitResult, BackLeftStart, BackLeftEnd, ECC_Visibility, BackLeftCollisionParams);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("BackLeftHit: %d"), BackLeftHitResult.bBlockingHit));
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