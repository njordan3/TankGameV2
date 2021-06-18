// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShell.h"
#include "Tank.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATankShell::ATankShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//Initialize Tank Shell Collision Box =================================================
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	CollisionComp->InitBoxExtent(FVector(57.7f, 10.7f, 10.7f));
	CollisionComp->BodyInstance.SetCollisionProfileName(TEXT("TankShell"));
	CollisionComp->OnComponentHit.AddDynamic(this, &ATankShell::OnHit);
	CollisionComp->SetSimulatePhysics(false);
	RootComponent = CollisionComp;

	if (GetLocalRole() == ROLE_Authority)
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &ATankShell::OnHit);
	}

	//Initialize Tank Shell Static Mesh ===================================================
	ShellMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellMesh(TEXT("/Game/TankMeshes/TankShell.TankShell"));
	if (ShellMesh.Succeeded())
	{
		ShellMeshComp->SetStaticMesh(ShellMesh.Object);
	}
	//ShellMeshComp->SetSimulatePhysics(false);
	ShellMeshComp->SetupAttachment(RootComponent);

	/*//Initialize Tank Shell Material ======================================================
	static ConstructorHelpers::FObjectFinder<UMaterial> ShellMaterial(TEXT("[ADD MATERIAL ASSET REFERENCE]"));
	if (ShellMaterial.Succeeded())
	{
		ShellMaterialInstance = UMaterialInstanceDynamic::Create(ShellMaterial.Object, ShellMeshComp);
	}
	ShellMeshComp->SetMaterial(0, ShellMaterialInstance);
	ShellMeshComp->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ShellMeshComp->SetupAttachment(RootComponent);
	*/

	//Initialize Tank Shell Movement Component ============================================
	ShellMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ShellMovementComp"));
	ShellMovementComp->SetUpdatedComponent(CollisionComp);
	ShellMovementComp->InitialSpeed = 1500.0f;
	ShellMovementComp->MaxSpeed = 1500.0f;
	ShellMovementComp->bRotationFollowsVelocity = true;
	ShellMovementComp->bShouldBounce = true;
	ShellMovementComp->Bounciness = 0.3f;
	ShellMovementComp->ProjectileGravityScale = 0.1f;

	//Initialize Tank Shell Damage Type ===================================================
	DamageType = UDamageType::StaticClass();
	Damage = 10.0f;

	//Initialize Tank Shell Explosion Effect ==============================================
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	InitialLifeSpan = 30.0f;
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
	if (OtherComponent->IsSimulatingPhysics())
	{
		//OtherComponent->AddImpulseAtLocation(ShellMovementComp->Velocity * 10.0f, Hit.ImpactPoint);
		OtherComponent->AddImpulse(ShellMovementComp->Velocity * 10.0f);
	}

	if (OtherActor->StaticClass() == ATank::StaticClass())
	{
		UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigator()->Controller, this, DamageType);
	}

	Destroy();
}

void ATankShell::Destroyed()
{
	FVector SpawnLocation = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, SpawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}