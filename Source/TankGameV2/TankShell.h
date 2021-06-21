// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	// Function that is called when the projectile hits something.
	UFUNCTION(Category = "Projectile")
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Projectile mesh
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UStaticMeshComponent* ShellMeshComp;

	// Projectile material
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		class UMaterialInstanceDynamic* ShellMaterialInstance;

	// Projectile movement component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		class UProjectileMovementComponent* ShellMovementComp;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystem* ExplosionEffect;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditAnywhere, Category = "Effects")
		class URadialForceComponent* ExplosionForce;

	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		TSubclassOf<class UDamageType> DamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float MinimumDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageInnerRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageOuterRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageFalloff;

	void FireInDirection(FVector& Direction);
};
