// Copyright Pagan Games, All rights reserved.

#include "DSN_FunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "DSN_Marker.h"
#include "DSN_MagneticBoots.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Pawn.h"

void UDSN_FunctionLibrary::CreateNavigationTaskData(APawn *Pawn, const FVector GoalLocation, AActor *GoalActor, APawn *&PawnToUse, FVector &TargetLocation, AActor *&TargetActor)
{
    if (GoalActor != nullptr)
    {
        // replace actor with a marker:
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        auto Marker = Pawn->GetWorld()->SpawnActor<ADSN_Marker>(
            ADSN_Marker::StaticClass(),
            GoalActor->GetTransform(),
            SpawnParams);
        Marker->SyncTarget = GoalActor;
        Marker->ProjectionRadius = 105.f;
        Marker->InitializeMarker();

        TargetLocation = Marker->GetActorLocation();
        TargetActor = Marker;
    }
    else
    {
        TargetActor = nullptr;
        TargetLocation = GoalLocation;

        // replace location with a virtual one
        TArray<AActor *> OverlappedActors;
        TArray<AActor *> IgnoreActors;
        IgnoreActors.Add(Pawn);
        // TODO add Pawn to ignore actors
        TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
        TSubclassOf<AActor> ClassFilter = nullptr;
        UKismetSystemLibrary::SphereOverlapActors(Pawn->GetWorld(), GoalLocation, 105.f, traceObjectTypes, ClassFilter, TArray<AActor *>(), OverlappedActors);

        for (auto Actor : OverlappedActors)
        {
            if (Actor != nullptr)
            {
                auto Surface = Actor->GetComponentByClass<UDynamicNavSurfaceComponent>();
                if (Surface != nullptr)
                {
                    TargetLocation = Surface->TransformPositionWorld2Virtual(Actor->GetActorLocation());
                    break;
                }
            }
        }
    }

    // select pawn or ghost pawn
    PawnToUse = Pawn;
    auto Boots = Pawn->GetComponentByClass<UDSN_MagneticBoots>();
    if (Boots != nullptr && Boots->HasDynamicSurface())
    {
        PawnToUse = Boots->GetGhost();
    }
}
