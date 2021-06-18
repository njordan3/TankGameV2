// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Containers/Array.h"
#include "TankShell.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SpringComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ATank::ATank()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileClass = ATankShell::StaticClass();

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	FireRate = 0.25f;

	SuspensionLength = 60.0f;
	SpringCoefficient = 2000.0f;
	DampingCoefficient = 100.0f;

	ForwardForce = 500.0f;
	ForwardForceOffset = FVector(10.0f, 0.0f, -10.0f);
	TurnTorque = 15000.0f;
	AngularDamping = 1.5f;
	LinearDamping = 0.5f;
	DriftCoefficient = 1.0f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	bAlwaysRelevant = true; 
	SetReplicateMovement(true);
	bNetLoadOnClient = true;
	bReplicates = true;

	//Initialize Tank Body Static Mesh ============================================
	BodyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Game/TankMeshes/TankBody.TankBody"));
	UStaticMesh* BodyAsset = BodyMesh.Object;
	BodyStaticMesh->SetStaticMesh(BodyAsset);
	BodyStaticMesh->SetAngularDamping(AngularDamping);
	BodyStaticMesh->SetLinearDamping(LinearDamping);
	//BodyStaticMesh->SetupAttachment(RootComponent);
	RootComponent = BodyStaticMesh;

	//Change Mass Properties
	DefaultCenterOfMass = BodyStaticMesh->GetCenterOfMass();
	BodyStaticMesh->SetCenterOfMass(FVector(0.0f, 0.0f, -150.0f));
	BodyStaticMesh->SetMassOverrideInKg(NAME_None, 100.0f);
	BodyStaticMesh->GetBodyInstance()->UpdateMassProperties();

	//Initialize Tank Gun Static Mesh =============================================
	GunStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunStaticMesh"));
	GunStaticMesh->SetupAttachment(BodyStaticMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh(TEXT("/Game/TankMeshes/TankGun.TankGun"));
	UStaticMesh* GunAsset = GunMesh.Object;
	GunStaticMesh->SetStaticMesh(GunAsset);
	GunStaticMesh->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));	//Rest Tank Gun in the correct position on the Tank Body'
	GunStaticMesh->SetIsReplicated(true);

	//Initialize Spring Arm Component =============================================
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment(BodyStaticMesh);
	SpringArmComp->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 0.0f),
		FRotator(-89.9f, 0.0f, 0.0f)
	);
	SpringArmComp->TargetArmLength = 3000.0f;
	SpringArmComp->bDoCollisionTest = false;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bInheritYaw = false;

	//Initialize Camera Component =================================================
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Initialize Front Right Spring Component =====================================
	FrontRightSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("FrontRightSpringComponent"));
	FrontRightSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Front Left Spring Component ======================================
	FrontLeftSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("FrontLeftSpringComponent"));
	FrontLeftSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Back Right Spring Component ======================================
	BackRightSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("BackRightSpringComponent"));
	BackRightSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Back Left Spring Component =======================================
	BackLeftSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("BackLeftSpringComponent"));
	BackLeftSpringComp->SetupAttachment(BodyStaticMesh);

	//Take control of the default Player ==========================================
	//AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	// Set Front Right variables
	FrontRightSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("FrontRight")),
		BodyStaticMesh->GetSocketRotation(TEXT("FrontRight"))
	);
	FrontRightSpringComp->SuspensionLength = SuspensionLength;
	FrontRightSpringComp->SpringCoefficient = SpringCoefficient;
	FrontRightSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Front Left variables
	FrontLeftSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("FrontLeft")),
		BodyStaticMesh->GetSocketRotation(TEXT("FrontLeft"))
	);
	FrontLeftSpringComp->SuspensionLength = SuspensionLength;
	FrontLeftSpringComp->SpringCoefficient = SpringCoefficient;
	FrontLeftSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Back Right variables
	BackRightSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("BackRight")),
		BodyStaticMesh->GetSocketRotation(TEXT("BackRight"))
	);
	BackRightSpringComp->SuspensionLength = SuspensionLength;
	BackRightSpringComp->SpringCoefficient = SpringCoefficient;
	BackRightSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Back Left variables
	BackLeftSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("BackLeft")),
		BodyStaticMesh->GetSocketRotation(TEXT("BackLeft"))
	);
	BackLeftSpringComp->SuspensionLength = SuspensionLength;
	BackLeftSpringComp->SpringCoefficient = SpringCoefficient;
	BackLeftSpringComp->DampingCoefficient = DampingCoefficient;

	check(GEngine != nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Tank Initialized %f"), BodyStaticMesh->GetMass()));
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATank::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ATank, CurrentHealth);
}

void ATank::MoveForward_Implementation(float ForwardInput)
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector UpVector = GetActorUpVector();

	//Project the Tank's forward vector onto the plane of the Suspension's average raycasting impact normal
	FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(ForwardVector, GetDirectedSuspensionNormal());
	//Calculate the force and reduce it by the ratio of however many Springs are grounded
	FVector Force = Direction * ForwardInput * ForwardForce * GetGroundedSpringRatio();
	//Calculate location to add the force. The offsets add a "bounciness" 
	//to accelerating and braking by moving the force location slightly away from the local center
	FVector Location = GetActorLocation() +
		ForwardForceOffset.X * ForwardVector +
		ForwardForceOffset.Y * RightVector +
		ForwardForceOffset.Z * UpVector;

	BodyStaticMesh->AddImpulseAtLocation(Force, Location);
}

void ATank::RotateBody_Implementation(float RotationInput)
{
	FVector Torque = GetActorUpVector() * RotationInput * TurnTorque * GetGroundedSpringRatio();
	BodyStaticMesh->AddAngularImpulse(Torque);
}

void ATank::SetGunRotation_Implementation(FVector MouseLocation, FVector MouseDirection)
{
	MouseDirection.Z = 0.f;
	FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(MouseDirection, GetActorUpVector());
	GunStaticMesh->SetWorldRotation(Direction.Rotation());

	//FVector Origin = GetActorLocation();
	//float d = FVector::DotProduct((FVector(0, 0, Origin.Z) - MouseLocation), FVector::UpVector)
	//	/ FVector::DotProduct(MouseDirection, FVector::UpVector);
	//FVector GroundPoint = MouseLocation + MouseDirection * d;
	//FVector FinalAim = GroundPoint - Origin;
	//FinalAim.Z = 0.f;
	//FinalAim.Normalize();

	//GunStaticMesh->SetWorldRotation(FinalAim.Rotation());
}

void ATank::CounteractDrifting_Implementation()
{
	FVector RightVector = GetActorRightVector();

	float DriftAmount = FVector::DotProduct(GetVelocity(), RightVector);
	FVector AntiDriftForce = RightVector * -DriftAmount * DriftCoefficient;
	BodyStaticMesh->AddImpulse(AntiDriftForce);
}

void ATank::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ATank::OnHealthUpdate()
{
	//Client-specific functionality
	if (IsLocallyControlled())
	{
		FString HealthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, HealthMessage);

		if (CurrentHealth <= 0)
		{
			FString DeathMessage = FString::Printf(TEXT("You have been killed."));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, DeathMessage);
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		FString HealthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, HealthMessage);
	}

	//Functions that occur on all machines. 
	/*
		Any special functionality that should occur as a result of damage or death should be placed here.
	*/
}

void ATank::SetCurrentHealth(float HealthValue)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentHealth = FMath::Clamp(HealthValue, 0.f, MaxHealth);
		OnHealthUpdate();
	}
}

float ATank::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageApplied = CurrentHealth - DamageTaken;
	SetCurrentHealth(DamageApplied);
	return DamageApplied;
}

FVector ATank::GetDirectedSuspensionNormal(float Direction)
{
	TArray<FVector> SuspensionNormals;

	//Consider Front
	if (Direction >= 0.0f) {
		SuspensionNormals.Push(FrontRightSpringComp->GetImpactNormal());
		SuspensionNormals.Push(FrontLeftSpringComp->GetImpactNormal());
	}
	//Consider Back
	if (Direction <= 0.0f)
	{
		SuspensionNormals.Push(BackRightSpringComp->GetImpactNormal());
		SuspensionNormals.Push(BackLeftSpringComp->GetImpactNormal());
	}

	return UKismetMathLibrary::GetVectorArrayAverage(SuspensionNormals).GetSafeNormal();
}

float ATank::GetGroundedSpringRatio()
{
	return (
		FrontRightSpringComp->IsGrounded() + 
		FrontLeftSpringComp->IsGrounded() +
		BackRightSpringComp->IsGrounded() +
		BackLeftSpringComp->IsGrounded()
	) / 4.0f;
}

void ATank::HandleShellFire_Implementation()
{
	FRotator MuzzleRotation = GunStaticMesh->GetSocketRotation(TEXT("GunMuzzle"));
	FVector MuzzleLocation = GunStaticMesh->GetSocketLocation(TEXT("GunMuzzle"));

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	// Spawn the projectile at the muzzle.
	ATankShell* Projectile = World->SpawnActor<ATankShell>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

	if (Projectile)
	{
		// Set the projectile's initial trajectory.
		FVector Direction = MuzzleRotation.Vector();
		Projectile->FireInDirection(Direction);
	}

}