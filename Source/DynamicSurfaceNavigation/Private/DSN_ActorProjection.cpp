// Copyright Pagan Games. All rights reserved.

#include "DSN_ActorProjection.h"
#include <typeinfo>
#include "Math/Vector.h"
#include "Engine/World.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Navigation/NavLinkProxy.h"
#include "DSN_NavLinkGhost.h"
#include <stdexcept>

#include "DSN_NavLinkProjector.h"
#include "NavLinkCustomComponent.h"

// Sets default values
ADSN_ActorProjection::ADSN_ActorProjection()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a new scene component and set it as the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MyRootComponent"));
}

void ADSN_ActorProjection::Init(AActor* originalActor)
{
	FString ActorName = "DSN_Projection_" + originalActor->GetName();
	SetActorLabel(*ActorName);
	FTransform ActorTransform = originalActor->GetActorTransform();
	CopyActor(originalActor, ActorTransform, false);
}

void ADSN_ActorProjection::CopyActor(AActor* Actor, FTransform ActorTransform, bool bChild)
{
	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* ActorComponent : Components)
	{
		if(ActorComponent->IsA(UStaticMeshComponent::StaticClass()))
		{				
			AddStaticMeshComponent(Cast<UStaticMeshComponent>(ActorComponent), ActorTransform, bChild);
		}
		
		
		if(ActorComponent->IsA(UChildActorComponent::StaticClass()))
		{
			auto ChildActor = Cast<UChildActorComponent>(ActorComponent)->GetChildActor();

			//TODO MAKE&USE interface IDSN_Projectable 
			if(ChildActor->IsA(ADSN_NavLinkProjector::StaticClass()))
			{
				Cast<ADSN_NavLinkProjector>(ChildActor)->ProjectTo(ActorTransform, GetTransform());
			}
			
			CopyActor(ChildActor, ActorTransform, true);
		}
		
	}

}

// Called when the game starts or when spawned
void ADSN_ActorProjection::BeginPlay()
{
	Super::BeginPlay();
}

void ADSN_ActorProjection::AddStaticMeshComponent(UStaticMeshComponent* StaticMeshComponent, FTransform ActorTransform, bool bChild)
{
	UStaticMeshComponent* NewMeshComponent = NewObject<UStaticMeshComponent>(this);
	NewMeshComponent->RegisterComponent();
	NewMeshComponent->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
	NewMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetIncludingScale);

	// set relative transform if is not root/ do not check for child actors
	if (!IsRootComponent(StaticMeshComponent) || bChild)
	{
		auto ComponentTransform = StaticMeshComponent->GetComponentTransform();
		auto RelativeTransform = ComponentTransform.GetRelativeTransform(ActorTransform);

		NewMeshComponent->SetRelativeTransform(RelativeTransform);
	}

	// set up collision
	NewMeshComponent->CanCharacterStepUpOn = StaticMeshComponent->CanCharacterStepUpOn;
	NewMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	NewMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	NewMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
}

bool ADSN_ActorProjection::IsRootComponent(UStaticMeshComponent* StaticMeshComponent)
{
	return StaticMeshComponent->IsRegistered() && StaticMeshComponent->GetAttachParent() == nullptr;
}

