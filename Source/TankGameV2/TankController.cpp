// Fill out your copyright notice in the Description page of Project Settings.

#include "TankController.h"
#include "Tank.h"
#include "TankShell.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//ProjectileClass = ATankShell::StaticClass();

	bIsFiring = false;

	bShowMouseCursor = true;
};

void ATankController::BeginPlay() {
	Super::BeginPlay();
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

void ATankController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATank* OwnerTank = Cast<ATank>(GetPawn());

	if (OwnerTank != nullptr && IsLocalController())
	{
		//Gun Rotation
		FVector MouseLocation;
		FVector MouseDirection;
		if (DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
		{
			OwnerTank->SetGunRotation(MouseLocation, MouseDirection);
		}
	}
}

void ATankController::MoveForward(float Value)
{
	ForwardInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);

	ATank* OwnerTank = Cast<ATank>(GetPawn());

	if (OwnerTank != nullptr && IsLocalController())
	{
		//Forward Movement
		if (ForwardInput != 0.0f)
		{
			OwnerTank->MoveForward(ForwardInput);
			OwnerTank->CounteractDrifting();
		}
	}
}

void ATankController::RotateBody(float Value)
{
	RotationInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);

	ATank* OwnerTank = Cast<ATank>(GetPawn());

	if (OwnerTank != nullptr && IsLocalController())
	{
		//Body Rotation
		if (RotationInput != 0.0f)
		{
			OwnerTank->RotateBody(RotationInput);
		}
	}
}

void ATankController::StartShellFire()
{
	ATank* OwnerTank = Cast<ATank>(GetPawn());

	if (OwnerTank != nullptr && IsLocalController())
	{
		if (!bIsFiring)
		{
			bIsFiring = true;
			UWorld* World = GetWorld();
			World->GetTimerManager().SetTimer(FiringTimer, this, &ATankController::StopShellFire, OwnerTank->GetFireRate(), false);
			OwnerTank->HandleShellFire();
		}
	}
}

void ATankController::StopShellFire()
{
	bIsFiring = false;
}

/*
void ATankController::HandleShellFire_Implementation()
{
	FRotator MuzzleRotation = OwnerTank->GunStaticMesh->GetSocketRotation(TEXT("GunMuzzle"));
	FVector MuzzleLocation = OwnerTank->GunStaticMesh->GetSocketLocation(TEXT("GunMuzzle"));

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.OwnerTank = this;
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
*/