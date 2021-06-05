// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tank.generated.h"

UCLASS()
class TANKGAMEV2_API ATank : public ACharacter
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

public:	
	
	void SetRelativeGunRotation(FRotator Rotation);

	FRotator GetRelativeGunRotation();

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* SpringArmComp;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* BodyStaticMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* GunStaticMesh;
};