// Copyright Pagan Games. All rights reserved.

#include "VirtualSurfaceActor.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Components/BrushComponent.h"
#include "Builders/CubeBuilder.h"
#include <typeinfo>
#include "Math/Vector.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "Engine/World.h"
#include "AI/NavigationSystemBase.h"
#include "NavMesh/RecastNavMesh.h"
#include "AI/Navigation/NavigationTypes.h"
#include <stdexcept>

// Sets default values
AVirtualSurfaceActor::AVirtualSurfaceActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Create a new scene component and set it as the root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MyRootComponent"));
}

void AVirtualSurfaceActor::Init(AActor *realSurfaceActor)
{
    FString ActorName = realSurfaceActor->GetName() + "_VirtualSurfaceActor";
    SetActorLabel(*ActorName);
    // Copy static meshes:
    TArray<UStaticMeshComponent *> StaticMeshComponents;
    realSurfaceActor->GetComponents(StaticMeshComponents);
    FTransform ActorTransform = realSurfaceActor->GetActorTransform();
    for (UStaticMeshComponent *StaticMeshComponent : StaticMeshComponents)
    {
        AddStaticMeshComponent(StaticMeshComponent, ActorTransform);
    }
}

// Called when the game starts or when spawned
void AVirtualSurfaceActor::BeginPlay()
{
    Super::BeginPlay();
}

void AVirtualSurfaceActor::AddStaticMeshComponent(UStaticMeshComponent *StaticMeshComponent, FTransform ActorTransform)
{
    UStaticMeshComponent *NewMeshComponent = NewObject<UStaticMeshComponent>(this);
    NewMeshComponent->RegisterComponent();
    NewMeshComponent->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
    NewMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

    // set relative transform if is not root
    if (!IsRootComponent(StaticMeshComponent))
    {
        auto ComponentTransform = StaticMeshComponent->GetComponentTransform();
        auto RelativeTransform = ComponentTransform.GetRelativeTransform(ActorTransform);

        NewMeshComponent->SetRelativeTransform(RelativeTransform);
    }

    // set up collision
    NewMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    NewMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    NewMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

bool AVirtualSurfaceActor::IsRootComponent(UStaticMeshComponent *StaticMeshComponent)
{
    return StaticMeshComponent->IsRegistered() && StaticMeshComponent->GetAttachParent() == nullptr;
}
