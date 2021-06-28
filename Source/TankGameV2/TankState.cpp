// Fill out your copyright notice in the Description page of Project Settings.


#include "TankState.h"
#include "Tank.h"
#include "TankGameV2GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATankState::ATankState(const FObjectInitializer& OI)
	: Super(OI)
{
	//static ConstructorHelpers::FClassFinder<ATank> DefaultCharacter(TEXT("/Game/Characters/Tyler/tyler"));
	//static ConstructorHelpers::FObjectFinder<UTexture2D> DefaultCharacterIcon(TEXT("/Game/Characters/Tyler/Textures/avatar_tyler"));

	TeamNumber = 0;

	//PlayerClass = DefaultCharacter.Class;
	//PlayerIcon = DefaultCharacterIcon.Object;
}

void ATankState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATankState, PlayerClass);
	DOREPLIFETIME(ATankState, PlayerIcon);
	DOREPLIFETIME(ATankState, TeamNumber);
}

/** Assigns a unique bot name to the player */
void ATankState::AssignBotName()
{
	if (ROLE_Authority > GetLocalRole())
	{
		// Only servers should be calling this
	}
	else
	{
		ATankGameV2GameModeBase* GameMode = Cast<ATankGameV2GameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		SetPlayerName(FString::Printf(TEXT("Bot %d"), GameMode->NextBotNameID++));
	}
}