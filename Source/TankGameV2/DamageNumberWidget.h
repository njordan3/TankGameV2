// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite)
		UWidgetAnimation* Animation;

	UPROPERTY(BlueprintReadWrite)
		FText DamageText;
};
