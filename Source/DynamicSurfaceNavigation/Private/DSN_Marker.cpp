// Copyright Pagan Games, All rights reserved.

#include "DSN_Marker.h"
#include "Components/SphereComponent.h"

// Sets default values
ADSN_Marker::ADSN_Marker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADSN_Marker::BeginPlay()
{
	Super::BeginPlay();
}

void ADSN_Marker::Initialize(AActor *Target, float Radius)
{
	// Create a new USphereComponent instance and add it to the actor
	TargetCollider = NewObject<USphereComponent>(Target, USphereComponent::StaticClass());
	Target->AddOwnedComponent(TargetCollider);
	TargetCollider->RegisterComponent(); // You need to register the new component
	TargetCollider->AttachToComponent(Target->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	TargetCollider->InitSphereRadius(Radius);

	TSet<AActor *> OverlappedActors;
	TargetCollider->GetOverlappingActors(OverlappedActors);
	for (const auto &element : OverlappedActors)
	{
		HandleComponentBeginOverlap(nullptr, element, nullptr, 0, false, FHitResult());
	}
	TargetCollider->OnComponentBeginOverlap.AddDynamic(this, &ADSN_Marker::HandleComponentBeginOverlap);
	TargetCollider->OnComponentEndOverlap.AddDynamic(this, &ADSN_Marker::HandleComponentEndOverlap);

	SetActorLabel("DSN_Marker_" + Target->GetName());
}

void ADSN_Marker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (TargetCollider != nullptr)
	{
		SetSurface(nullptr);
		TargetCollider->OnComponentBeginOverlap.RemoveDynamic(this, &ADSN_Marker::HandleComponentBeginOverlap);
		TargetCollider->OnComponentEndOverlap.RemoveDynamic(this, &ADSN_Marker::HandleComponentEndOverlap);
		TargetCollider->DestroyComponent();
		TargetCollider = nullptr;
	}
}

// Called every frame
void ADSN_Marker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SyncPosition();
}

void ADSN_Marker::SyncPosition()
{
	if (CurrentSurface != nullptr)
	{
		SetActorLocation(CurrentSurface->TransformPositionWorld2Virtual(TargetCollider->GetComponentLocation()));
	}
	else
	{
		SetActorLocation(TargetCollider->GetComponentLocation());
	}
}

void ADSN_Marker::HandleComponentBeginOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	if (OtherActor == nullptr)
	{
		return;
	}
	auto Surface = OtherActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
	if (Surface != nullptr)
	{
		SetSurface(Surface);
	}
}

void ADSN_Marker::HandleComponentEndOverlap(UPrimitiveComponent *OverlappedComponent, AActor *OtherActor, UPrimitiveComponent *OtherComp, int32 OtherBodyIndex)
{
	auto Surface = OtherActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
	if (Surface != nullptr && Surface != CurrentSurface)
	{
		SetSurface(nullptr);
	}
}

void ADSN_Marker::SetSurface(UDynamicNavSurfaceComponent *Surface)
{
	CurrentSurface = Surface;
}
