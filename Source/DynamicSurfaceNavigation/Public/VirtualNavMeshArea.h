// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ChildActorComponent.h"
#include "Components/BrushComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "VirtualNavMeshArea.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API AVirtualNavMeshArea : public ANavMeshBoundsVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVirtualNavMeshArea();

public:
	UChildActorComponent *Volume;
};
