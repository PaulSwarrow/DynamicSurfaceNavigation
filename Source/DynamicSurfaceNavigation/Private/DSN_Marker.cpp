// Copyright Pagan Games, All rights reserved.

#include "DSN_Marker.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"

// Sets default values
ADSN_Marker::ADSN_Marker()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create the root sphere component
    RootSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    RootComponent = RootSphereComponent;
    // Set up collision settings
    RootSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    //RootSphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1); // Custom object type for navigation markers
    RootSphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // Allow overlapping with other objects

}

// Called when the game starts or when spawned
void ADSN_Marker::BeginPlay()
{
	Super::BeginPlay();
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

void ADSN_Marker::InitializeMarker()
{
	// Create a new USphereComponent instance and add it to the actor
	TargetCollider = NewObject<USphereComponent>(SyncTarget, USphereComponent::StaticClass());
	SyncTarget->AddOwnedComponent(TargetCollider);
	TargetCollider->RegisterComponent(); // You need to register the new component
	TargetCollider->AttachToComponent(SyncTarget->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
	TargetCollider->InitSphereRadius(ProjectionRadius);

	//TODO draw debug sphere matching TargetCollider radius and position
	//UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetCollider->GetComponentLocation(), TargetCollider->GetScaledSphereRadius(), 12, FColor::Red, 0.5f, 1.f);

	//TSet<AActor *> OverlappedActors;
	//TargetCollider->GetOverlappingActors(OverlappedActors, null);
	TArray<AActor *> OverlappedActors;
	TArray<AActor *> IgnoreActors;
	IgnoreActors.Add(SyncTarget);
	//TODO add Pawn to ignore actors
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	TSubclassOf<AActor> ClassFilter = nullptr;
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), TargetCollider->GetComponentLocation(), TargetCollider->GetScaledSphereRadius(), traceObjectTypes, ClassFilter, TArray<AActor *>(), OverlappedActors);

	for (const auto &element : OverlappedActors)
	{
		HandleComponentBeginOverlap(nullptr, element, nullptr, 0, false, FHitResult());
	}
	TargetCollider->OnComponentBeginOverlap.AddDynamic(this, &ADSN_Marker::HandleComponentBeginOverlap);
	TargetCollider->OnComponentEndOverlap.AddDynamic(this, &ADSN_Marker::HandleComponentEndOverlap);

	SetActorLabel("DSN_Marker_" + SyncTarget->GetName());
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
	if (CurrentSurface != nullptr && Surface == CurrentSurface)
	{
		SetSurface(nullptr);
	}
}

void ADSN_Marker::SetSurface(UDynamicNavSurfaceComponent *Surface)
{
	CurrentSurface = Surface;
}
