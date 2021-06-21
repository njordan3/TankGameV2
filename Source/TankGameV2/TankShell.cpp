// Fill out your copyright notice in the Description page of Project Settings.


#include "TankShell.h"
#include "Tank.h"
#include "TankShellExplosion.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicsEngine/RadialForceComponent.h"

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
	//ShellMovementComp->SetUpdatedComponent(ShellMeshComp);
	ShellMovementComp->InitialSpeed = 1500.0f;
	ShellMovementComp->MaxSpeed = 1500.0f;
	ShellMovementComp->bRotationFollowsVelocity = true;
	//ShellMovementComp->bShouldBounce = true;
	//ShellMovementComp->Bounciness = 0.3f;
	ShellMovementComp->ProjectileGravityScale = 0.1f;

	//Initialize Tank Shell Damage Type ===================================================
	DamageType = UDamageType::StaticClass();
	Damage = 20.0f;
	MinimumDamage = 10.0f;
	DamageInnerRadius = 100.0f;
	DamageOuterRadius = 200.0f;
	DamageFalloff = ERadialImpulseFalloff::RIF_Linear;

	//Initialize Tank Shell Explosion Effect ==============================================
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}

	//Initialize Tank Shell Explosion Force ===============================================
	ExplosionForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("ExplosionEffect"));
	ExplosionForce->SetupAttachment(RootComponent);
	//ExplosionForce->AddShellMeshChannelToAffect(ECC_Visibility);
	//ExplosionForce->AddObjectTypeToAffect((EObjectTypeQuery)ECC_WorldDynamic);
	//ExplosionForce->AddObjectTypeToAffect((EObjectTypeQuery)ECC_PhysicsBody);
	ExplosionForce->Radius = DamageOuterRadius;
	ExplosionForce->ForceStrength = 100000.0f;
	ExplosionForce->ImpulseStrength = 100000.0f;

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
		//if (OtherComponent->IsSimulatingPhysics())
		//{
			//OtherComponent->AddImpulseAtLocation(ShellMovementComp->Velocity * 10.0f, Hit.ImpactPoint);
		//	OtherComponent->AddImpulse(ShellMovementComp->Velocity * 10.0f);
		//}

		//SetRootComponent(ExplosionForce);
		//ExplosionForce->SetWorldLocation(Hit.ImpactPoint);

		//ShellMeshComp->DestroyComponent();

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, GetActorLocation().ToString());
		//ExplosionForce->FireImpulse();

		FVector Location = GetActorLocation();
		FRotator Rotation = GetActorRotation();
		ATankShellExplosion* Explosion = GetWorld()->SpawnActor<ATankShellExplosion>(ATankShellExplosion::StaticClass(), Location, Rotation);

		//UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, GetInstigatorController(), this, DamageType);
		//UGameplayStatics::ApplyRadialDamageWithFalloff(this, Damage, MinimumDamage, Hit.ImpactPoint, 
		//	DamageInnerRadius, DamageOuterRadius, DamageFalloff, DamageType, TArray<AActor*>(), this, GetInstigatorController(), ECC_Visibility);
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Hit.ImpactPoint, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

	Destroy();
}