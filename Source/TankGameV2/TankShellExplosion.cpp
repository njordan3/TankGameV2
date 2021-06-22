// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShellExplosion.h"
#include "Tank.h"

ATankShellExplosion::ATankShellExplosion()
{
	PrimitiveComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimitiveComp"));
	SetRootComponent(PrimitiveComp);
}

void ATankShellExplosion::FireImpulse(float Radius, float ImpulseForce)
{
	TArray<FHitResult> OutHits;

	FVector MyLocation = GetActorLocation();

	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	//Get TArray of things hit by the sphere sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_PhysicsBody, Sphere);

	if (isHit)
	{
		for (auto& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((HitActor)->GetRootComponent());

			if (MeshComp)
			{
				MeshComp->AddRadialImpulse(MyLocation, Radius, ImpulseForce, ERadialImpulseFalloff::RIF_Linear, true);
			}
		}
	}
		
	Destroy();
}
