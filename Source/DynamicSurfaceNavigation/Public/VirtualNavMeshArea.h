// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ChildActorComponent.h"
#include "Components/BrushComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "DSN_ActorProjection.h"
#include <vector>
#include <unordered_map>
#include "VirtualNavMeshArea.generated.h"

USTRUCT(BlueprintType)
struct FVirtualNavMesh
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FIntVector Coord;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FTransform Transform;

};

struct FIntVectorHash
{
    std::size_t operator()(const FIntVector& Vector) const
    {
        // Combine the hash of individual components to create a unique hash for the vector
        std::size_t Hash = 17;
        Hash = Hash * 31 + std::hash<int32>()(Vector.X);
        Hash = Hash * 31 + std::hash<int32>()(Vector.Y);
        Hash = Hash * 31 + std::hash<int32>()(Vector.Z);
        return Hash;
    }
};

UCLASS()
class DYNAMICSURFACENAVIGATION_API AVirtualNavMeshArea : public ANavMeshBoundsVolume
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AVirtualNavMeshArea();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DynamicSurfaceNavigation")
	bool AllwaysGenerateNavMesh = true;

	FIntVector AreaBounds;
	std::vector<std::vector<std::vector<bool>>> Grid;

	bool TryCreateVirtualNavMesh(AActor *realSurfaceActor, FVirtualNavMesh &VirtualNavMesh);

	void ReleaseVirtualNavMesh(AActor *realSurfaceActor, FIntVector Coord);


	virtual void Tick(float DeltaTime) override;

	FIntVector GetVolume(FIntVector ActorCoord);

	FIntVector GetOrigin() { return GridOffset; }

protected:
	virtual void BeginPlay() override;

private:
	std::unordered_map<FIntVector, AActor *, FIntVectorHash> Coord2Actor;//actors storage
	std::unordered_map<FIntVector, FIntVector, FIntVectorHash> Coord2Volume;//volume storage
	
	std::unordered_map<AActor*, FVirtualNavMesh> PendingAddings; //actor creation pending

	FIntVector GetNavBounds(const FVector Volume);
	bool FindPlace(const FIntVector Volume, FIntVector &Coord);
	void ReservePlace(const FIntVector Volume, FIntVector Coord);
	void ReleasePlace(const FIntVector Volume, FIntVector Coord);
	FVector GetReservedLocation(const FIntVector Volume, FIntVector Coord);
	void AddSurface(AActor *Surface, const FVirtualNavMesh& virtualNavMesh);
	//void RemoveSurface(AActor *Surface);
	void LazyInit();
	bool Initialized;
	float CellSize;
	FIntVector GridOffset;
};
