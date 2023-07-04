// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "StableBuoyancyComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DYNAMICSURFACENAVIGATION_API UStableBuoyancyComponent : public USplineComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this component's properties
	UStableBuoyancyComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "StableBuoyancy")
	int NumPoints = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StableBuoyancy")
	float Buoyancy = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StableBuoyancy")
	float DragCoefficient = 0.1f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual float GetWaterLevel(FVector WorldPosition) const;

private:
	void GeneratePantoons();
	TArray<FVector> Pantons;
	void ApplyForceToPoint(UPrimitiveComponent *Root, FVector Point, float BaseForce, float DeltaTime);
};
