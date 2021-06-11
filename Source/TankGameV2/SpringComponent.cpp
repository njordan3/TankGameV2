// Fill out your copyright notice in the Description page of Project Settings.

#include "SpringComponent.h"
#include "DrawDebugHelpers.h"
#include "Containers/Array.h"

// Sets default values for this component's properties
USpringComponent::USpringComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
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
	
	//Owner = Cast<UPrimitiveComponent>(GetOwner()->GetComponentsByClass(UPrimitiveComponent::StaticClass())[0]);

	TArray<UPrimitiveComponent*> OwnerComponents;
	GetOwner()->GetComponents<UPrimitiveComponent>(OwnerComponents, false);
	Owner = OwnerComponents[0];
	Owner->SetSimulatePhysics(true);
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

		DrawDebugLine(World, Start, End, FColor::Green, false, 0.01f, 0, 1);

		Grounded = World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
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