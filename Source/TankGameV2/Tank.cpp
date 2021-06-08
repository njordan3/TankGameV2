// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

// Sets default values
ATank::ATank()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SuspensionLength = 60.0f;
	SpringCoefficient = 800.0f;
	DampingCoefficient = 100;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Initialize Tank Body Static Mesh
	BodyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMesh"));
	RootComponent = BodyStaticMesh;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Game/TankMeshes/TankBody.TankBody"));
	UStaticMesh* BodyAsset = BodyMesh.Object;
	BodyStaticMesh->SetStaticMesh(BodyAsset);

	//Initialize Tank Gun Static Mesh
	GunStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunStaticMesh"));
	GunStaticMesh->SetupAttachment(BodyStaticMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh(TEXT("/Game/TankMeshes/TankGun.TankGun"));
	UStaticMesh* GunAsset = GunMesh.Object;
	GunStaticMesh->SetStaticMesh(GunAsset);
	GunStaticMesh->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));	//Rest Tank Gun in the correct position on the Tank Body

	//Initialize Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment(BodyStaticMesh);
	SpringArmComp->SetRelativeLocationAndRotation(
		FVector(0.0f, 0.0f, 0.0f),
		FRotator(-89.9f, 0.0f, 0.0f)
	);
	SpringArmComp->TargetArmLength = 1500.0f;
	SpringArmComp->bInheritPitch = false;
	SpringArmComp->bInheritRoll = false;
	SpringArmComp->bInheritYaw = false;

	//Initialize Camera Component
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Initialize Front Right Spring Component
	FrontRightSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("FrontRightSpringComponent"));
	FrontRightSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Front Left Spring Component
	FrontLeftSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("FrontLeftSpringComponent"));
	FrontLeftSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Back Right Spring Component
	BackRightSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("BackRightSpringComponent"));
	BackRightSpringComp->SetupAttachment(BodyStaticMesh);

	//Initialize Back Left Spring Component
	BackLeftSpringComp = CreateDefaultSubobject<USpringComponent>(TEXT("BackLeftSpringComponent"));
	BackLeftSpringComp->SetupAttachment(BodyStaticMesh);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
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
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Tank Initialized"));
}

// Called every frame
void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATank::SetRelativeGunRotation(FRotator Rotation)
{
	GunStaticMesh->SetRelativeRotation(Rotation);
}

FRotator ATank::GetRelativeGunRotation()
{
	return GunStaticMesh->GetRelativeRotation();
}
