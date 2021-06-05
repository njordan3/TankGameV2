// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShell.h"

// Sets default values
ATankShell::ATankShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComp"));
	}

	if (!ShellMeshComp)
	{
		ShellMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMeshComp"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellMesh(TEXT("/Game/TankMeshes/TankShell.TankShell"));
		if (ShellMesh.Succeeded())
		{
			ShellMeshComp->SetStaticMesh(ShellMesh.Object);
		}
		ShellMeshComp->SetSimulatePhysics(false);
		RootComponent = ShellMeshComp;
	}

	/*
	static ConstructorHelpers::FObjectFinder<UMaterial> ShellMaterial(TEXT("[ADD MATERIAL ASSET REFERENCE]"));
	if (ShellMaterial.Succeeded())
	{
		ShellMaterialInstance = UMaterialInstanceDynamic::Create(ShellMaterial.Object, ShellMeshComp);
	}
	ShellMeshComp->SetMaterial(0, ShellMaterialInstance);
	ShellMeshComp->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ShellMeshComp->SetupAttachment(RootComponent);
	*/

	if (!CollisionComp)
	{
		// Use a bpx as a simple collision representation.
		CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
		// Set the box's dimensions
		CollisionComp->InitBoxExtent(FVector(57.7f, 10.7f, 10.7f));
		// Set the Shell's collision profile name to "TankShell".
		CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("TankShell"));
		// Event called when component hits something.
		CollisionComp->OnComponentHit.AddDynamic(this, &ATankShell::OnHit);

		CollisionComp->SetSimulatePhysics(true);
		CollisionComp->SetupAttachment(ShellMeshComp);
	}

	if (!ShellMovementComp)
	{
		ShellMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ShellMovementComp"));
		ShellMovementComp->SetUpdatedComponent(ShellMeshComp);
		ShellMovementComp->InitialSpeed = 1000.0f;
		ShellMovementComp->MaxSpeed = 1000.0f;
		ShellMovementComp->bRotationFollowsVelocity = true;
		ShellMovementComp->bShouldBounce = true;
		ShellMovementComp->Bounciness = 0.3f;
		ShellMovementComp->ProjectileGravityScale = 0.0f;
	}

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void ATankShell::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATankShell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATankShell::FireInDirection(FVector& Direction)
{
	ShellMeshComp->SetWorldRotation(FQuat(Direction.Rotation()));
	ShellMovementComp->Velocity = Direction * ShellMovementComp->InitialSpeed;
}

// Function that is called when the projectile hits something.
void ATankShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ShellMovementComp->Velocity * 100.0f, Hit.ImpactPoint);
	}

	Destroy();
}