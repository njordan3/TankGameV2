// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tank.generated.h"

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

	UFUNCTION()
		void OnRep_CurrentHealth();

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* SpringArmComp;

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

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
		float FireRate;

public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
		FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
		void SetCurrentHealth(float HealthValue);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	FORCEINLINE void SetRelativeGunRotation(FRotator Rotation) { GunStaticMesh->SetWorldRotation(Rotation); }

	FORCEINLINE FRotator GetRelativeGunRotation() const { return GunStaticMesh->GetRelativeRotation(); }

	FORCEINLINE float GetForwardForce() const { return ForwardForce; }

	FORCEINLINE FVector GetForwardForceOffset() const { return ForwardForceOffset; }

	FORCEINLINE float GetDriftCoefficient() const { return DriftCoefficient; }

	FORCEINLINE float GetTurnTorque() const { return TurnTorque; }

	FORCEINLINE float GetFireRate() const { return FireRate; }

	FVector GetDirectedSuspensionNormal(float Direction = 0.0f);

	float GetRatioOfGroundedSprings();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BodyStaticMesh;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* GunStaticMesh;

	UPROPERTY(EditAnywhere)
		class USpringComponent* FrontRightSpringComp;

	UPROPERTY(EditAnywhere)
		class USpringComponent* FrontLeftSpringComp;

	UPROPERTY(EditAnywhere)
		class USpringComponent* BackRightSpringComp;

	UPROPERTY(EditAnywhere)
		class USpringComponent* BackLeftSpringComp;
private:
	FVector DefaultCenterOfMass;
};
