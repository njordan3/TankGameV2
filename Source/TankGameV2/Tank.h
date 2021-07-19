// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankController.h"
#include "Tank.generated.h"

#define PROXY_STATE_ARRAY_SIZE 20

USTRUCT()
struct FSmoothPhysicsState
{
	GENERATED_BODY()

	UPROPERTY()
		uint64 Timestamp;
	UPROPERTY()
		FVector BodyPos;
	UPROPERTY()
		FVector BodyVel;
	UPROPERTY()
		FRotator BodyRot;
	UPROPERTY()
		FVector GunPos;
	UPROPERTY()
		FRotator GunRot;

	FSmoothPhysicsState()
	{
		Timestamp = 0;
		
		BodyPos = FVector::ZeroVector;
		BodyVel = FVector::ZeroVector;
		BodyRot = FRotator::ZeroRotator;

		GunPos = FVector::ZeroVector;
		GunRot = FRotator::ZeroRotator;
	}
};

UCLASS()
class TANKGAMEV2_API ATank : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATank();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnHealthUpdate();

	UPROPERTY(EditDefaultsOnly, Category = "Health")
		float MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
		float CurrentHealth;

	UPROPERTY()
		float HealthPercentage;

	UFUNCTION()
		void OnRep_CurrentHealth();

	UPROPERTY(ReplicatedUsing = OnRep_ServerPhysicsState)
		FSmoothPhysicsState ServerPhysicsState;

	UFUNCTION()
		void OnRep_ServerPhysicsState();

	//Move velocity to run along the forward vector
	UFUNCTION()
		void RedirectVelocityForward();

	UFUNCTION()
		void CounteractDrifting();

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* CameraSpringArmComp;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* TankGunSpringArmComp;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, Category = "Suspension")
		float SuspensionLength;

	UPROPERTY(EditAnywhere, Category = "Suspension")
		float SpringCoefficient;

	UPROPERTY(EditAnywhere, Category = "Suspension")
		float DampingCoefficient;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float ForwardForce;

	UPROPERTY(EditAnywhere, Category = "Movement")
		FVector ForwardForceOffset;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float TurnTorque;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float AngularDamping;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float LinearDamping;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float DriftCoefficient;

	//Actual FireRate per second is 1/FireRate
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StopShellFire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		UCurveFloat* ReloadCurve;

	UFUNCTION()
		void UpdateReloadPercentage();

	UFUNCTION()
		void BeginReloadHUDAnimation();

	bool bCanFire;
	FTimerHandle ReloadTimer;
	class UTimelineComponent* ReloadTimeline;
	FKeyHandle CurrentReloadCurvePoint;
	float CurveFloatValue;
	float ReloadPercentage;
	float PrevReloadPercentage;
	float ReloadAnimationStep;

	// Projectile class to spawn.
	UPROPERTY(EditDefaultsOnly, Category = "TankShell")
		TSubclassOf<class ATankShell> ProjectileClass;

	//Damage Number Widget
	UPROPERTY(EditAnywhere, Category = "DamageNumber")
		TSubclassOf<class UUserWidget> DamageNumberWidgetClass;

	UPROPERTY(EditAnywhere, Category = "DamageNumber")
		class UUserWidget* DamageNumberWidget;

	UPROPERTY(EditAnywhere, Category = "DamageNumber")
		class UWidgetComponent* DamageNumberWidgetComp;

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable)
		void ServerActivateMovementInput(FMovementInput Input);

	UFUNCTION()
		void ActivateMovementInput(FMovementInput Input);

	UFUNCTION()
		void MoveForward(float ForwardInput);

	UFUNCTION()
		void RotateBody(float RotationInput);

	UFUNCTION()
		void SetGunRotation(float Yaw);

	UFUNCTION()
		void UseHandBrake();

	UFUNCTION()
		void PlayDamageNumber(int32 Damage);

	void FireShell();

	UFUNCTION(Server, Reliable)
		void ServerHandleShellFire();

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float HealthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetHealth() { return HealthPercentage; }

	UFUNCTION(BlueprintPure, Category = "Health")
		FText GetHealthText();

	UFUNCTION(BlueprintPure, Category = "Gameplay")
		FORCEINLINE float GetReloadPercentage() { return ReloadPercentage; }

	UFUNCTION(BlueprintPure, Category = "Gameplay")
		FText GetReloadText();

	FORCEINLINE FRotator GetRelativeGunRotation() const { return GunStaticMesh->GetRelativeRotation(); }

	FORCEINLINE float GetForwardForce() const { return ForwardForce; }

	FORCEINLINE FVector GetForwardForceOffset() const { return ForwardForceOffset; }

	FORCEINLINE float GetDriftCoefficient() const { return DriftCoefficient; }

	FORCEINLINE float GetTurnTorque() const { return TurnTorque; }

	void SetFireRate(const float NewFireRate);

	FVector GetDirectedSuspensionNormal(float Direction = 0.0f);

	float GetGroundedSpringRatio();

	//Returns true if there are no overlapping Actors or if the only overlapping Actors are other Tanks or are Tank Shells
	bool GunHasValidOverlapping();

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
		void StartShellFire();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BodyStaticMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* GunStaticMesh;

	UPROPERTY(EditAnywhere)
		class USpringComponent* FrontRightSpringComp;

	UPROPERTY(EditAnywhere)
		USpringComponent* FrontLeftSpringComp;

	UPROPERTY(EditAnywhere)
		USpringComponent* BackRightSpringComp;

	UPROPERTY(EditAnywhere)
		USpringComponent* BackLeftSpringComp;
private:
	FVector DefaultCenterOfMass;

	FSmoothPhysicsState ProxyStates[PROXY_STATE_ARRAY_SIZE];

	int32 ProxyStateCount;

	void ClientSimulateTankMovement();

	AController* LastDamageInstigator;
	AActor* LastDamageCauser;

	FTimerHandle TankDestroyTimer;
	float DestroyTime;
	void CallDestroy();
};
