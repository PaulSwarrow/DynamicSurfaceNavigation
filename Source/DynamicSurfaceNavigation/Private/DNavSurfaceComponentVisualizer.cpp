// Copyright Pagan Games. All rights reserved.

#include "DNavSurfaceComponentVisualizer.h"
#include "DynamicNavSurfaceComponent.h"
#include "NavMesh/RecastNavMesh.h"
#include "VirtualSurfaceActor.h"
#include "NavigationSystem.h"
#include "CoreMinimal.h"
#include "LocalVertexFactory.h"

void DNavSurfaceComponentVisualizer::DrawVisualization(const UActorComponent *Component, const FSceneView *View, FPrimitiveDrawInterface *PDI)
{
    const UDynamicNavSurfaceComponent *Surface = Cast<UDynamicNavSurfaceComponent>(Component);
    const AVirtualSurfaceActor *VirtualSurface = Surface->VirtualSurfaceActor;

    auto navsys = Surface->GetWorld()->GetNavigationSystem();
    if (navsys == nullptr || VirtualSurface == nullptr)
    {
        return;
    }
    auto navmesh = Cast<ARecastNavMesh>(navsys->GetMainNavData());
    auto TileSize = navmesh->TileSizeUU;
    //
    int32 TileX;
    int32 TileY;
    TArray<int32> TileIndices;
    navmesh->GetNavMeshTileXY(VirtualSurface->GetActorLocation(), TileX, TileY);
    // bool HasTile = navmesh->GetNavMeshTileXY(FVector::ZeroVector, TileX, TileY);
    navmesh->GetNavMeshTilesAt(TileX, TileY, TileIndices);
    navmesh->GetNavMeshTilesAt(TileX + 1, TileY, TileIndices);
    navmesh->GetNavMeshTilesAt(TileX - 1, TileY, TileIndices);
    navmesh->GetNavMeshTilesAt(TileX, TileY + 1, TileIndices);
    navmesh->GetNavMeshTilesAt(TileX, TileY - 1, TileIndices);
    if (TileIndices.Num() == 0)
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
            /* code */

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
    }
}

FVector DNavSurfaceComponentVisualizer::Virtual2WorldPosition(FVector position, const UDynamicNavSurfaceComponent *surface)
{
    auto local = surface->VirtualSurfaceActor->GetTransform().InverseTransformPosition(position);
    auto world = surface->GetOwner()->GetTransform().TransformPosition(local);
    return world;
}
