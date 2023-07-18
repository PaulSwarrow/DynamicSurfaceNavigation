// Copyright Pagan Games. All rights reserved.

#include "DynamicNavSurfaceComponent.h"
#include "NavMesh/RecastNavMesh.h"
#include "VirtualSurfaceActor.h"
#include "NavigationSystem.h"
#include "Kismet/GameplayStatics.h"
#include <unordered_set>

std::unordered_set<UDynamicNavSurfaceComponent *> UDynamicNavSurfaceComponent::RegisterQueue;
std::unordered_set<UDynamicNavSurfaceComponent *> UDynamicNavSurfaceComponent::UnregisterQueue;

// Sets default values for this component's properties
UDynamicNavSurfaceComponent::UDynamicNavSurfaceComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UDynamicNavSurfaceComponent::BeginPlay()
{
    Super::BeginPlay();
    // TODO fined AVirtualNAvMeshArea instance
    UnregisterQueue.erase(this);
    RegisterQueue.insert(this);
}

void UDynamicNavSurfaceComponent::LazyInit()
{
}

void UDynamicNavSurfaceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterQueue.insert(this);
    RegisterQueue.erase(this);
}

FTransform UDynamicNavSurfaceComponent::TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection)
{
    LazyInit();
    if (VirtualSurfaceActor == nullptr)
    {
        return WorldTransform;
    }
    FTransform LocalTransform = WorldTransform.GetRelativeTransform(GetOwner()->GetActorTransform());

    FTransform ResultTransform = LocalTransform * VirtualSurfaceActor->GetTransform();
    if (KeepUpDirection)
    {
        FRotator Rotation = ResultTransform.Rotator();
        float YawRotation = Rotation.Yaw;

        // Calculate the new rotation by keeping the yaw and making the pitch and roll zero
        FRotator NewRotation(0.0f, YawRotation, 0.0f);

        // Create a new transform with the aligned rotation and the original translation and scale
        ResultTransform = FTransform(NewRotation, ResultTransform.GetLocation(), ResultTransform.GetScale3D());
    }

    return ResultTransform;
}

FTransform UDynamicNavSurfaceComponent::TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection)
{
    LazyInit();
    if (VirtualSurfaceActor == nullptr)
    {
        return VirtualTransform;
    }
    FTransform LocalTransform = VirtualTransform.GetRelativeTransform(VirtualSurfaceActor->GetActorTransform());

    FTransform ResultTransform = LocalTransform * GetOwner()->GetTransform();
    if (RestoreUpDirection)
    {
        FRotator Rotation = ResultTransform.Rotator();
        float YawRotation = Rotation.Yaw;

        // Calculate the new rotation by keeping the yaw and making the pitch and roll zero
        FRotator NewRotation(0.0f, YawRotation, 0.0f);

        // Create a new transform with the aligned rotation and the original translation and scale
        ResultTransform = FTransform(NewRotation, ResultTransform.GetLocation(), ResultTransform.GetScale3D());
    }
    return ResultTransform;
}

FVector UDynamicNavSurfaceComponent::TransformDirectionWorld2Virtual(FVector WorldDirection)
{
    LazyInit();
    if (VirtualSurfaceActor == nullptr)
    {
        return WorldDirection;
    }
    // auto localVector = GetOwner()->GetTransform().InverseTransformVector(WorldDirection);
    // return VirtualSurfaceActor->GetTransform().TransformVector(localVector);

    FTransform T1 = GetOwner()->GetTransform();
    T1.SetScale3D(FVector::OneVector); // Ignore scaling
    auto localVector = T1.InverseTransformVector(WorldDirection);
    FTransform T2 = VirtualSurfaceActor->GetTransform();
    T2.SetScale3D(FVector::OneVector); // Ignore scaling
    return T2.TransformVector(localVector);
}

FVector UDynamicNavSurfaceComponent::TransformDirectionVirtual2World(FVector VirtualDirection)
{
    LazyInit();
    if (VirtualSurfaceActor == nullptr)
    {
        return VirtualDirection;
    }
    FTransform T1 = VirtualSurfaceActor->GetTransform();
    T1.SetScale3D(FVector::OneVector); // Ignore scaling
    auto localVector = T1.InverseTransformVector(VirtualDirection);
    FTransform T2 = GetOwner()->GetTransform();
    T2.SetScale3D(FVector::OneVector); // Ignore scaling
    return T2.TransformVector(localVector);
}

FVector UDynamicNavSurfaceComponent::TransformPositionWorld2Virtual(FVector WorldPosition)
{
    LazyInit();
    if (VirtualSurfaceActor == nullptr)
    {
        return WorldPosition;
    }
    auto localPosition = GetOwner()->GetTransform().InverseTransformPosition(WorldPosition);
    return VirtualSurfaceActor->GetTransform().TransformPosition(localPosition);
}

FVector UDynamicNavSurfaceComponent::GetVelocityAtPosition(FVector WorldPosition)
{
    LazyInit();
    UPrimitiveComponent *StaticMeshComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

    if (StaticMeshComponent)
    {
        // Identify the specific spot on the static mesh (e.g., a location relative to the origin)

        FVector SpotVelocity = StaticMeshComponent->GetPhysicsLinearVelocityAtPoint(WorldPosition);

        return SpotVelocity;
    }
    UE_LOG(LogTemp, Error, TEXT("Actor with dynamic surface component should have UPrimitiveComponent root component."));

    return FVector::ZeroVector;
}
