// Copyright Pagan Games. All rights reserved.

#include "VirtualNavMeshArea.h"
#include "Components/ChildActorComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"

// Sets default values
AVirtualNavMeshArea::AVirtualNavMeshArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a new scene component and set it as the root component

	SetRootComponent(GetBrushComponent());
	RootComponent->SetMobility(EComponentMobility::Static);
}
