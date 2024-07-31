// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDSNCoordTransformer.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, NotBlueprintable)
class UDSNCoordTransformer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYNAMICSURFACENAVIGATION_API IDSNCoordTransformer
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "CoordTransformer")
	virtual FTransform TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection) const = 0;
	UFUNCTION(BlueprintCallable, Category = "CoordTransformer")
	virtual FTransform TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection) const = 0;
};
