// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShell.h"
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
	ShellMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ATankShell::OnShellBeginOverlap);
	ShellMeshComp->SetCollisionProfileName(TEXT("TankShell"));
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

void ATankShell::OnShellBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != GetInstigator())
	{
		HitPlayer(SweepResult.GetActor(), SweepResult.ImpactPoint);
	}
}

void ATankShell::Destroyed()
{
	Super::Destroyed();

	FVector Location = GetActorLocation();
	FRotator Rotation = GetActorRotation();

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Location, Rotation, true, EPSCPoolMethod::AutoRelease);

	//Do final radial impulse and damage on the server
	if (GetInstigator()->GetLocalRole() == ROLE_Authority)
	{
		bool IsPlayerDamaged = false;
		ATankShellExplosion* Explosion = GetWorld()->SpawnActor<ATankShellExplosion>(ATankShellExplosion::StaticClass(), Location, Rotation);
		if (Explosion)
		{
			IsPlayerDamaged = Explosion->FireImpulseWithDamage(BaseDamage, DamageType, (AActor*)0, GetInstigatorController(), PlayerDirectHit, DamageInfo, OuterRadius, InnerRadius, Impulse, ERadialImpulseFalloff::RIF_Linear);
		}

		if (DamageInfo.Num() > 0)
		{
			Cast<ATankController>(GetInstigatorController())->PlayDamageNumbers(DamageInfo);
		}
	}
}

void ATankShell::HitPlayer(AActor* Player, FVector ImpulseLocation)
{
	//If HitActor is a Tank, push the Tank and deal flat damage
	if (GetInstigator()->GetLocalRole() == ROLE_Authority && Player->GetClass() == ATank::StaticClass())
	{
		Cast<ATank>(Player)->BodyStaticMesh->AddImpulseAtLocation(GetActorForwardVector() * Impulse * 2, ImpulseLocation);
		UGameplayStatics::ApplyDamage(Player, BaseDamage, GetInstigatorController(), GetInstigator(), DamageType);
		DamageInfo.Add(FDamageNumberInfo(Player, BaseDamage));
		//Ignore the direct hit player in the radial explosion
		PlayerDirectHit = Player;
	}

	Destroy();
}