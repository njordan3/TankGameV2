// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActorBase.h"
#include "Misc/OutputDeviceNull.h"

AMyLevelScriptActorBase::AMyLevelScriptActorBase()
{
	DefaultSpawn = FVector::ZeroVector;
}

FVector AMyLevelScriptActorBase::GetDefaultSpawn()
{
	return DefaultSpawn;
}

void AMyLevelScriptActorBase::SetDefaultSpawn(FVector NewDefaultSpawn)
{
	DefaultSpawn = NewDefaultSpawn;
}

void AMyLevelScriptActorBase::BeginPlay()
{
	Super::BeginPlay();
}