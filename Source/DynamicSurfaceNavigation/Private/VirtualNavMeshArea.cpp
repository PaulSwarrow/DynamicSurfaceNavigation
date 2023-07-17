// Copyright Pagan Games. All rights reserved.

#include "VirtualNavMeshArea.h"
#include "Components/ChildActorComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h"

// Sets default values
AVirtualNavMeshArea::AVirtualNavMeshArea()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a new scene component and set it as the root component

	SetRootComponent(GetBrushComponent());
	RootComponent->SetMobility(EComponentMobility::Static);
}

void AVirtualNavMeshArea::BeginPlay()
{

	auto bounds = GetBounds();
}

void AVirtualNavMeshArea::LazyInit()
{
	if (Initialized)
	{
		return;
	}

	auto navsys = GetWorld()->GetNavigationSystem();
	if (navsys == nullptr)
	{
		return;
	}
	auto navmesh = Cast<ARecastNavMesh>(navsys->GetMainNavData());
	float TileSize;
	GConfig->GetFloat(
		TEXT("/Script/NavigationSystem.RecastNavMesh"),
		TEXT("TileSizeUU"),
		TileSize,
		GEngineIni);
	CellSize = TileSize;

	auto Bounds = GetBounds().BoxExtent; //box extent is half the size
	GridOffset = FIntVector(floorf(Bounds.X / CellSize), floorf(Bounds.Y / CellSize), floorf(Bounds.Z / CellSize));
	AreaBounds = GridOffset * 2;
	Grid.resize(AreaBounds.X, std::vector<std::vector<bool>>(AreaBounds.Y, std::vector<bool>(AreaBounds.Z, false)));
	UE_LOG(LogTemp, Warning, TEXT("VNMA Init: Tile: %f, Bounds: %s"), TileSize, *AreaBounds.ToString());
	Initialized = true;
}

FIntVector AVirtualNavMeshArea::GetNavBounds(const FVector Volume, bool RoundDown)
{
	LazyInit();

	if (RoundDown)
	{
		return FIntVector(floorf(Volume.X / CellSize), floorf(Volume.Y / CellSize), floorf(Volume.Z / CellSize));
	}
	else
	{
		return FIntVector(ceilf(Volume.X / CellSize), ceilf(Volume.Y / CellSize), ceilf(Volume.Z / CellSize));
	}
}

bool AVirtualNavMeshArea::FindPlace(const FIntVector Volume, FIntVector &Coord)
{
	LazyInit();
	UE_LOG(LogTemp, Warning, TEXT("VNMA Find place for: %s"), *Volume.ToString());
	for (int z = 0; z <= AreaBounds.Z - Volume.Z; z++)
	{
		for (int y = 0; y <= AreaBounds.Y - Volume.Y; y++)
		{
			for (int x = 0; x <= AreaBounds.X - Volume.X; x++)
			{
				bool emptySpaceFound = true;
				for (int dz = 0; dz < Volume.Z; dz++)
				{
					for (int dy = 0; dy < Volume.Y; dy++)
					{
						for (int dx = 0; dx < Volume.X; dx++)
						{
							if (Grid[x + dx][y + dy][z + dz])
							{
								emptySpaceFound = false;
								break;
							}
						}
						if (!emptySpaceFound)
						{
							break;
						}
					}
					if (!emptySpaceFound)
					{
						break;
					}
				}
				if (emptySpaceFound)
				{
					Coord = FIntVector(x, y, z);
					return true;
				}
			}
		}
	}
	return false;
}

void AVirtualNavMeshArea::ReservePlace(const FIntVector Volume, FIntVector Coord)
{
	LazyInit();
	for (int z = 0; z < Volume.Z; z++)
	{
		for (int y = 0; y < Volume.Y; y++)
		{
			for (int x = 0; x < Volume.X; x++)
			{
				int dx = Coord.X + x;
				int dy = Coord.Y + y;
				int dz = Coord.Z + z;

				if (Grid[dx][dy][dz])
				{
					UE_LOG(LogTemp, Error, TEXT("ReservePlace Error: place has been already reserved!"));
				}
				else
				{
					Grid[dx][dy][dz] = true;
				}
			}
		}
	}
}

void AVirtualNavMeshArea::ReleasePlace(const FIntVector Volume, FIntVector Place)
{
	LazyInit();
	for (int z = 0; z < Volume.Z; z++)
	{
		for (int y = 0; y < Volume.Y; y++)
		{
			for (int x = 0; x < Volume.X; x++)
			{
				int dx = Place.X + x;
				int dy = Place.Y + y;
				int dz = Place.Z + z;

				if (Grid[dx][dy][dz])
				{
					Grid[dx][dy][dz] = false;
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("ReleasePlace Error: place has not been reserved!"));
				}
			}
		}
	}
}

FVector AVirtualNavMeshArea::GetReservedLocation(const FIntVector Volume, FIntVector Coord)
{
	return GetActorLocation() + FVector(Coord - GridOffset) * CellSize + FVector(Volume) * CellSize/ 2;
	 
}
