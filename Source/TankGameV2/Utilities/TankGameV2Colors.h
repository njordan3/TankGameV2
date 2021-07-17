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
	static inline FSlateColor DamageColor = FSlateColor(FColor::FromHex(FString(TEXT("FFFF60FF"))));		//pale yellow
	static inline FSlateColor HealColor = FSlateColor(FColor::FromHex(FString(TEXT("536FFFFF"))));			//pale purpley blue

	static inline FSlateColor ReloadBarColor = FSlateColor(FColor::FromHex(FString(TEXT("89CFFFFF"))));		//very pale blue
	static inline FSlateColor ReloadBarTextColor = ReloadBarColor;
	static inline FSlateColor HealthBarColor = FSlateColor(FColor::FromHex(FString(TEXT("DA1C00FF"))));		//red
	static inline FSlateColor HealthBarTextColor = FSlateColor(FColor::FromHex(FString(TEXT("FFFFFFFF"))));	//white

	//Getters and setters are for blueprint access
public:
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetDamageColor(FSlateColor NewDamageColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetDamageColor();

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetHealColor(FSlateColor NewHealColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetHealColor();

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetReloadBarColor(FSlateColor NewReloadBarColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetReloadBarColor();

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetReloadBarTextColor(FSlateColor NewReloadBarTextColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetReloadBarTextColor();

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetHealthBarColor(FSlateColor NewHealthBarColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetHealthBarColor();

	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static void SetHealthBarTextColor(FSlateColor NewHealthBarTextColor);
	UFUNCTION(BlueprintCallable, Category = "TankGameV2 Color")
		static FLinearColor GetHealthBarTextColor();
};