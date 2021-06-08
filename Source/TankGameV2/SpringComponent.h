// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SpringComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKGAMEV2_API USpringComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USpringComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
		float SuspensionLength;

	UPROPERTY(EditAnywhere)
		float SpringCoefficient;

	UPROPERTY(EditAnywhere)
		float DampingCoefficient;

protected:
	UPrimitiveComponent* Owner;

private:
	float PreviousSuspensionLengthDelta;
};
