// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShell.h"
#include "Tank.h"
#include "TankShellExplosion.h"
#include "TankState.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ATankShell::ATankShell()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//Initialize Tank Shell Static Mesh ===================================================
	ShellMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellMesh(TEXT("/Game/TankMeshes/TankShell.TankShell"));
	if (ShellMesh.Succeeded())
	{
		ShellMeshComp->SetStaticMesh(ShellMesh.Object);
	}
	if (GetLocalRole() == ROLE_Authority)
	{
		ShellMeshComp->OnComponentHit.AddDynamic(this, &ATankShell::OnHit);
	}
	SetRootComponent(ShellMeshComp);

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
	ShellMovementComp->InitialSpeed = 1500.0f;
	ShellMovementComp->MaxSpeed = 1500.0f;
	ShellMovementComp->bRotationFollowsVelocity = true;
	ShellMovementComp->ProjectileGravityScale = 0.1f;

	//Initialize Tank Shell Damage Type ===================================================
	DamageType = UDamageType::StaticClass();
	Damage = 20.0f;
	MinimumDamage = 10.0f;
	DamageInnerRadius = 100.0f;
	DamageOuterRadius = 400.0f;
	DamageFalloff = ERadialImpulseFalloff::RIF_Linear;

	//Initialize Tank Shell Explosion Effect ==============================================
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	//Initialize Tank Shell Explosion Effect ==============================================
	TracerEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TracerEffect"));
	TracerEffect->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultTracerEffect(TEXT("/Game/StarterContent/Particles/P_Smoke.P_Smoke"));
	if (DefaultTracerEffect.Succeeded())
	{
		TracerEffect->SetTemplate(DefaultTracerEffect.Object);
	}

	DamageImpulse = 500.0f;

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
	if (GetLocalRole() == ROLE_Authority)
	{
		AActor* HitActor = Hit.GetActor();

		//If HitActor is a Tank just push the Tank, else create a Radial Impulse and try to push surrounding Actors
		if (HitActor->GetClass() == ATank::StaticClass())
		{
			Cast<ATank>(HitActor)->BodyStaticMesh->AddImpulseAtLocation(GetActorForwardVector() * DamageImpulse * 2, Hit.ImpactPoint);
		}
		else 
		{
			FVector Location = GetActorLocation();
			FRotator Rotation = GetActorRotation();
			ATankShellExplosion* Explosion = GetWorld()->SpawnActor<ATankShellExplosion>(ATankShellExplosion::StaticClass(), Location, Rotation);
			Explosion->FireImpulse(DamageOuterRadius, DamageImpulse);
		}

		UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, Hit.ImpactPoint, 
			DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageType, TArray<AActor*>(), GetInstigator(), GetInstigatorController(), ECC_Visibility);
	}

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Hit.ImpactPoint, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

	Destroy();
}