// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameV2Colors.h"

void UTankGameV2Colors::SetDamageColor(FSlateColor NewDamageColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Damage"));
	DamageColor = NewDamageColor;
}

void UTankGameV2Colors::SetHealColor(FSlateColor NewHealColor)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Heal"));
	HealColor = NewHealColor;
}