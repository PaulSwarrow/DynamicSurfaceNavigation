// Copyright Pagan Games, All rights reserved.

#include "DSN_FunctionLibrary.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "DSN_Marker.h"
#include "DSN_MagneticBoots.h"
#include "NavigationSystem.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI/Navigation/NavigationTypes.h"
#include "GameFramework/Pawn.h"

FDSN_Point UDSN_FunctionLibrary::DSN_ParseLocation(UObject *WorldContextObject, FVector origin)
{
    FDSN_Point point;
    point.Location = origin;

    const float ActorHalfHeight = 100.f; // slightly greater than real character bounds
    const float ActorR = 40.f;           // slightly greater than real character bounds
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World)
    {
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PhysicsBody));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Destructible));

        TArray<AActor *> IgnoreActors;
        TArray<FHitResult> HitResults;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActors(IgnoreActors);

        FVector Target = FVector(origin.X, origin.Y, origin.Z - ActorHalfHeight);
        if (World->SweepMultiByObjectType(HitResults, origin, Target, FQuat::Identity, FCollisionObjectQueryParams(ObjectTypes), FCollisionShape::MakeSphere(ActorR), QueryParams))
        {
            for (const FHitResult &HitResult : HitResults)
            {
                AActor *HitActor = HitResult.GetActor();
                if (HitActor)
                {
                    auto Surface = HitActor->FindComponentByClass<UDynamicNavSurfaceComponent>();
                    if (Surface != nullptr)
                    {
                        point.Surface = Surface;
                        point.Location = Surface->TransformPositionWorld2Virtual(HitResult.Location);
                        break;
                    }
                }
            }
        }
    }
    return point;
}

bool UDSN_FunctionLibrary::DSN_FindPointInNavMesh(UObject *WorldContextObject, FVector origin, float radius, bool reachable, FDSN_Point &point)
{
    point = FDSN_Point();
    UWorld *World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    if (World)
    {
        UNavigationSystemV1 *NavSystem = UNavigationSystemV1::GetCurrent(World);
        if (NavSystem)
        {
            auto dsn_origin = DSN_ParseLocation(World, origin);
            point.Surface = dsn_origin.Surface;

            FNavLocation result;
            //UKismetSystemLibrary::DrawDebugSphere(WorldContextObject, dsn_origin.GetNavLocation(), radius, 12, dsn_origin.IsVirtual() ? FColor::Blue : FColor::Green, 0.1f, 3.f);
            if (reachable)
            {
                if (NavSystem->GetRandomReachablePointInRadius(dsn_origin.GetNavLocation(), radius, result))
                {
                    point.Location = result.Location;
                    return true;
                }
            }
            else
            {
                if (NavSystem->GetRandomPointInNavigableRadius(dsn_origin.GetNavLocation(), radius, result))
                {
                    point.Location = result.Location;
                    return true;
                }
            }
        }
    }
    return false;
}

void UDSN_FunctionLibrary::DSN_StopMovement(APawn *Pawn)
{
    auto Boots = Pawn->GetComponentByClass<UDSN_MagneticBoots>();
    if (Boots != nullptr && Boots->HasDynamicSurface())
    {
        Boots->GetGhostController()->StopMovement();
    }
    else
    {
        Cast<AAIController>(Pawn->GetController())->StopMovement();
    }
}

void UDSN_FunctionLibrary::CreateNavigationTaskData(APawn *Pawn, const FVector GoalLocation, AActor *GoalActor, bool ConvertLocation, APawn *&PawnToUse, FVector &TargetLocation, AActor *&TargetActor)
{
    auto World = Pawn->GetWorld();
    if (GoalActor != nullptr)
    {
        // replace actor with a marker:
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        auto Marker = World->SpawnActor<ADSN_Marker>(
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
        TargetLocation = ConvertLocation ? DSN_ParseLocation(Pawn, GoalLocation).GetNavLocation() : GoalLocation;
    }

    // select pawn or ghost pawn
    PawnToUse = Pawn;
    auto Boots = Pawn->GetComponentByClass<UDSN_MagneticBoots>();
    if (Boots != nullptr && Boots->HasDynamicSurface())
    {
        PawnToUse = Boots->GetGhost();
    }
}
