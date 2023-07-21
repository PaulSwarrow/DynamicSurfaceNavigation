// Copyright Pagan Games, All rights reserved.

#include "DSN_ActorProjector.h"
#include "GameFramework/Actor.h"
#include "DynamicNavSurfaceComponent.h"
#include "Components/SceneComponent.h"

// Sets default values for this component's properties
UDSN_ActorProjector::UDSN_ActorProjector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UDSN_ActorProjector::BeginPlay()
{
	Super::BeginPlay();
	
	OnComponentBeginOverlap.AddDynamic(this, &UDSN_ActorProjector::HandleComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UDSN_ActorProjector::HandleComponentEndOverlap);

	//get all overlapped actors and pass them to HandleComponentBeginOverlap
	// ...
	
	TSet<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors);
	for(const auto &element : OverlappedActors){
		HandleComponentBeginOverlap(nullptr, element, nullptr, 0, false, FHitResult());
	}
}

void UDSN_ActorProjector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	OnComponentBeginOverlap.RemoveAll(this);
	OnComponentEndOverlap.RemoveAll(this);
}

// Called every frame
void UDSN_ActorProjector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (const auto &element : Projections)
	{
		SyncProjectionPosition(element.first, element.second);
	}
	// ...
}

void UDSN_ActorProjector::HandleComponentBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}
	auto Surface = OtherActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
	if (Surface != nullptr)
	{
		if (Projections.find(Surface) == Projections.end())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			auto VirtualTransform = Surface->TransformWorld2Virtual(GetOwner()->GetTransform(), false);

			auto Projection = GetWorld()->SpawnActor<ADSN_ActorProjection>(
				ADSN_ActorProjection::StaticClass(),
				VirtualTransform,
				SpawnParams);
			Projection->Init(GetOwner());
			USceneComponent* RootComponent = Projection->GetRootComponent();
			if (RootComponent)
			{
				RootComponent->SetMobility(EComponentMobility::Movable);
			}
			Projections[Surface] = Projection;

		}
	}
}

void UDSN_ActorProjector::HandleComponentEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr)
	{
		return;
	}
	auto Surface = OtherActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
	if (Surface != nullptr)
	{
		if (Projections.find(Surface) != Projections.end())
		{
			Projections[Surface]->Destroy();
			Projections.erase(Surface);
		}
	}
}

void UDSN_ActorProjector::SyncProjectionPosition(UDynamicNavSurfaceComponent *Surface, AActor *Projection)
{
	auto VirtualTransform = Surface->TransformWorld2Virtual(GetOwner()->GetTransform(), false);
	auto CurrentTransform = Projection->GetTransform();
	float AngularDelta = FMath::RadiansToDegrees(CurrentTransform.GetRotation().AngularDistance(VirtualTransform.GetRotation()));
	float LinearDelta = FVector::Distance(CurrentTransform.GetLocation(), VirtualTransform.GetLocation());
	if(AngularDelta > AgulatMinStep || LinearDelta > LinearMinStep)
	{
		Projection->SetActorTransform(VirtualTransform);
	}
}
