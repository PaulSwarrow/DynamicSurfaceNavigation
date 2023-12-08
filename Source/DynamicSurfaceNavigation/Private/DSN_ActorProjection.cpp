// Copyright Pagan Games. All rights reserved.

#include "DSN_ActorProjection.h"
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
#include "NavModifierComponent.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Navigation/NavLinkProxy.h"
#include "DSN_NavLinkGhost.h"
#include <stdexcept>

// Sets default values
ADSN_ActorProjection::ADSN_ActorProjection()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    // Create a new scene component and set it as the root component
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MyRootComponent"));
}

void ADSN_ActorProjection::Init(AActor *originalActor)
{
    FString ActorName = "DSN_Projection_" + originalActor->GetName();
    SetActorLabel(*ActorName);
    // Copy static meshes:
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    originalActor->GetComponents(StaticMeshComponents);
    FTransform ActorTransform = originalActor->GetActorTransform();
    for (UStaticMeshComponent* StaticMeshComponent : StaticMeshComponents)
    {
        AddStaticMeshComponent(StaticMeshComponent, ActorTransform);
    }
    //copy child components if they are nav link proxies
    TArray<UChildActorComponent*> ChildComponents;
    originalActor->GetComponents(ChildComponents);
    for (UChildActorComponent* ChildComponent : ChildComponents)
    {
        auto childActor = ChildComponent->GetChildActor();
        if (childActor->IsA(ANavLinkProxy::StaticClass()))
        {
            CopyNavLink(Cast<ANavLinkProxy>(childActor), ActorTransform);
        }
    }

}

// Called when the game starts or when spawned
void ADSN_ActorProjection::BeginPlay()
{
    Super::BeginPlay();
}

void ADSN_ActorProjection::AddStaticMeshComponent(UStaticMeshComponent *StaticMeshComponent, FTransform ActorTransform)
{
    UStaticMeshComponent *NewMeshComponent = NewObject<UStaticMeshComponent>(this);
    NewMeshComponent->RegisterComponent();
    NewMeshComponent->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
    NewMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

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

bool ADSN_ActorProjection::IsRootComponent(UStaticMeshComponent *StaticMeshComponent)
{
    return StaticMeshComponent->IsRegistered() && StaticMeshComponent->GetAttachParent() == nullptr;
}

void ADSN_ActorProjection::CopyNavLink(ANavLinkProxy *originalNavLinkProxy, FTransform ActorTransform)
{
    // Register the new NavLinkProxy
    UChildActorComponent* ChildActorComponent = NewObject<UChildActorComponent>(this);
    ChildActorComponent->SetChildActorClass(ADSN_NavLinkGhost::StaticClass());
    ChildActorComponent->CreateChildActor();

    // Get the NavLinkProxy from the Child Actor Component
    ADSN_NavLinkGhost* newNavLinkProxy = Cast<ADSN_NavLinkGhost>(ChildActorComponent->GetChildActor());

    // Attach the Child Actor Component to the root component
    ChildActorComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

    auto originalTransform = originalNavLinkProxy->GetActorTransform();
    auto relativeTransform = originalTransform.GetRelativeTransform(ActorTransform);
    ChildActorComponent->SetRelativeTransform(relativeTransform);

    // Copy properties from the original NavLinkProxy to the new one
    /*newNavLinkProxy->SetSmartLink(originalNavLinkProxy->GetSmartLink());
    newNavLinkProxy->SetAreaClass(originalNavLinkProxy->GetAreaClass());
    newNavLinkProxy->SetSmartLinkEnabled(originalNavLinkProxy->IsSmartLinkEnabled());
    newNavLinkProxy->SetEnabled(originalNavLinkProxy->IsEnabled());
    newNavLinkProxy->SetSmartLinkCompName(originalNavLinkProxy->GetSmartLinkCompName());
    newNavLinkProxy->SetAbsolute(false, false, false);*/
    newNavLinkProxy->SetActorLabel(*FString("DSN_Projection_" + originalNavLinkProxy->GetName()));
    
    // Modify the in and out points of the new NavLinkProxy
    auto newInPoint = originalNavLinkProxy->PointLinks[0].Left;
    auto newOutPoint = originalNavLinkProxy->PointLinks[0].Right;

    newNavLinkProxy->PointLinks[0].Left = newInPoint;
    newNavLinkProxy->PointLinks[0].Right = newOutPoint;
}
