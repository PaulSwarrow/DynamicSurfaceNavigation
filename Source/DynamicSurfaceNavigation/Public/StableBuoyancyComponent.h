// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "StableBuoyancyComponent.generated.h"

/**
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StableBuoyancy")
	bool FixCenterOfMass = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StableBuoyancy")
	FVector FixedCenterOfMass = FVector(0.0f, 0.0f, -100.0f);

protected:
	// Called when the gaame starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "StableBuoyancy")
	float GetWaterLevel(FVector WorldPosition) const;

	virtual float GetWaterLevel_Implementation(FVector WorldPosition) const;

private:
	void GeneratePantoons();
	TArray<FVector> Pantons;
	void ApplyForceToPoint(UPrimitiveComponent *Root, FVector Point, float BaseForce, float DeltaTime);
};
