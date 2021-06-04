// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ATank::ATank()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	//Initialize Spring Arm Component
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComp->SetupAttachment((USceneComponent*)GetCapsuleComponent());
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

	//Initialize Tank Body Static Mesh
	BodyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyStaticMesh"));
	BodyStaticMesh->SetupAttachment((USceneComponent*)GetCapsuleComponent());
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BodyMesh(TEXT("/Game/TankMeshes/TankBody.TankBody"));
	UStaticMesh* BodyAsset = BodyMesh.Object;
	BodyStaticMesh->SetStaticMesh(BodyAsset);
	BodyStaticMesh->SetRelativeLocation(FVector(-50.0f, 0.0f, -88.0f));	//Rest Tank Body on the ground and position Capsule Component where the gun rotates

	//Initialize Tank Gun Static Mesh
	GunStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GunStaticMesh"));
	GunStaticMesh->SetupAttachment(BodyStaticMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> GunMesh(TEXT("/Game/TankMeshes/TankGun.TankGun"));
	UStaticMesh* GunAsset = GunMesh.Object;
	GunStaticMesh->SetStaticMesh(GunAsset);
	GunStaticMesh->SetRelativeLocation(FVector(50.0f, 0.0f, 110.0f));	//Rest Tank Gun in the correct position on the Tank Body

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ATank::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using FPSCharacter."));
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