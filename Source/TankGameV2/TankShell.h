// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tank.h"
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

	virtual void Destroyed() override;

	void HitPlayer(AActor* Player, FVector ImpulseLocation);

	UFUNCTION()
		void OnShellBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystem* ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Effects")
		class UParticleSystemComponent* TracerEffect;

	//Force of the explosion
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
		float Impulse;

	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
		TSubclassOf<class UDamageType> DamageType;

	//Max damage dealt by the explosion
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
		float BaseDamage;

	//Max radius for the explosion to deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
		float InnerRadius;

	//Max radius of the explosion
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion")
		float OuterRadius;

	void FireInDirection(FVector& Direction);

private:
	AActor* PlayerDirectHit;

	TArray<FDamageNumberInfo> DamageInfo;
};
