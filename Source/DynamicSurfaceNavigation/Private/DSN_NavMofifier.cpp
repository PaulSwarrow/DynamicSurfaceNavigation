// Copyright Pagan Games, All rights reserved.


#include "DSN_NavMofifier.h"
#include "GameFramework/Actor.h"
#include "DynamicNavSurfaceComponent.h"
#include "DSN_NavModifierProjection.h"
#include "Components/SceneComponent.h"

UDSN_NavMofifier::UDSN_NavMofifier()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UDSN_NavMofifier::BeginPlay()
{
    Super::BeginPlay();
    OnComponentBeginOverlap.AddDynamic(this, &UDSN_NavMofifier::HandleComponentBeginOverlap);
	OnComponentEndOverlap.AddDynamic(this, &UDSN_NavMofifier::HandleComponentEndOverlap);

	TSet<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors);
	for(const auto &element : OverlappedActors)
    {
		HandleComponentBeginOverlap(nullptr, element, nullptr, 0, false, FHitResult());
	}

	StartProjection(GetOwner()->GetComponentByClass<UDynamicNavSurfaceComponent>());
}

void UDSN_NavMofifier::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
	OnComponentBeginOverlap.RemoveAll(this);
	OnComponentEndOverlap.RemoveAll(this);
}


void UDSN_NavMofifier::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	for (const auto &element : Projections)
	{
		SyncProjectionPosition(element.first, element.second);
	}
}

void UDSN_NavMofifier::HandleComponentBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    if (OtherActor == nullptr)
	{
		return;
	}
	auto Surface = OtherActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
	StartProjection(Surface);
}

void UDSN_NavMofifier::HandleComponentEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
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

void UDSN_NavMofifier::StartProjection(UDynamicNavSurfaceComponent *Surface)
{
	if (Surface != nullptr)
	{
		if (Projections.find(Surface) == Projections.end())
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform WorldTransform = GetComponentTransform();
			UE_LOG(LogTemp, Warning, TEXT("WorldTransform: %s"), *WorldTransform.GetLocation().ToString());
			auto VirtualTransform = Surface->TransformWorld2Virtual(WorldTransform, false);
			UE_LOG(LogTemp, Warning, TEXT("VirtualTransform: %s"), *VirtualTransform.GetLocation().ToString());

			auto Projection = GetWorld()->SpawnActor<ADSN_NavModifierProjection>(
				ADSN_NavModifierProjection::StaticClass(),
				VirtualTransform,
				SpawnParams);
			Projection->Init(NavAreaClass, BoxExtent);
			USceneComponent* RootComponent = Projection->GetRootComponent();
			if (RootComponent)
			{
				RootComponent->SetMobility(EComponentMobility::Movable);
			}
			Projections[Surface] = Projection;

		}
	}
}

void UDSN_NavMofifier::SyncProjectionPosition(UDynamicNavSurfaceComponent *Surface, AActor *Projection)
{
    auto VirtualTransform = Surface->TransformWorld2Virtual(GetComponentTransform(), false);
	auto CurrentTransform = Projection->GetTransform();
	float AngularDelta = FMath::RadiansToDegrees(CurrentTransform.GetRotation().AngularDistance(VirtualTransform.GetRotation()));
	float LinearDelta = FVector::Distance(CurrentTransform.GetLocation(), VirtualTransform.GetLocation());
	float ScaleDelta = FVector::Distance(CurrentTransform.GetScale3D(), VirtualTransform.GetScale3D());
	if(AngularDelta > AgulatMinStep || LinearDelta > LinearMinStep || ScaleDelta > ScaleMinStep)
	{
		Projection->SetActorTransform(VirtualTransform);
	}
}
