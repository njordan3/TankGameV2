// Fill out your copyright notice in the Description page of Project Settings.


#include "TankSpawnPoint.h"

// Sets default values
ATankSpawnPoint::ATankSpawnPoint()
{
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SpawnMesh(TEXT("/Game/TankMeshes/Tank.Tank"));
	StaticMeshComp->SetStaticMesh(SpawnMesh.Object);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(StaticMeshComp);
	SetActorHiddenInGame(true);
}

// Called when the game starts or when spawned
void ATankSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}