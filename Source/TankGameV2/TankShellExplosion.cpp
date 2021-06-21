// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShellExplosion.h"

ATankShellExplosion::ATankShellExplosion()
{
	PrimitiveComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimitiveComp"));
	SetRootComponent(PrimitiveComp);
}

void ATankShellExplosion::BeginPlay()
{
	Super::BeginPlay();

	// create tarray for hit results
	TArray<FHitResult> OutHits;

	// get actor locations
	FVector MyLocation = GetActorLocation();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, MyLocation.ToString());

	// create a collision sphere
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(500.f);

	// check if something got hit in the sweep
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_PhysicsBody, MyColSphere);

	if (isHit)
	{
		// loop through TArray
		for (auto& Hit : OutHits)
		{
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((Hit.GetActor())->GetRootComponent());

			if (MeshComp)
			{
				// alternivly you can use  ERadialImpulseFalloff::RIF_Linear for the impulse to get linearly weaker as it gets further from origin.
				// set the float radius to 500 and the float strength to 2000.
				MeshComp->AddRadialImpulse(MyLocation, 500.f, 1000.f, ERadialImpulseFalloff::RIF_Constant, true);
			}
		}
	}

	Destroy();
}