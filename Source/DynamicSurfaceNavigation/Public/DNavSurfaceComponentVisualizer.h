// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "DynamicNavSurfaceComponent.h"
#include "ComponentVisualizer.h"

/**
 * 
 */
class DYNAMICSURFACENAVIGATION_API DNavSurfaceComponentVisualizer :  public FComponentVisualizer
{
	
public:
	void DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI) override;

private:
	FVector Virtual2WorldPosition(FVector position, const UDynamicNavSurfaceComponent* surface);
};
