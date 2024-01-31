// Fill out your copyright notice in the Description page of Project Settings.

#include "DSN_NavLinkProjector.h"
#include "DSN_MagneticBoots.h"
#include "DSN_NavLinkGhost.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/PathFollowingComponent.h"

// Sets default values
ADSN_NavLinkProjector::ADSN_NavLinkProjector()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

	// Initialize Points A and B
	PointA = FVector::ZeroVector;
	PointB = FVector::ZeroVector;
}

void ADSN_NavLinkProjector::OnNavLinkEnter_Implementation(AActor* Agent)
{
	
}

void ADSN_NavLinkProjector::ProjectTo(FTransform Origin, FTransform Target)
{	
	auto Relative = GetTransform().GetRelativeTransform(Origin);
	auto Virtual = Relative * Target;
	ProjectNavLink(this, Virtual);
}

// Called when the game starts or when spawned
void ADSN_NavLinkProjector::BeginPlay()
{
	Super::BeginPlay();
	UDynamicNavSurfaceComponent* Surface;
	if(TryFindParent(this, Surface))
	{
		//Wait for external ProjectTo call (Due to UE lifecycle)
	}
	else
	{
		ProjectNavLink(this, GetTransform());
	}
}

void ADSN_NavLinkProjector::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(GhostLink != nullptr)
	{		
		GhostLink->OnSmartLinkReachedEvent.RemoveDynamic(this, &ADSN_NavLinkProjector::HandleSmartLinkReached);
		GhostLink->Destroy();
		GhostLink = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}


bool ADSN_NavLinkProjector::TryFindParent(AActor* StartActor, UDynamicNavSurfaceComponent*& Surface)
{
	if (!StartActor)
	{
		return false;
	}

	AActor* ParentActor = StartActor->GetParentActor();
	if (ParentActor)
	{
		Surface = ParentActor->GetComponentByClass<UDynamicNavSurfaceComponent>();
		if (Surface != nullptr)
		{
			return true;
		}
		return TryFindParent(ParentActor, Surface);
	}

	return false;
}

// Called every frame
void ADSN_NavLinkProjector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Ensure we're in the editor
#if WITH_EDITOR
	if (GEngine && GetWorld() && GetWorld()->WorldType == EWorldType::Editor)
	{
		DrawGizmos();
	}
#endif
}


void ADSN_NavLinkProjector::ProjectNavLink(ADSN_NavLinkProjector* Projector, FTransform ActorTransform)
{
	//Get world context
	UWorld* World = GetWorld();
	if (!World) return;

	ADSN_NavLinkGhost* newNavLinkProxy = World->SpawnActor<ADSN_NavLinkGhost>(ADSN_NavLinkGhost::StaticClass());
	newNavLinkProxy->SetActorTransform(ActorTransform);

	newNavLinkProxy->SetActorLabel(*FString("DSN_Projection_" + Projector->GetName()));

	// Modify the in and out points of the new NavLinkProxy
	auto newInPoint = Projector->PointA;
	auto newOutPoint = Projector->PointB;
	
	newNavLinkProxy->PointLinks[0].Left = newInPoint;
	newNavLinkProxy->PointLinks[0].Right = newOutPoint;

	newNavLinkProxy->GetSmartLinkComp()->SetLinkData(newInPoint, newOutPoint, ENavLinkDirection::BothWays);
	GhostLink = newNavLinkProxy;
	
	GhostLink->OnSmartLinkReachedEvent.AddDynamic(this, &ADSN_NavLinkProjector::HandleSmartLinkReached);
}

void ADSN_NavLinkProjector::HandleSmartLinkReached(AActor* Actor, const FVector& Vector)
{
	auto Agent = Actor;
	if(Agent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("NavLink Reached: Null ACTOR!"));
		return;
	}
	if(Actor->IsA(ADSN_Ghost::StaticClass()))
	{
		Agent = Cast<ADSN_Ghost>(Actor)->Origin;	
	}
	
	auto boots = Agent->GetComponentByClass<UDSN_MagneticBoots>();
	if(boots != nullptr)
	{
		boots->PauseMovement();
	}
	
	OnNavLinkEnter(Agent);
}

void ADSN_NavLinkProjector::ExitNavLink(AActor* Agent)
{
	auto boots = Agent->GetComponentByClass<UDSN_MagneticBoots>();
	if(boots != nullptr)
	{
		boots->ResumeMovement();
	} else if(Agent->IsA(APawn::StaticClass()))
	{
		//TODO more checks!
		auto Controller = Cast<ACharacter>(Agent)->GetController();
		Cast<AAIController>(Controller)->GetPathFollowingComponent()->ResumeMove();
	}
}

void ADSN_NavLinkProjector::DrawGizmos()
{
#if WITH_EDITOR
	if (GEngine)
	{
		auto Transform = GetTransform();
		auto A = Transform.TransformPosition(PointA);
		auto B = Transform.TransformPosition(PointB);
		// Draw spheres at Points A and B
		DrawDebugSphere(GetWorld(), A, 10.0f, 12, FColor::Red, false, -1.0f, 0, 1.0f);
		DrawDebugSphere(GetWorld(), B, 10.0f, 12, FColor::Blue, false, -1.0f, 0, 1.0f);

		// Draw a line between Points A and B
		DrawDebugLine(GetWorld(), A, B, FColor::Yellow, false, -1.0f, 0, 1.0f);
	}
#endif
}
