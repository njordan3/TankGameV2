// Fill out your copyright notice in the Description page of Project Settings.

#include "Tank.h"
#include "../Weapons/TankGun.h"
#include "Containers/Array.h"
#include "../Weapons/TankShell.h"
#include "../Setup/TankState.h"
#include "../Setup/Gamemodes/TankGameV2GameModeBase.h"
#include "Components/DamageNumberWidget.h"
#include "../Utilities/TankGameV2Colors.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpringComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ATank::ATank()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	HealthPercentage = 1.0f;

	SuspensionLength = 60.0f;
	SpringCoefficient = 2000.0f;
	DampingCoefficient = 100.0f;

	ForwardForce = 1500.0f;	
	ForwardForceOffset = FVector(10.0f, 0.0f, -10.0f);
	TurnTorque = 45000.0f;
	AngularDamping = 1.5f;
	LinearDamping = 0.5f;
	DriftCoefficient = 1.0f;

	DestroyTime = 5.0f;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	bAlwaysRelevant = true; 
	bNetLoadOnClient = true;

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
	BodyStaticMesh->SetCollisionProfileName(TEXT("TankBody"));
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

	//Initialize Tank Gun =========================================================
	Gun = CreateDefaultSubobject<UTankGun>(TEXT("TankGun"));
	Gun->SetupAttachment(TankGunSpringArmComp, USpringArmComponent::SocketName);

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

	//Set Damage Number Widget. The widget component keeps the widget on the Tank instead of 0,0,0
	static ConstructorHelpers::FClassFinder<UUserWidget> Widget(TEXT("/Game/Widgets/DamageNumbers"));
	DamageNumberWidgetClass = Widget.Class;
	if (DamageNumberWidgetClass != nullptr)
	{
		DamageNumberWidget = CreateWidget<UUserWidget>(GetWorld(), DamageNumberWidgetClass);
		DamageNumberWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("DamageNumberWidgetComp"));
		DamageNumberWidgetComp->SetupAttachment(BodyStaticMesh);
		DamageNumberWidgetComp->SetCollisionProfileName(TEXT("NoCollision"));
		DamageNumberWidgetComp->SetWidget(DamageNumberWidget);
		DamageNumberWidgetComp->SetWidgetClass(DamageNumberWidgetClass);
		DamageNumberWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		DamageNumberWidgetComp->SetDrawSize(FVector2D(1.0f, 1.0f));
	}
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();

	// Set Front Right variables
	FrontRightSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("FrontRight")),
		BodyStaticMesh->GetSocketRotation(TEXT("FrontRight")));
	FrontRightSpringComp->SuspensionLength = SuspensionLength;
	FrontRightSpringComp->SpringCoefficient = SpringCoefficient;
	FrontRightSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Front Left variables
	FrontLeftSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("FrontLeft")),
		BodyStaticMesh->GetSocketRotation(TEXT("FrontLeft")));
	FrontLeftSpringComp->SuspensionLength = SuspensionLength;
	FrontLeftSpringComp->SpringCoefficient = SpringCoefficient;
	FrontLeftSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Back Right variables
	BackRightSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("BackRight")),
		BodyStaticMesh->GetSocketRotation(TEXT("BackRight")));
	BackRightSpringComp->SuspensionLength = SuspensionLength;
	BackRightSpringComp->SpringCoefficient = SpringCoefficient;
	BackRightSpringComp->DampingCoefficient = DampingCoefficient;

	// Set Back Left variables
	BackLeftSpringComp->SetWorldLocationAndRotation(
		BodyStaticMesh->GetSocketLocation(TEXT("BackLeft")),
		BodyStaticMesh->GetSocketRotation(TEXT("BackLeft")));
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
		ServerPhysicsState.BodyPos = GetActorLocation();
		ServerPhysicsState.BodyRot = GetActorRotation();
		ServerPhysicsState.BodyVel = BodyStaticMesh->GetComponentVelocity();
		ServerPhysicsState.GunPos = Gun->GetComponentLocation();
		ServerPhysicsState.GunRot = Gun->GetComponentRotation();
		ServerPhysicsState.Timestamp = ATankController::GetLocalTime();
	}

	if (IsLocallyControlled() && Gun != nullptr)
	{
		Gun->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
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
		BodyStaticMesh->SetWorldLocationAndRotation(ServerPhysicsState.BodyPos, ServerPhysicsState.BodyRot);
		Gun->SetWorldLocationAndRotation(ServerPhysicsState.GunPos, ServerPhysicsState.GunRot);
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

					FVector TargetBodyPos = FMath::Lerp(LHS.BodyPos, RHS.BodyPos, Time);
					FRotator TargetBodyRot = FMath::Lerp(LHS.BodyRot, RHS.BodyRot, Time);
					BodyStaticMesh->SetWorldLocationAndRotation(TargetBodyPos, TargetBodyRot);

					if (!IsLocallyControlled())
					{
						FVector TargetGunPos = FMath::Lerp(LHS.GunPos, RHS.GunPos, Time);
						FRotator TargetGunRot = FMath::Lerp(LHS.GunRot, RHS.GunRot, Time);
						Gun->SetWorldLocationAndRotation(TargetGunPos, TargetGunRot);
					}
					
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
				FVector BodyPos = Latest.BodyPos + Latest.BodyVel * ((float)ExtrapolationLength * 0.001f);
				FRotator BodyRot = Latest.BodyRot;
				BodyStaticMesh->SetWorldLocationAndRotation(BodyPos, BodyRot);

				FVector GunPos = Latest.GunPos + Latest.BodyVel * ((float)ExtrapolationLength * 0.001f);
				FRotator GunRot = Latest.GunRot;
				Gun->SetWorldLocationAndRotation(GunPos, GunRot);
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

void ATank::ServerActivateMovementInput_Implementation(FMovementInput Input)
{
	ActivateMovementInput(Input);
}

void ATank::ActivateMovementInput(FMovementInput Input)
{
	float GroundedSpringRatio = GetGroundedSpringRatio();

	MoveForward(Input.ForwardInput, Input.IsHandBraked, GroundedSpringRatio);
	RotateBody(Input.BodyRotationInput, GroundedSpringRatio);
	Gun->SetGunRotation(Input.GunRotationYaw);
	UseHandBrake(Input.IsHandBraked, GroundedSpringRatio);
}

void ATank::UseHandBrake(bool IsHandBraked, float GroundedSpringRatio)
{
	//Only work while fully grounded
	if (IsHandBraked && GroundedSpringRatio == 1.0f)
	{
		FVector Velocity = GetVelocity();

		if (Velocity.Size() > 30.0f)
		{
			//Get the sign of the direction the braking force should be applied in relative to the current velocity
			//We want to apply a force in the opposite direction we are currently moving
			//Using -2 and 2 doubles the force applied for faster braking
			float Sign = (FVector::DotProduct(GetActorForwardVector(), Velocity) > 0) ? -2.0 : 2.0;

			ApplyForceOnSuspensionNormal(Sign);
		}
		else
		{
			BodyStaticMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
		}
	}
}

void ATank::MoveForward(float ForwardInput, bool IsHandBraked, float GroundedSpringRatio)
{
	if (GroundedSpringRatio > 0.0f)
	{
		if (!IsHandBraked && ForwardInput != 0.0f)
		{
			//If ForwardInput is the opposite sign of the current Velocity, double the amount of force applied
			//The Dot Product is positive when moving forward and negative when moving backwards
			//This operation checks if ForwardInput and the Dot Product are opposite signs
			if ((ForwardInput * FVector::DotProduct(GetActorForwardVector(), GetVelocity())) < 0.0f)
			{
				ForwardInput *= 2.0f;
			}

			ApplyForceOnSuspensionNormal(ForwardInput * GroundedSpringRatio);

			CounteractDrifting();
		}
		else
		{
			RedirectVelocityForward();
		}
	}
}

void ATank::RotateBody(float RotationInput, float GroundedSpringRatio)
{
	if (GroundedSpringRatio > 0.0f)
	{
		if (RotationInput != 0.0f)
		{
			//If RotationInput is the opposite sign of the current Angular Velocity Z component, double the amount of torque applied
			if (RotationInput * BodyStaticMesh->GetPhysicsAngularVelocityInRadians().Z < 0.0f)
			{
				RotationInput *= 2.0f;
			}

			FVector Torque = GetActorUpVector() * RotationInput * TurnTorque * GroundedSpringRatio;
			BodyStaticMesh->AddAngularImpulseInRadians(Torque);
		}
		//Remove rotation if no RotationInput and the Velocity Magnitude is less than or equal to 500
		//Using FVector2D because we don't want to consider Z component in the Magnitude
		else if (FVector2D(GetVelocity()).Size() <= 500.0f)
		{
			BodyStaticMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);
		}

		RedirectVelocityForward();
	}
}

void ATank::CounteractDrifting()
{
	FVector RightVector = GetActorRightVector();

	float DriftAmount = FVector::DotProduct(GetVelocity(), RightVector);
	FVector AntiDriftForce = RightVector * -DriftAmount * DriftCoefficient;
	BodyStaticMesh->AddImpulse(AntiDriftForce);
}

void ATank::RedirectVelocityForward()
{
	//Move velocity to run along the forward vector
	if (GetGroundedSpringRatio() == 1.0f)
	{
		FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(GetActorForwardVector(), GetDirectedSuspensionNormal());

		float DriftAmount = FVector::DotProduct(GetVelocity(), Direction);
		FVector Redirect = Direction * DriftAmount;

		BodyStaticMesh->SetAllPhysicsLinearVelocity(Redirect);
	}
}

void ATank::OnRep_CurrentHealth()
{
	OnHealthUpdate();
}

void ATank::OnHealthUpdate()
{
	bool Dead = CurrentHealth <= 0;
	HealthPercentage = CurrentHealth / MaxHealth;
	AController* PC = GetController();

	//Client
	if (IsLocallyControlled() && GetLocalRole() < ROLE_Authority)
	{
		if (Dead)
		{
			if (PC != nullptr)
			{
				
				PC->UnPossess();
			}
		}
		else
		{
			//Took damage but didn't die
		}
	}
	
	//Server
	if (GetLocalRole() == ROLE_Authority)
	{
		if (Dead)
		{
			if (!GetWorld()->GetTimerManager().IsTimerActive(TankDestroyTimer))
			{
				//Respawn and Possess new Tank Actor on a timer
				Cast<ATankGameV2GameModeBase>(GetWorld()->GetAuthGameMode())->Respawn(PC);
				//Despawn Tank Actor on a timer
				GetWorld()->GetTimerManager().SetTimer(TankDestroyTimer, this, &ATank::CallDestroy, DestroyTime, false);
			}

			//Unposses the Server Host here because the OnHealthUpdate() function is called on the Server first
			//and the Player Controller needs to be unpossed AFTER setting a timer to respawn
			if (IsLocallyControlled())
			{
				if (PC != nullptr)
				{
					PC->UnPossess();
				}
			}
		}
		else
		{
			//Took damage but didn't die
		}
	}
}

void ATank::CallDestroy()
{
	Destroy();
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

FText ATank::GetHealthText()
{
	int32 HP = FMath::RoundHalfFromZero(CurrentHealth);
	FString HPS = FString::FromInt(HP);
	FText HPText = FText::FromString(HPS);
	return HPText;
}

void ATank::PlayDamageNumber(int32 Damage)
{
	UDamageNumberWidget* Widget = Cast<UDamageNumberWidget>(DamageNumberWidget);
	if (Damage > 0)
	{
		Widget->SetCurrentColor(UTankGameV2Colors::DamageColor);
		Widget->DamageText = FText::FromString(FString::Printf(TEXT("-%d"), Damage));
	}
	else
	{
		Widget->SetCurrentColor(UTankGameV2Colors::HealColor);
		Widget->DamageText = FText::FromString(FString::Printf(TEXT("+%d"), Damage));
	}
	Widget->PlayAnimation(Widget->Animation);
}

void ATank::ApplyForceOnSuspensionNormal(float Coeff)
{
	FVector ForwardVector = GetActorForwardVector();
	FVector RightVector = GetActorRightVector();
	FVector UpVector = GetActorUpVector();

	//Project the Tank's forward vector onto the plane of the Suspension's average raycasting impact normal
	//Calculate the force and reduce it by the ratio of however many Springs are grounded
	FVector Direction = UKismetMathLibrary::ProjectVectorOnToPlane(ForwardVector, GetDirectedSuspensionNormal());

	FVector Force = Direction * ForwardForce * Coeff;

	//Calculate location to add the force. The offsets add a "bounciness" 
	//to accelerating and braking by moving the force location slightly away from the local center
	FVector Location = GetActorLocation() +
		ForwardForceOffset.X * ForwardVector +
		ForwardForceOffset.Y * RightVector +
		ForwardForceOffset.Z * UpVector;

	BodyStaticMesh->AddImpulseAtLocation(Force, Location);
}