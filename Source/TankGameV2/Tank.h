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

public:

	FORCEINLINE void SetRelativeGunRotation(FRotator Rotation) { GunStaticMesh->SetRelativeRotation(Rotation); }

	FORCEINLINE FRotator GetRelativeGunRotation() { return GunStaticMesh->GetRelativeRotation(); }

	FORCEINLINE float GetForwardForce() { return ForwardForce; }

	FORCEINLINE FVector GetForwardForceOffset() { return ForwardForceOffset; }

	FORCEINLINE float GetDriftCoefficient() { return DriftCoefficient; }

	FORCEINLINE float GetTurnTorque() { return TurnTorque; }

	FVector GetDirectedSuspensionNormal(float Direction = 0.0f);

	float GetRatioOfGroundedSprings();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BodyStaticMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* GunStaticMesh;

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
