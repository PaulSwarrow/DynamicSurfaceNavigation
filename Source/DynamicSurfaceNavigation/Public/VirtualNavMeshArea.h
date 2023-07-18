// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ChildActorComponent.h"
#include "Components/BrushComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "DynamicNavSurfaceComponent.h"
#include "VirtualSurfaceActor.h"
#include <vector>
#include <unordered_map>
#include "VirtualNavMeshArea.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API AVirtualNavMeshArea : public ANavMeshBoundsVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVirtualNavMeshArea();

	FIntVector AreaBounds;
	std::vector<std::vector<std::vector<bool>>> Grid;

	bool FindPlace(const FIntVector Volume, FIntVector &Coord);

	void ReservePlace(const FIntVector Volume, FIntVector Coord);

	void ReleasePlace(const FIntVector Volume, FIntVector Coord);

	FVector GetReservedLocation(const FIntVector Volume, FIntVector Coord);

	FIntVector GetNavBounds(const FVector Volume, bool RoundDown);

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	std::unordered_map<UDynamicNavSurfaceComponent *, AVirtualSurfaceActor *> ChildrenMap;
	void AddSurface(UDynamicNavSurfaceComponent *Surface);
	void RemoveSurface(UDynamicNavSurfaceComponent *Surface);
	void LazyInit();
	bool Initialized;
	float CellSize;
	FIntVector GridOffset;
};
