// Fill out your copyright notice in the Description page of Project Settings.


#include "TankHUD.h"
#include "Blueprint/UserWidget.h"

ATankHUD::ATankHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> HUD(TEXT("/Game/HUD/HUD_UI"));
	HUDWidgetClass = HUD.Class;
}

void ATankHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ATankHUD::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}