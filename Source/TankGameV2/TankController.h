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

	bool IsNetworkTimeValid();

	static int64 GetLocalTime();

	int64 GetNetworkTime();

protected:
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartShellFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopShellFire();

	/** Gets the game state */
	//class ATankGameStateBase* GetGameState();

	/** Sent from a client to the server to get the server's system time */
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerGetServerTime();

	/** Sent from the server to a client to give them the server's system time */
	UFUNCTION(Reliable, Client)
		void ClientGetServerTime(int64 serverTime);

	/** Sent from a client to the server to set the client's player name. We don't use
	any sort of known online subsystem so we do it this way */
	UFUNCTION(Reliable, Server, WithValidation)
		void ServerSetPlayerName(const FString& PlayerName);

	bool bIsFiring;
	FTimerHandle FiringTimer;

	int64 TimeServerTimeRequestWasPlaced;
	int64 TimeOffsetFromServer;
	bool TimeOffsetIsValid;

	//Input variables
	float ForwardInput;
	float RotationInput;

	//Input functions
	void MoveForward(float AxisValue);
	void RotateBody(float AxisValue);

private:
	FVector MouseLocation;
	FVector MouseDirection;
};
