// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActorBase.h"

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