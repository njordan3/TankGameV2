// Fill out your copyright notice in the Description page of Project Settings.

#include "TankController.h"
#include "Tank.h"
#include "TankShell.h"
#include "TankState.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include <chrono>

using namespace std::chrono;

ATankController::ATankController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	//ProjectileClass = ATankShell::StaticClass();
	TimeOffsetIsValid = true;
	TimeOffsetFromServer = 0;

	bShowMouseCursor = true;
};

void ATankController::BeginPlay() {
	Super::BeginPlay();

	ServerSetPlayerName(TEXT("TEST"));
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
		FVector MouseLocation, MouseDirection;

		if (DeprojectMousePositionToWorld(MouseLocation, MouseDirection))
		{
			//Get the aim vector using mouse world location and direction
			FVector Origin = OwnerTank->GunStaticMesh->GetComponentLocation();
			FVector UpVector = OwnerTank->GetActorUpVector();
			float d = FVector::DotProduct((FVector(0, 0, Origin.Z) - MouseLocation), UpVector)
				/ FVector::DotProduct(MouseDirection, UpVector);
			FVector GroundPoint = MouseLocation + MouseDirection * d;
			FVector FinalAim = GroundPoint - Origin;
			FinalAim.Normalize();

			Input.GunRotationYaw = FinalAim.Rotation().Yaw;
		}

		//Clients only send to server
		if (GetLocalRole() < ROLE_Authority)
		{
			//Locally rotate gun for visually less lag
			OwnerTank->SetGunRotation(Input.GunRotationYaw);
			//Send all inputs to server
			OwnerTank->ServerActivateMovementInput(Input);
		}
		else
		{
			OwnerTank->ActivateMovementInput(Input);
		}
	}
}

void ATankController::MoveForward(float Value)
{
	if (IsLocalController())
	{
		Input.ForwardInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);
	}
}

void ATankController::RotateBody(float Value)
{
	if (IsLocalController())
	{
		Input.BodyRotationInput = FMath::Clamp<float>(Value, -1.0f, 1.0f);
	}
}

void ATankController::StartShellFire()
{
	ATank* OwnerTank = Cast<ATank>(GetPawn());

	if (OwnerTank != nullptr && IsLocalController())
	{
		OwnerTank->StartShellFire();
	}
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
	GetPlayerState<ATankState>()->SetPlayerName(PlayerName);
}