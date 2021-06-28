// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TankState.generated.h"

/**
 * 
 */
UCLASS()
class TANKGAMEV2_API ATankState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ATankState(const class FObjectInitializer& OI);

	/** the character class to spawn */
	UPROPERTY(Transient, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player")
		TSubclassOf<class ATank> PlayerClass;

	/** the icon of the character */
	UPROPERTY(Transient, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player")
		UTexture2D* PlayerIcon;

	/** The team number */
	UPROPERTY(Transient, Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player")
		int32 TeamNumber;

	/** Assigns a unique bot name to the player */
	UFUNCTION(BlueprintCallable, Category = "Player")
		void AssignBotName();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
