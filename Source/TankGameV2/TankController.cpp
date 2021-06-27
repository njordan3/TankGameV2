// Fill out your copyright notice in the Description page of Project Settings.

#include "TankController.h"
#include "Tank.h"
#include "TankShell.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include <chrono>

using namespace std::chrono;

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

int64 ATankController::GetLocalTime()
{
	milliseconds ms = duration_cast<milliseconds>(
		high_resolution_clock::now().time_since_epoch()
		);
	return (int64)ms.count();
}

bool ATankController::IsNetworkTimeValid()
{
	return TimeOffsetIsValid;
}

int64 ATankController::GetNetworkTime()
{
	return GetLocalTime() + TimeOffsetFromServer;
}

bool ATankController::ServerGetServerTime_Validate()
{
	return true;
}

/** Sent from a client to the server to get the server's system time */
void ATankController::ServerGetServerTime_Implementation()
{
	ClientGetServerTime(GetLocalTime());
}

/** Sent from the server to a client to give them the server's system time */
void ATankController::ClientGetServerTime_Implementation(int64 ServerTime)
{
	int64 LocalTime = GetLocalTime();

	// Calculate the server's system time at the moment we actually sent the request for it.
	int64 RoundTripTime = LocalTime - TimeServerTimeRequestWasPlaced;
	ServerTime -= RoundTripTime / 2;

	// Now calculate the difference between the two values
	TimeOffsetFromServer = ServerTime - TimeServerTimeRequestWasPlaced;

	// Now we can safely say that the following is true
	//
	// serverTime = timeServerTimeRequestWasPlaced + timeOffsetFromServer
	//
	// which is another way of saying
	//
	// NetworkTime = LocalTime + timeOffsetFromServer

	TimeOffsetIsValid = true;
}

bool ATankController::ServerSetPlayerName_Validate(const FString& PlayerName)
{
	return true;
}

/** Sent from a client to the server to set the client's player name. We don't use
any sort of known online subsystem so we do it this way */
void ATankController::ServerSetPlayerName_Implementation(const FString& PlayerName)
{
	//GetPlayerState<ATankState>()->SetPlayerName(PlayerName);
}