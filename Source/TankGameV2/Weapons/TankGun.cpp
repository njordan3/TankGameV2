// Fill out your copyright notice in the Description page of Project Settings.

#include "TankGun.h"
#include "../Tank/Tank.h"
#include "TankShell.h"
#include "Components/TimelineComponent.h"

UTankGun::UTankGun()
{
	PrimaryComponentTick.bCanEverTick = true;

	ProjectileClass = ATankShell::StaticClass();
	bCanFire = true;
	FireRate = 0.25f;
	ReloadPercentage = 1.0f;
	ReloadPercentage = PrevReloadPercentage = 1.0f;
	ReloadAnimationStep = 1.0f / FireRate;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh(TEXT("/Game/TankMeshes/TankGun.TankGun"));
	SetStaticMesh(GunMesh.Object);

	SetEnableGravity(false);
	bApplyImpulseOnDamage = false;
	bReplicatePhysicsToAutonomousProxy = false;
	SetCollisionProfileName(TEXT("TankGun"));
	SetGenerateOverlapEvents(true);

	//Change Mass Properties
	SetMassOverrideInKg(NAME_None, 0.0f);
	GetBodyInstance()->UpdateMassProperties();

	//Initialize HUD Reload Animation Timeline
	ReloadCurve = CreateDefaultSubobject<UCurveFloat>(TEXT("FireRateAnimation"));
	ReloadCurve->FloatCurve.AddKey(0.0f, 0.0f);
	CurrentReloadCurvePoint = ReloadCurve->FloatCurve.AddKey(FireRate, FireRate);

	FOnTimelineFloat TimelineCallback;
	FOnTimelineEventStatic TimelineFinishedCallback;

	TimelineCallback.BindUFunction(this, FName("UpdateReloadPercentage"));
	TimelineFinishedCallback.BindUFunction(this, FName("StopShellFire"));

	ReloadTimeline = NewObject<UTimelineComponent>(this, FName("Reload HUD Animation"));
	ReloadTimeline->AddInterpFloat(ReloadCurve, TimelineCallback);
	ReloadTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
	ReloadTimeline->RegisterComponent();
}

void UTankGun::BeginPlay()
{
	Super::BeginPlay();
}

void UTankGun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (ReloadTimeline != nullptr)
	{
		ReloadTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}
}

void UTankGun::SetGunRotation(float Yaw)
{
	AActor* Owner = GetOwner();

	//Check if the Tank is upside down
	FRotator Orientation = Owner->GetActorRotation();
	int8 OrientationSign = ((Orientation.Pitch < -90.0f || Orientation.Pitch > 90.0f) || (Orientation.Roll < -90.0f || Orientation.Roll > 90.0f)) ? -1 : 1;	//Upside down is -1

	Yaw = OrientationSign * (Yaw - Owner->GetActorForwardVector().Rotation().Yaw);
	SetRelativeRotation(FRotator(0.0f, Yaw, 0.0f));
}

void UTankGun::FireShell()
{
	FRotator MuzzleRotation = GetSocketRotation(TEXT("GunMuzzle"));
	FVector MuzzleLocation = GetSocketLocation(TEXT("GunMuzzle"));

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	APawn* Owner = Cast<APawn>(GetOwner());
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner;

	// Spawn the projectile at the muzzle.
	ATankShell* Projectile = World->SpawnActor<ATankShell>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);

	if (Projectile)
	{
		// Set the projectile's initial trajectory.
		FVector Direction = MuzzleRotation.Vector();
		Projectile->FireInDirection(Direction);
	}
}

void UTankGun::StartShellFire()
{
	if (bCanFire && GunHasValidOverlapping())
	{
		APawn* Owner = Cast<APawn>(GetOwner());
		if (Owner->IsLocallyControlled())
		{
			BeginReloadHUDAnimation();
		}

		if (Owner->GetLocalRole() < ROLE_Authority)
		{
			bCanFire = false;
		}

		ServerHandleShellFire();
	}
}

void UTankGun::StopShellFire()
{
	bCanFire = true;
}

void UTankGun::ServerHandleShellFire_Implementation()
{
	if (bCanFire && GunHasValidOverlapping())
	{
		bCanFire = false;
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UTankGun::StopShellFire, FireRate, false);	//Start reload timer on server
		FireShell();
	}
}

FText UTankGun::GetReloadText()
{
	FText ReloadText = FText::FromString(TEXT("Fire!"));

	if (!bCanFire)
	{
		ReloadText = FText::FromString(TEXT("Reloading"));
	}

	return ReloadText;
}

void UTankGun::SetFireRate(float NewFireRate)
{
	FireRate = NewFireRate;
	ReloadAnimationStep = 1.0f / FireRate;
	ReloadCurve->FloatCurve.DeleteKey(CurrentReloadCurvePoint);
	CurrentReloadCurvePoint = ReloadCurve->FloatCurve.AddKey(FireRate, FireRate);
}

bool UTankGun::GunHasValidOverlapping()
{
	bool Valid = true;

	TArray<AActor*> OverlappingActors;

	GetOverlappingActors(OverlappingActors);

	for (auto& Actor : OverlappingActors)
	{
		Valid = (Actor->GetClass() == ATank::StaticClass() || Actor->GetClass() == ProjectileClass);

		if (!Valid) break;
	}

	return Valid;
}

void UTankGun::UpdateReloadPercentage()
{
	float TimelineValue = ReloadTimeline->GetPlaybackPosition();
	CurveFloatValue = PrevReloadPercentage + ReloadAnimationStep * ReloadCurve->GetFloatValue(TimelineValue);
	ReloadPercentage = FMath::Clamp(CurveFloatValue, 0.0f, 1.0f);
}

void UTankGun::BeginReloadHUDAnimation()
{
	ReloadPercentage = PrevReloadPercentage = 0.0f;
	ReloadTimeline->PlayFromStart();
}