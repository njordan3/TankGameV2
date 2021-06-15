// Fill out your copyright notice in the Description page of Project Settings.

#include "TankController.h"
#include "Tank.h"
#include "TankShell.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ProjectileClass = ATankShell::StaticClass();

	bIsFiring = false;

	bShowMouseCursor = true;
};

void ATankController::BeginPlay() {
	Super::BeginPlay();

	Owner = Cast<ATank>(GetPawn());
}

// Called to bind functionality to input
void ATankController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);
	
	InputComponent->BindAxis("MoveForward", this, &ATankController::MoveForward);
	InputComponent->BindAxis("RotateBody", this, &ATankController::RotateBody);

	InputComponent->BindAction("FireShell", IE_Pressed, this, &ATankController::StartShellFire);

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Tank Controls Initialized"));
}

void ATankController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	float GroundedSpringRatio = Owner->GetRatioOfGroundedSprings();
	FVector ForwardVector = Owner->GetActorForwardVector();
	FVector RightVector = Owner->GetActorRightVector();
	FVector UpVector = Owner->GetActorUpVector();

	{	//Body Rotation
		if (RotationInput != 0.0f && GroundedSpringRatio > 0.0f)
		{	
			FVector Torque = UpVector * RotationInput * Owner->GetTurnTorque() * GroundedSpringRatio;
			Owner->BodyStaticMesh->AddTorqueInDegrees(Torque);
		}
	}

	{	//Forward Movement
		if (ForwardInput != 0.0f && GroundedSpringRatio > 0.0f)
		{
			//Project the Tank's forward vector onto the plane of the Suspension's average raycasting impact normal
			FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(ForwardVector, Owner->GetDirectedSuspensionNormal());
			//Calculate the force and reduce it by the ratio of however many Springs are grounded
			FVector Force = Direction * ForwardInput * Owner->GetForwardForce() * GroundedSpringRatio;
			//Calculate location to add the force. The offsets add a "bounciness" 
			//to accelerating and braking by moving the force location slightly away from the local center
			FVector ForceOffset = Owner->GetForwardForceOffset();
			FVector Location = Owner->GetActorLocation() +
				ForceOffset.X * ForwardVector +
				ForceOffset.Y * RightVector +
				ForceOffset.Z * UpVector;

			Owner->BodyStaticMesh->AddForceAtLocation(Force, Location);
		}
	}

	{	//Counteract Drifting
		float DriftAmount = FVector::DotProduct(Owner->GetVelocity(), RightVector);
		FVector AntiDriftForce = RightVector * -DriftAmount * Owner->GetDriftCoefficient();
		Owner->BodyStaticMesh->AddImpulse(AntiDriftForce);
	}

	{	//Get the angle between mouse position and the current forward vector and rotate the Gun mesh
		float MouseX, MouseY;
		GetMousePosition(MouseX, MouseY);
		//Player is centered in the viewport, so just grab the middle viewport coordinates for the Tank position
		FVector2D PawnPosition = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY()) / 2;

		//Get vector from the center of the viewport to the mouse position
		FVector2D MouseVector = FVector2D(PawnPosition.X - MouseX, PawnPosition.Y - MouseY);

		//2D vector math to calculate Yaw's angle
		float Yaw = FMath::RadiansToDegrees(FMath::Atan2(MouseVector.Y, MouseVector.X)) - 90;	//-90 to move the Yaw 90 degrees counter clockwise

		Owner->SetRelativeGunRotation(FRotator(0.0f, Yaw, 0.0f));
	}
}

void ATankController::MoveForward(float Value)
{
	ForwardInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void ATankController::RotateBody(float Value)
{
	RotationInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);
}

void ATankController::StartShellFire()
{
	if (!bIsFiring)
	{
		bIsFiring = true;
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(FiringTimer, this, &ATankController::StopShellFire, Owner->GetFireRate(), false);
		HandleShellFire();
	}
}

void ATankController::StopShellFire()
{
	bIsFiring = false;
}

void ATankController::HandleShellFire_Implementation()
{
	FRotator MuzzleRotation = Owner->GunStaticMesh->GetSocketRotation(TEXT("GunMuzzle"));
	FVector MuzzleLocation = Owner->GunStaticMesh->GetSocketLocation(TEXT("GunMuzzle"));

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Spawn the projectile at the muzzle.
	ATankShell* Projectile = World->SpawnActor<ATankShell>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
	
	if (Projectile)
	{
		// Set the projectile's initial trajectory.
		FVector Direction = MuzzleRotation.Vector();
		Projectile->FireInDirection(Direction);
	}
	
}