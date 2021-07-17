// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TankGameV2Colors.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API UTankGameV2Colors : public UObject
{
	GENERATED_BODY()
	
public:

	//Uninitialized Slate colors are Fuchsia by default
	//Initial Hex string from UE4 color picker sRGB Hex field
	static inline FSlateColor DamageColor = FSlateColor(FColor::FromHex(FString(TEXT("FFFF60FF"))));	//pale yellow
	static inline FSlateColor HealColor = FSlateColor(FColor::FromHex(FString(TEXT("536FFFFF"))));		//pale blue

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetDamageColor(FSlateColor NewDamageColor);

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetHealColor(FSlateColor NewHealColor);
};