// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "Containers/Array.h"
#include "TankShell.h"
#include "TankController.h"
#include "TankState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "SpringComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATank::ATank()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileClass = ATankShell::StaticClass();

	MaxHealth = 10000000.0f;
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

	NumberOfOverlappingActors = 0;
	bGunIsUnblocked = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	bAlwaysRelevant = true; 
	bNetLoadOnClient = true;

	UpdateOverlapsMethodDuringLevelStreaming = EActorUpdateOverlapsMethod::AlwaysUpdate;
	bGenerateOverlapEventsDuringLevelStreaming = true;

	//Initialize Tank Body Static Mesh ============================================
	BodyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Game/TankMeshes/TankBody.TankBody"));
	UStaticMesh* BodyAsset = BodyMesh.Object;
	BodyStaticMesh->SetStaticMesh(BodyAsset);
	BodyStaticMesh->SetSimulatePhysics(true);
	BodyStaticMesh->bApplyImpulseOnDamage = false;
	BodyStaticMesh->bReplicatePhysicsToAutonomousProxy = false;
	BodyStaticMesh->SetAngularDamping(AngularDamping);
	BodyStaticMesh->SetLinearDamping(LinearDamping);
	BodyStaticMesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	SetRootComponent(BodyStaticMesh);

	//Change Mass Properties
	DefaultCenterOfMass = BodyStaticMesh->GetCenterOfMass();
	BodyStaticMesh->SetCenterOfMass(FVector(0.0f, 0.0f, -150.0f));
	BodyStaticMesh->SetMassOverrideInKg(NAME_None, 100.0f);
	BodyStaticMesh->GetBodyInstance()->UpdateMassProperties();

	//Initialize Camera Spring Arm Component ======================================
	CameraSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArmComponent"));
	CameraSpringArmComp->SetupAttachment(BodyStaticMesh);
	CameraSpringArmComp->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 0.0f),
		FRotator(-89.9f, 0.0f, 0.0f)
	);
	CameraSpringArmComp->TargetArmLength = 3000.0f;
	CameraSpringArmComp->bDoCollisionTest = false;
	CameraSpringArmComp->bInheritPitch = false;
	CameraSpringArmComp->bInheritRoll = false;
	CameraSpringArmComp->bInheritYaw = false;

	//Initialize Camera Component =================================================
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(CameraSpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	//Initialize Tank Gun Spring Arm Component ====================================
	TankGunSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("TankGunSpringArmComponent"));
	TankGunSpringArmComp->SetupAttachment(BodyStaticMesh);
	TankGunSpringArmComp->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));	//Rest Tank Gun in the correct position on the Tank Body'
	TankGunSpringArmComp->TargetArmLength = 0.0f;
	TankGunSpringArmComp->SetIsReplicated(true);	//Replicate Spring Arm; otherwise the Tank Gun is in the wrong spot for clients

	//Initialize Tank Gun Static Mesh =============================================
	GunStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunStaticMesh"));
	GunStaticMesh->SetupAttachment(TankGunSpringArmComp, USpringArmComponent::SocketName);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh(TEXT("/Game/TankMeshes/TankGun.TankGun"));
	UStaticMesh* GunAsset = GunMesh.Object;
	GunStaticMesh->SetStaticMesh(GunAsset);
	GunStaticMesh->SetEnableGravity(false);
	GunStaticMesh->bApplyImpulseOnDamage = false;
	GunStaticMesh->bReplicatePhysicsToAutonomousProxy = false;
	GunStaticMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	GunStaticMesh->SetGenerateOverlapEvents(true);
	if (GetLocalRole() == ROLE_Authority)
	{
		GunStaticMesh->OnComponentBeginOverlap.AddDynamic(this, &ATank::OnGunBeginOverlap);
		GunStaticMesh->OnComponentEndOverlap.AddDynamic(this, &ATank::OnGunEndOverlap);
	}
	GunStaticMesh->SetIsReplicated(true);	//Replicate Gun Static Mesh instead of custom interpolation because it doesn't need interpolation

	//Change Mass Properties
	GunStaticMesh->SetMassOverrideInKg(NAME_None, 0.0f);
	GunStaticMesh->GetBodyInstance()->UpdateMassProperties();

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
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetLocalRole() < ROLE_Authority)
	{
		ClientSimulateTankMovement();
	}
	else
	{
		// Servers should simulate the physics freely and replicate the orientation
		ServerPhysicsState.Pos = GetActorLocation();
		ServerPhysicsState.BodyRot = GetActorRotation();
		ServerPhysicsState.Vel = BodyStaticMesh->GetComponentVelocity();
		ServerPhysicsState.Timestamp = ATankController::GetLocalTime();
	}
}

void ATank::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ATank, CurrentHealth);
	DOREPLIFETIME(ATank, ServerPhysicsState);
}

void ATank::ClientSimulateTankMovement()
{
	ATankController* MyPC = Cast<ATankController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));	//Index 0 because this is client side so there is only one visible controller
	if (nullptr == MyPC || !MyPC->IsNetworkTimeValid() || 0 == ProxyStateCount)
	{
		// We don't know yet know what the time is on the server yet so the timestamps
		// of the proxy states mean nothing; that or we simply don't have any proxy
		// states yet. Don't do any interpolation.
		BodyStaticMesh->SetWorldLocationAndRotation(ServerPhysicsState.Pos, ServerPhysicsState.BodyRot);
	}
	else
	{
		uint64 InterpolationBackTime = 100;
		uint64 ExtrapolationLimit = 500;

		// This is the target playback time of the rigid body
		uint64 InterpolationTime = MyPC->GetNetworkTime() - InterpolationBackTime;

		// Use interpolation if the target playback time is present in the buffer
		if (ProxyStates[0].Timestamp > InterpolationTime)
		{
			// Go through buffer and find correct state to play back
			for (int i = 0; i < ProxyStateCount; i++)
			{
				if (ProxyStates[i].Timestamp <= InterpolationTime || i == ProxyStateCount - 1)
				{
					// The state one slot newer (<100ms) than the best playback state
					FSmoothPhysicsState RHS = ProxyStates[FMath::Max(i - 1, 0)];
					// The best playback state (closest to 100 ms old (default time))
					FSmoothPhysicsState LHS = ProxyStates[i];

					// Use the time between the two slots to determine if interpolation is necessary
					int64 Length = (int64)(RHS.Timestamp - LHS.Timestamp);
					double Time = 0.0F;

					//As the time difference gets closer to 100ms Time gets closer to 1.0f in which case RHS is only used.
					//If Time = 0, then LHS is used directly.
					if (Length > 1)
						Time = (double)(InterpolationTime - LHS.Timestamp) / (double)Length;

					FVector TargetPos = FMath::Lerp(LHS.Pos, RHS.Pos, Time);
					FRotator TargetBodyRot = FMath::Lerp(LHS.BodyRot, RHS.BodyRot, Time);
			
					BodyStaticMesh->SetWorldLocationAndRotation(TargetPos, TargetBodyRot);
					
					return;
				}
			}
		}
		// Use extrapolation
		else
		{
			FSmoothPhysicsState Latest = ProxyStates[0];

			uint64 ExtrapolationLength = InterpolationTime - Latest.Timestamp;
			// Don't extrapolate for more than [ExtrapolationLimit] milliseconds
			if (ExtrapolationLength < ExtrapolationLimit)
			{
				FVector Pos = Latest.Pos + Latest.Vel * ((float)ExtrapolationLength * 0.001f);
				FRotator BodyRot = Latest.BodyRot;

				BodyStaticMesh->SetWorldLocationAndRotation(Pos, BodyRot);
			}
			else
			{
				// Don't move. If we're this far away from the server, we must be pretty laggy.
				// Wait to catch up with the server.
			}
		}
	}
}

void ATank::OnRep_ServerPhysicsState()
{
	// If we get here, we are always the client. Here we store the physics state
	// for physics state interpolation.

	// Shift the buffer sideways, deleting state PROXY_STATE_ARRAY_SIZE
	for (int i = PROXY_STATE_ARRAY_SIZE - 1; i >= 1; i--)
	{
		ProxyStates[i] = ProxyStates[i - 1];
	}

	// Record current state in slot 0
	ProxyStates[0] = ServerPhysicsState;

	// Update used slot count, however never exceed the buffer size
	// Slots aren't actually freed so this just makes sure the buffer is
	// filled up and that uninitalized slots aren't used.
	ProxyStateCount = FMath::Min(ProxyStateCount + 1, PROXY_STATE_ARRAY_SIZE);

	// Check if states are in order
	if (ProxyStates[0].Timestamp < ProxyStates[1].Timestamp)
	{
		UE_LOG(LogAssetData, Verbose, TEXT("Timestamp inconsistent: %d should be greater than %d"), ProxyStates[0].Timestamp, ProxyStates[1].Timestamp);
	}
}

void ATank::MoveForward_Implementation(float ForwardInput, float Yaw)
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

	CounteractDrifting();

	SetGunRotation(Yaw);
}

void ATank::RotateBody_Implementation(float RotationInput, float Yaw)
{
	FVector Torque = GetActorUpVector() * RotationInput * TurnTorque * GetGroundedSpringRatio();
	BodyStaticMesh->AddAngularImpulse(Torque);

	SetGunRotation(Yaw);
}

void ATank::ServerSetGunRotation_Implementation(float Yaw)
{
	SetGunRotation(Yaw);
}

void ATank::SetGunRotation(float Yaw)
{
	//Check if the Tank is upside down
	FRotator Orientation = GetActorRotation();
	int8 OrientationSign = ((Orientation.Pitch < -90.0f || Orientation.Pitch > 90.0f) || (Orientation.Roll < -90.0f || Orientation.Roll > 90.0f)) ? -1 : 1;	//Upside down is -1

	Yaw = OrientationSign * (Yaw - GetActorForwardVector().Rotation().Yaw);
	TankGunSpringArmComp->SetRelativeRotation(FRotator(0.0f, Yaw, 0.0f));
}

void ATank::CounteractDrifting()
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
	bool Dead = CurrentHealth <= 0;

	//Client-specific functionality
	if (IsLocallyControlled())
	{
		if (Dead)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You have been killed."));
		}
		else
		{
			FString HealthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, HealthMessage);
		}
	}

	//Server-specific functionality
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Dead)
		{
			Destroy();
		}
	}
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
	float ActualDamage = Super::TakeDamage(DamageTaken, DamageEvent, EventInstigator, DamageCauser);

	if (EventInstigator != nullptr || DamageCauser != nullptr)
	{
		LastDamageInstigator = EventInstigator;
		LastDamageCauser = DamageCauser;

		float DamageApplied = CurrentHealth - ActualDamage;
		SetCurrentHealth(DamageApplied);
		return DamageApplied;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("No Damage Instigator"));
		return CurrentHealth;
	}
	
}

FVector ATank::GetDirectedSuspensionNormal(float Direction)
{
	TArray<FVector> SuspensionNormals;

	//Consider Front
	if (Direction >= 0.0f) 
	{
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
	if (bGunIsUnblocked)
	{
		FRotator MuzzleRotation = GunStaticMesh->GetSocketRotation(TEXT("GunMuzzle"));
		FVector MuzzleLocation = GunStaticMesh->GetSocketLocation(TEXT("GunMuzzle"));

		UWorld* World = GetWorld();
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

		// Spawn the projectile at the muzzle.
		ATankShell* Projectile = World->SpawnActor<ATankShell>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

		if (Projectile)
		{
			// Set the projectile's initial trajectory.
			FVector Direction = MuzzleRotation.Vector();
			Projectile->FireInDirection(Direction);
		}
	}
}

void ATank::OnGunBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor && (OtherActor != this) && OtherComp)
		{
			NumberOfOverlappingActors++;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Begin: %d"), NumberOfOverlappingActors));

			//Tank Gun is considered unblocked if there are no overlapping Actors, or if the only overlapping Actor is another Tank or a Tank Shell
			bGunIsUnblocked = (NumberOfOverlappingActors == 1 && (OtherActor->GetClass() == ATank::StaticClass() || OtherActor->GetClass() == ATankShell::StaticClass()));
		}
	}
}

void ATank::OnGunEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (OtherActor && (OtherActor != this) && OtherComp)
		{
			NumberOfOverlappingActors--;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("End: %d"), NumberOfOverlappingActors));

			//Tank Gun is considered unblocked if there are no overlapping Actors, or if the only overlapping Actor is another Tank or a Tank Shell
			if (NumberOfOverlappingActors == 0)
			{
				bGunIsUnblocked = true;
			}
			else if (NumberOfOverlappingActors == 1)
			{
				TArray<AActor*> OverlappingActors;
				GetOverlappingActors(OverlappingActors);

				bGunIsUnblocked = (OverlappingActors[0]->GetClass() == ATank::StaticClass() || OtherActor->GetClass() == ATankShell::StaticClass());
			}
			else
			{
				bGunIsUnblocked = false;
			}
		}
	}
}
