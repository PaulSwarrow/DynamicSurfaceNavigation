// Copyright Pagan Games. All rights reserved.

#include "DNavSurfaceComponentVisualizer.h"
#include "DynamicNavSurfaceComponent.h"
#include "NavMesh/RecastNavMesh.h"

#include "NavigationSystem.h"
#include "CoreMinimal.h"
#include "LocalVertexFactory.h"
#include "Math/Color.h"
#include "Math/UnrealMathUtility.h"

void DNavSurfaceComponentVisualizer::DrawVisualization(const UActorComponent *Component, const FSceneView *View, FPrimitiveDrawInterface *PDI)
{
    if(Component == nullptr) return;
    auto owner = Component->GetOwner();
    if(owner == nullptr) return;

    auto area = Cast<AVirtualNavMeshArea>(owner);

    auto extend = FVector(area->CellSize/2, area->CellSize/2, area->CellSize/2);

    for (int x = 0; x < area->AreaSize.X; x++)
    { 
        for (int y = 0; y < area->AreaSize.Y; y++)
        {        
            for (int z = 0; z < area->AreaSize.Z; z++)
            {        
                FVector center = area->GetCoordPoisition(FIntVector(x,y,z));
                //auto color = FLinearColor::Green;
                auto color = area->Grid.empty()? FLinearColor::Green: area->Grid[x][y][z]? FLinearColor::Red : FLinearColor::Green;
                DrawColorBox(PDI, center, extend, color);
            }
        }
    }
    

    /*const UDynamicNavSurfaceComponent* Surface = Cast<UDynamicNavSurfaceComponent>(Component);
    auto Area = Surface->GetVirtualArea();
    auto navsys = Surface->GetWorld()->GetNavigationSystem();
    if (navsys == nullptr || Area == nullptr)
    {
        return;
    }
    auto navmesh = Cast<ARecastNavMesh>(navsys->GetMainNavData());
    auto TileSize = navmesh->TileSizeUU;

    auto origin = Area->GetOrigin();
    auto coord = Surface->VirtualNavMeshData.Coord;
    auto volume = Area->GetVolume(coord);

    //
    int32 TileX = coord.X + origin.X;
    int32 TileY = coord.Y + origin.Y;
    TArray<int32> TileIndices;
    return;
    for (int x = 0; x < volume.X; x++)
    { 
        for (int y = 0; y < volume.Y; y++)
        {

            navmesh->GetNavMeshTilesAt(TileX + x, TileY + y, TileIndices);
        }
    }*/
    
   // navmesh->GetNavMeshTileXY(VirtualSurface->GetActorLocation(), TileX, TileY);
    /*if (TileIndices.Num() == 0)
    {
        return;
    }

    for (int t = 0; t < TileIndices.Num(); t++)
    {
        FRecastDebugGeometry RecastDebugGeometry;
        RecastDebugGeometry.bGatherNavMeshEdges = true;

        navmesh->GetDebugGeometryForTile(RecastDebugGeometry, TileIndices[t]);

        for (int c = 0; c < RecastDebugGeometry.Clusters.Num(); c++)
        {

            for (int32 i = 0; i < RecastDebugGeometry.Clusters[c].MeshIndices.Num() - 3; i += 3)
            {
                int32 Index0 = RecastDebugGeometry.Clusters[c].MeshIndices[i];
                int32 Index1 = RecastDebugGeometry.Clusters[c].MeshIndices[i + 1];
                int32 Index2 = RecastDebugGeometry.Clusters[c].MeshIndices[i + 2];

                FVector Vertex0 = Virtual2WorldPosition(RecastDebugGeometry.MeshVerts[Index0], Surface);
                FVector Vertex1 = Virtual2WorldPosition(RecastDebugGeometry.MeshVerts[Index1], Surface);
                FVector Vertex2 = Virtual2WorldPosition(RecastDebugGeometry.MeshVerts[Index2], Surface);
                FLinearColor Color = FLinearColor::Red;

                // Draw the triangle
                PDI->DrawLine(Vertex0, Vertex1, Color, SDPG_World);
                PDI->DrawLine(Vertex1, Vertex2, Color, SDPG_World);
                PDI->DrawLine(Vertex2, Vertex0, Color, SDPG_World);
            }
        }
    }*/
}

FVector DNavSurfaceComponentVisualizer::Virtual2WorldPosition(FVector position, const UDynamicNavSurfaceComponent *surface)
{
    return FVector::ZeroVector;
    /*
    auto local = surface->VirtualSurfaceActor->GetTransform().InverseTransformPosition(position);
    auto world = surface->GetOwner()->GetTransform().TransformPosition(local);
    return world;
    */
}

void DNavSurfaceComponentVisualizer::DrawColorBox(FPrimitiveDrawInterface *PDI, FVector Center, const FVector &Extent, const FLinearColor &Color)
{
    
    const FVector Min = Center - Extent * 0.9f;
    const FVector Max = Center + Extent * 0.9f;

    const FVector Vertices[8] = {
        FVector(Min.X, Min.Y, Min.Z),
        FVector(Max.X, Min.Y, Min.Z),
        FVector(Min.X, Max.Y, Min.Z),
        FVector(Max.X, Max.Y, Min.Z),
        FVector(Min.X, Min.Y, Max.Z),
        FVector(Max.X, Min.Y, Max.Z),
        FVector(Min.X, Max.Y, Max.Z),
        FVector(Max.X, Max.Y, Max.Z)
    };

    // Draw the lines to create the box
    PDI->DrawLine(Vertices[0], Vertices[1], Color, SDPG_World);
    PDI->DrawLine(Vertices[1], Vertices[3], Color, SDPG_World);
    PDI->DrawLine(Vertices[3], Vertices[2], Color, SDPG_World);
    PDI->DrawLine(Vertices[2], Vertices[0], Color, SDPG_World);

    PDI->DrawLine(Vertices[4], Vertices[5], Color, SDPG_World);
    PDI->DrawLine(Vertices[5], Vertices[7], Color, SDPG_World);
    PDI->DrawLine(Vertices[7], Vertices[6], Color, SDPG_World);
    PDI->DrawLine(Vertices[6], Vertices[4], Color, SDPG_World);

    PDI->DrawLine(Vertices[0], Vertices[4], Color, SDPG_World);
    PDI->DrawLine(Vertices[1], Vertices[5], Color, SDPG_World);
    PDI->DrawLine(Vertices[2], Vertices[6], Color, SDPG_World);
    PDI->DrawLine(Vertices[3], Vertices[7], Color, SDPG_World);
}
