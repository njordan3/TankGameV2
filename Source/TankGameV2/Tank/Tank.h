// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "../Setup/TankController.h"
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
		void MoveForward(float ForwardInput, bool IsHandBraked, float GroundedSpringRatio);

	UFUNCTION()
		void RotateBody(float RotationInput, float GroundedSringRatio);

	UFUNCTION()
		void UseHandBrake(bool IsHandBraked, float GroundedSpringRatio);

	UFUNCTION()
		void PlayDamageNumber(int32 Damage);

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

	FORCEINLINE float GetForwardForce() const { return ForwardForce; }

	FORCEINLINE FVector GetForwardForceOffset() const { return ForwardForceOffset; }

	FORCEINLINE float GetDriftCoefficient() const { return DriftCoefficient; }

	FORCEINLINE float GetTurnTorque() const { return TurnTorque; }

	FVector GetDirectedSuspensionNormal(float Direction = 0.0f);

	float GetGroundedSpringRatio();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BodyStaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		class UTankGun* Gun;

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

	//Applies a force on the current suspension normal at the offset of the center of mass
	void ApplyForceOnSuspensionNormal(float Coeff);

	AController* LastDamageInstigator;
	AActor* LastDamageCauser;

	FTimerHandle TankDestroyTimer;
	float DestroyTime;
	void CallDestroy();
};
