// Copyright Pagan Games. All rights reserved.

#include "VirtualNavMeshArea.h"
#include "Components/ChildActorComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "NavigationSystem.h"
#include <vector>
#include <unordered_map>
#include "NavMesh/RecastNavMesh.h"
#include "NavigationInvokerComponent.h"
#include "Components/SceneComponent.h"
#include "VirtualNavMeshAreaComponent.h"

// Sets default values
AVirtualNavMeshArea::AVirtualNavMeshArea()
{
	
    // Initialize PendingAddings as an empty map
    PendingAddings = std::unordered_map<AActor*, FVirtualNavMesh>();
	Coord2Actor = std::unordered_map<FIntVector, AActor *, FIntVectorHash>();
	Coord2Volume = std::unordered_map<FIntVector, FIntVector, FIntVectorHash>();
	
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	// Create a new scene component and set it as the root component

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MyRootComponent"));
	//RootComponent->SetMobility(EComponentMobility::Static);
	SetRootComponent(RootComponent);
	
    auto VirtualNavMeshAreaComponent = CreateDefaultSubobject<UVirtualNavMeshAreaComponent>(TEXT("VirtualNavMeshAreaComponent"));
    VirtualNavMeshAreaComponent->SetupAttachment(RootComponent); // Attach to the root component or another appropriate component

	//TODO initialize pendingaddings
	
	float TileSize;
	GConfig->GetFloat(
		TEXT("/Script/NavigationSystem.RecastNavMesh"),
		TEXT("TileSizeUU"),
		TileSize,
		GEngineIni);
	CellSize = TileSize;
}

void AVirtualNavMeshArea::BeginPlay()
{
	Super::BeginPlay();
}

void AVirtualNavMeshArea::RecalculateGrid()
{
	if(!Grid.empty()) return;

	float TileSize;
	GConfig->GetFloat(
		TEXT("/Script/NavigationSystem.RecastNavMesh"),
		TEXT("TileSizeUU"),
		TileSize,
		GEngineIni);
	CellSize = TileSize;


	Grid.resize(AreaSize.X, std::vector<std::vector<bool>>(AreaSize.Y, std::vector<bool>(AreaSize.Z, false)));
	UE_LOG(LogTemp, Warning, TEXT("VNMA Init: Tile: %f, Volume: %s ,GridSize: %s"), TileSize,*AreaSize.ToString(), *AreaSize.ToString());
	Initialized = true;
}

void AVirtualNavMeshArea::Tick(float DeltaTime)
{
	for (const auto &element : PendingAddings)
	{
		AddSurface(element.first, element.second);
	}

	PendingAddings.clear();
}

FIntVector AVirtualNavMeshArea::GetVolume(FIntVector ActorCoord)
{
	if (Coord2Volume.find(ActorCoord) != Coord2Volume.end())
	{
		return Coord2Volume[ActorCoord];
	}
    return FIntVector::ZeroValue;
}


bool AVirtualNavMeshArea::TryCreateVirtualNavMesh(AActor *realSurfaceActor, FVirtualNavMesh &VirtualNavMesh)
{
	if (!PendingAddings.empty() && PendingAddings.find(realSurfaceActor) != PendingAddings.end())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNMA: Actor has already been registered: %s"), *realSurfaceActor->GetName());
		return false;
	}

	// calculate required volume:
	FVector Center;
	FVector Bounds;
	realSurfaceActor->GetActorBounds(true, Center, Bounds, true);
	Bounds *= 2;
	auto Volume = GetNavBounds(Bounds);
	// Search for place available
	FIntVector Coord;
	if (FindPlace(Volume, Coord))
	{
		
		UE_LOG(LogTemp, Warning, TEXT("VNMA: Found place for %s: %s"), *realSurfaceActor->GetName(), *Coord.ToString());
		FTransform VirtualTransform(FRotator::ZeroRotator, GetReservedLocation(Volume, Coord),realSurfaceActor->GetActorScale3D());
		// reservation data:
		VirtualNavMesh.Coord = Coord;
		VirtualNavMesh.Transform = VirtualTransform;
		// reserve:
		ReservePlace(Volume, Coord);

		// pend virtual actor creation:
		PendingAddings[realSurfaceActor] = VirtualNavMesh;
		return true;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("VNMA: Not enough place for an actor: %s, actor's volume: %s"), *realSurfaceActor->GetName(), *Volume.ToString());
	}
	return false;
}

void AVirtualNavMeshArea::ReleaseVirtualNavMesh(AActor *realSurfaceActor, FIntVector Coord)
{
	if (PendingAddings.find(realSurfaceActor) != PendingAddings.end())
	{
		PendingAddings.erase(realSurfaceActor);
	}

	// TODO provide more safety!
	if (Coord2Volume.find(Coord) != Coord2Volume.end())
	{
		// release volume:
		ReleasePlace(Coord2Volume[Coord], Coord);
		Coord2Volume.erase(Coord);

		// destroy virtual actor:
		if (Coord2Actor.find(Coord) != Coord2Actor.end())
		{
			Coord2Actor[Coord]->Destroy();
			Coord2Actor.erase(Coord);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNMA: Can not release %s: actor hasn't been registered"), *realSurfaceActor->GetName());
	}
}

FIntVector AVirtualNavMeshArea::GetNavBounds(const FVector Volume)
{
	auto ExtenedVolume = Volume + FVector(CellSize / 2, CellSize / 2, CellSize / 2);
	return FIntVector(ceilf(ExtenedVolume.X / CellSize), ceilf(ExtenedVolume.Y / CellSize), ceilf(ExtenedVolume.Z / CellSize));
}

bool AVirtualNavMeshArea::FindPlace(const FIntVector Volume, FIntVector &Coord)
{
	RecalculateGrid();
	UE_LOG(LogTemp, Warning, TEXT("VNMA Find place for: %s"), *Volume.ToString());

	for (int z = 0; z <= AreaSize.Z - Volume.Z; z++)
	{
		for (int y = 0; y <= AreaSize.Y - Volume.Y; y++)
		{
			for (int x = 0; x <= AreaSize.X - Volume.X; x++)
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
	RecalculateGrid();

	Coord2Volume[Coord] = Volume;
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

void AVirtualNavMeshArea::ReleasePlace(const FIntVector Volume, FIntVector Coord)
{
	RecalculateGrid();
	Coord2Volume.erase(Coord);
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

FVector AVirtualNavMeshArea::GetCoordPoisition(FIntVector Coord)
{
 	return FVector(Coord + GetOrigin()) * CellSize + CellSize / 2;
}

FVector AVirtualNavMeshArea::GetReservedLocation(const FIntVector Volume, FIntVector Coord)
{
	return FVector(Coord + GetOrigin()) * CellSize + FVector(Volume) * CellSize / 2;
}

void AVirtualNavMeshArea::AddSurface(AActor *Surface, const FVirtualNavMesh &virtualNavMesh)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	auto VirtualSurfaceActor = GetWorld()->SpawnActor<ADSN_ActorProjection>(
		ADSN_ActorProjection::StaticClass(),
		virtualNavMesh.Transform,
		SpawnParams);

	if (VirtualSurfaceActor != nullptr)
	{
		Coord2Actor[virtualNavMesh.Coord] = VirtualSurfaceActor;
		VirtualSurfaceActor->Init(Surface);
		if (AllwaysGenerateNavMesh)
		{
			// TODO create and add NavigationInvoker component
			UNavigationInvokerComponent *navInvoker = NewObject<UNavigationInvokerComponent>(VirtualSurfaceActor);

			FVector Center;
			FVector Bounds;
			VirtualSurfaceActor->GetActorBounds(true, Center, Bounds, true);
			navInvoker->SetGenerationRadii(Bounds.GetMax(), Bounds.GetMax() + 1000);
			navInvoker->RegisterComponent();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNMA: Surface %s failed to spawn virtual actor"), *Surface->GetOwner()->GetName());
		return;
	}
}