// Fill out your copyright notice in the Description page of Project Settings.


#include "TankGameV2Colors.h"

//DamageColor
void UTankGameV2Colors::SetDamageColor(FSlateColor NewDamageColor)
{
	DamageColor = NewDamageColor;
}

FLinearColor UTankGameV2Colors::GetDamageColor()
{
	return DamageColor.GetSpecifiedColor();
}

//HealColor
void UTankGameV2Colors::SetHealColor(FSlateColor NewHealColor)
{
	HealColor = NewHealColor;
}

FLinearColor UTankGameV2Colors::GetHealColor()
{
	return HealColor.GetSpecifiedColor();
}

//ReloadBar
void UTankGameV2Colors::SetReloadBarColor(FSlateColor NewReloadBarColor)
{
	ReloadBarColor = NewReloadBarColor;
}

FLinearColor UTankGameV2Colors::GetReloadBarColor()
{
	return ReloadBarColor.GetSpecifiedColor();
}

//ReloadBar Text
void UTankGameV2Colors::SetReloadBarTextColor(FSlateColor NewReloadBarTextColor)
{
	ReloadBarTextColor = NewReloadBarTextColor;
}

FLinearColor UTankGameV2Colors::GetReloadBarTextColor()
{
	return ReloadBarTextColor.GetSpecifiedColor();
}

//HealthBar
void UTankGameV2Colors::SetHealthBarColor(FSlateColor NewHealthBarColor)
{
	HealthBarColor = NewHealthBarColor;
}

FLinearColor UTankGameV2Colors::GetHealthBarColor()
{
	return HealthBarColor.GetSpecifiedColor();
}

//HealthBar Text
void UTankGameV2Colors::SetHealthBarTextColor(FSlateColor NewHealthBarTextColor)
{
	HealthBarTextColor = NewHealthBarTextColor;
}

FLinearColor UTankGameV2Colors::GetHealthBarTextColor()
{
	return HealthBarTextColor.GetSpecifiedColor();
}