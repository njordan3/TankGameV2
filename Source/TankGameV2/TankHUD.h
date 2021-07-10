// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TankHUD.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API ATankHUD : public AHUD
{
	GENERATED_BODY()

public:
	ATankHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Status")
		TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Status")
		class UUserWidget* CurrentWidget;
};
