// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankController.generated.h"

USTRUCT()
struct FMovementInput
{
	GENERATED_BODY()

	UPROPERTY()
		float BodyRotationInput;
	UPROPERTY()
		float GunRotationYaw;
	UPROPERTY()
		float ForwardInput;
	UPROPERTY()
		bool IsHandBraked;

	FMovementInput()
	{
		BodyRotationInput = 0.0f;
		GunRotationYaw = 0.0f;
		ForwardInput = 0.0f;
		IsHandBraked = false;
	}
};

USTRUCT()
struct FDamageNumberInfo
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* Actor;

	UPROPERTY()
		int32 Damage;

	FDamageNumberInfo()
	{
		Actor = nullptr;
		Damage = 0;
	}

	FDamageNumberInfo(AActor* Actr, int32 Dmg)
	{
		Actor = Actr;
		Damage = Dmg;
	}

	FString ToString() const
	{
		return FString::FromInt(Damage);
	}
};

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

	UFUNCTION(Client, Reliable)
		void PlayDamageNumbers(const TArray<FDamageNumberInfo>& DamageInfo);

protected:
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

	int64 TimeServerTimeRequestWasPlaced;
	int64 TimeOffsetFromServer;
	bool TimeOffsetIsValid;

	//Input functions
	void MoveForward(float AxisValue);
	void RotateBody(float AxisValue);
	void ActivateHandBrake();
	void DeactivateHandBrake();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartShellFire();

private:
	//Struct sent to server for movement
	FMovementInput Input;
};