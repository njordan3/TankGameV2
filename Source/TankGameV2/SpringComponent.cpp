// Fill out your copyright notice in the Description page of Project Settings.

#include "SpringComponent.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"

// Sets default values for this component's properties
USpringComponent::USpringComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PreviousSuspensionLengthDelta = 0.0f;
	PreviousCompressionRatio = 0.0f;
	PreviousImpactPoint = FVector::ZeroVector;
	PreviousImpactNormal = FVector::ZeroVector;

	Grounded = false;
}


// Called when the game starts
void USpringComponent::BeginPlay()
{
	Super::BeginPlay();

	TArray<UPrimitiveComponent*> OwnerComponents;
	GetOwner()->GetComponents<UPrimitiveComponent>(OwnerComponents, false);
	Owner = OwnerComponents[0];
}

// Called every frame
void USpringComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsValid(Owner))
	{
		UWorld* World = GetWorld();

		FVector Start = GetComponentLocation();
		FVector End = (GetComponentRotation().Vector() * SuspensionLength) + Start;

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(GetOwner());	//Ignore the Tank's collision boundaries

		Grounded = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);

		//Check if the impact point is a wall, and set Grounded to false so it isn't considered in force calculations
		FRotator ImpactRotation = HitResult.ImpactNormal.Rotation();
		float Tolerance = 0.01F;
		if (FMath::IsNearlyZero(ImpactRotation.Pitch, Tolerance) || FMath::IsNearlyZero(ImpactRotation.Yaw, Tolerance))
		{
			Grounded = false;
			return;
		}

		if (Grounded)
		{
			float SuspensionLengthDelta = HitResult.Distance - SuspensionLength;
			float Velocity = (SuspensionLengthDelta - PreviousSuspensionLengthDelta) / DeltaTime;

			FVector Force = -GetComponentRotation().Vector() * ((Velocity * -DampingCoefficient) - (SuspensionLengthDelta * SpringCoefficient));

			Owner->AddForceAtLocation(Force, Start);

			PreviousSuspensionLengthDelta = SuspensionLengthDelta;
			PreviousCompressionRatio = HitResult.Distance / SuspensionLength;
			PreviousImpactPoint = HitResult.ImpactPoint;
			PreviousImpactNormal = HitResult.ImpactNormal;
		}
	}
}