// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "TankGun.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API UTankGun : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	UTankGun();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		virtual void SetGunRotation(float Yaw);

	UFUNCTION()
		virtual void FireShell();

	//Returns true if there are no overlapping Actors or if the only overlapping Actors are other Tanks or are Tank Shells
	UFUNCTION()
		bool GunHasValidOverlapping();

	UFUNCTION()
		void SetFireRate(const float NewFireRate);

	UFUNCTION(Server, Reliable)
		void ServerHandleShellFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartShellFire();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopShellFire();

	UFUNCTION(BlueprintPure, Category = "Gameplay")
		FORCEINLINE float GetReloadPercentage() { return ReloadPercentage; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
		FText GetReloadText();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		UCurveFloat* ReloadCurve;

	UFUNCTION()
		void UpdateReloadPercentage();

	UFUNCTION()
		void BeginReloadHUDAnimation();

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "TankShell")
		TSubclassOf<class ATankShell> ProjectileClass;

	//Actual FireRate per second is 1/FireRate
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	bool bCanFire;
	FTimerHandle ReloadTimer;
	class UTimelineComponent* ReloadTimeline;
	FKeyHandle CurrentReloadCurvePoint;
	float CurveFloatValue;
	float ReloadPercentage;
	float PrevReloadPercentage;
	float ReloadAnimationStep;
};
