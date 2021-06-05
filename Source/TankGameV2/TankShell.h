// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TankShell.generated.h"

UCLASS()
class TANKGAMEV2_API ATankShell : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATankShell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function that is called when the projectile hits something.
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	// Box collision component.
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UBoxComponent* CollisionComp;

	// Projectile mesh
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UStaticMeshComponent* ShellMeshComp;

	// Projectile material
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
		UMaterialInstanceDynamic* ShellMaterialInstance;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ShellMovementComp;

	void FireInDirection(FVector& Direction);
};
