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

	UpdateOverlaps(true);

	//Initialize Tank Shell Static Mesh ===================================================
	ShellMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShellMeshComp"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShellMesh(TEXT("/Game/TankMeshes/TankShell.TankShell"));
	if (ShellMesh.Succeeded())
	{
		ShellMeshComp->SetStaticMesh(ShellMesh.Object);
	}
	ShellMeshComp->SetGenerateOverlapEvents(true);
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
	BaseDamage = 20.0f;
	InnerRadius = 100.0f;
	OuterRadius = 400.0f;
	Impulse = 500.0f;

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

	InitialLifeSpan = 30.0f;
}

// Called when the game starts or when spawned
void ATankShell::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> OverlappingActors;
		GetOverlappingActors(OverlappingActors);

		int32 ActorsHit = OverlappingActors.Num();

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%d"), ActorsHit));

		if (ActorsHit == 0)
		{
			//Do nothing
		}
		else if (ActorsHit == 1)
		{
			HitPlayer(OverlappingActors[0], GetActorLocation());
		}
		else
		{
			Destroy();
		}

		ShellMeshComp->OnComponentHit.AddDynamic(this, &ATankShell::OnHit);
	}
	else    //Remove client side collision
	{
		ShellMeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		ShellMeshComp->UpdateCollisionProfile();
	}
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
		HitPlayer(Hit.GetActor(), Hit.ImpactPoint);
	}
}

void ATankShell::Destroyed()
{
	Super::Destroyed();

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Location, Rotation, true, EPSCPoolMethod::AutoRelease);

	//Do final radial impulse and damage on the server
	if (GetLocalRole() == ROLE_Authority)
	{
		bool PlayerDamaged = false;
		ATankShellExplosion* Explosion = GetWorld()->SpawnActor<ATankShellExplosion>(ATankShellExplosion::StaticClass(), Location, Rotation);
		if (Explosion)
		{
			PlayerDamaged = Explosion->FireImpulseWithDamage(BaseDamage, DamageType, (AActor*)0, GetInstigatorController(), PlayerHit, OuterRadius, InnerRadius, Impulse, ERadialImpulseFalloff::RIF_Linear);
		}
	}
}

void ATankShell::HitPlayer(AActor* Player, FVector ImpulseLocation)
{
	//If HitActor is a Tank, push the Tank and deal flat damage
	if (Player->GetClass() == ATank::StaticClass())
	{
		Cast<ATank>(Player)->BodyStaticMesh->AddImpulseAtLocation(GetActorForwardVector() * Impulse * 2, ImpulseLocation);
		UGameplayStatics::ApplyDamage(Player, BaseDamage, GetInstigatorController(), GetInstigator(), DamageType);
		PlayerHit = Player;
	}

	Destroy();
}