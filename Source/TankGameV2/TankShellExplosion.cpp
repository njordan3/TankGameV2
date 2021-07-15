// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShellExplosion.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ATankShellExplosion::ATankShellExplosion()
{
	PrimitiveComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrimitiveComp"));
	SetRootComponent(PrimitiveComp);
}

void ATankShellExplosion::FireImpulse(float Radius, float Impulse, ERadialImpulseFalloff Falloff)
{
	TArray<FHitResult> OutHits;
	FVector MyLocation = GetActorLocation();
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Radius);

	//Get TArray of things hit by the sphere sweep
	GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_PhysicsBody, Sphere);

	for (auto& Hit : OutHits)
	{
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((Hit.GetActor())->GetRootComponent());

		//Apply impulse only if simulating physics
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			MeshComp->AddRadialImpulse(MyLocation, Radius, Impulse, Falloff, true);
		}
	}
		
	Destroy();
}

bool ATankShellExplosion::FireImpulseWithDamage(float BaseDamage, TSubclassOf<class UDamageType> DamageType, AActor* DamageCauser, AController* EventInstigator, AActor* IgnoreActor, TArray<FDamageNumberInfo>& DamageInfo, float OuterRadius, float InnerRadius, float Impulse, ERadialImpulseFalloff Falloff)
{
	bool PlayerDamaged = false;

	TArray<FHitResult> OutHits;
	FVector MyLocation = GetActorLocation();
	FCollisionShape Sphere = FCollisionShape::MakeSphere(OuterRadius);

	//DrawDebugSphere(GetWorld(), MyLocation, OuterRadius, 50, FColor::Purple, true);

	//Get TArray of things hit by the sphere sweep
	GetWorld()->SweepMultiByChannel(OutHits, MyLocation, MyLocation, FQuat::Identity, ECC_Visibility, Sphere);

	for (auto& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();

		if (IgnoreActor != nullptr && HitActor == IgnoreActor)
		{
			continue;
		}

		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((HitActor)->GetRootComponent());

		if (MeshComp)
		{
			FHitResult BlockingHit;

			//DrawDebugLine(GetWorld(), MyLocation, Hit.ImpactPoint, FColor::Orange, true);

			if (GetWorld()->LineTraceSingleByChannel(BlockingHit, MyLocation, Hit.ImpactPoint, ECC_Visibility))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Hit"));
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not Hit"));

				//Apply impulse only if simulating physics
				if (MeshComp->IsSimulatingPhysics())
				{
					MeshComp->AddRadialImpulse(MyLocation, OuterRadius, Impulse, Falloff, true);
				}

				//Deal damage if the HitActor is a Tank
				if (HitActor->GetClass() == ATank::StaticClass())
				{
					//UGameplayStatics::ApplyRadialDamage(GetWorld(), BaseDamage, MyLocation, 300.0f, DamageType, TArray<AActor*>(), DamageCauser, EventInstigator, false, ECC_GameTraceChannel1);

					float Distance = FVector::Distance(MyLocation, Hit.ImpactPoint);

					//Take no damage past 300 distance
					if (Distance <= 300.0f)
					{
						//Decrease the damage as the Tank is farther away
						float Ratio = (OuterRadius - Distance) / OuterRadius;
						float FinalDamage = BaseDamage * Ratio * Ratio;
						if (FinalDamage >= 1.0f)
						{
							UGameplayStatics::ApplyDamage(HitActor, FinalDamage, EventInstigator, DamageCauser, DamageType);

							//Don't count self damage
							if (EventInstigator->GetPawn() != HitActor)
							{
								DamageInfo.Add(FDamageNumberInfo(HitActor, FMath::RoundHalfFromZero(FinalDamage)));
								PlayerDamaged = true;
							}
						}
					}
				}
			}
		}
	}

	Destroy();

	return PlayerDamaged;
}