// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicNavSurfaceComponent.h"
#include "NavMesh/RecastNavMesh.h"
#include "VirtualSurfaceActor.h"
#include "NavigationSystem.h"

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


    // Create an instance of AVirtualSurfaceActor with a name of "VirtualSurfaceActor"
    VirtualSurfaceActor = GetWorld()->SpawnActor<AVirtualSurfaceActor>(
        AVirtualSurfaceActor::StaticClass(),
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        FActorSpawnParameters()
    );

    FString ActorName = GetOwner()->GetName() + "_VirtualSurfaceActor";
    VirtualSurfaceActor->Rename(*ActorName, nullptr, REN_DontCreateRedirectors);

    VirtualSurfaceActor->VirtualArea = VirtualArea;  

    VirtualSurfaceActor->Init(GetOwner());
}



// Called every frame
void UDynamicNavSurfaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FTransform UDynamicNavSurfaceComponent::TransformWorld2Virtual(FTransform WorldTransform,  bool KeepUpDirection) const
{
    
    if(VirtualSurfaceActor == nullptr)
    {
        return WorldTransform;
    }
    FTransform LocalTransform = WorldTransform.GetRelativeTransform(GetOwner()->GetActorTransform());

    FTransform ResultTransform = LocalTransform * VirtualSurfaceActor->GetTransform();
    if(KeepUpDirection)
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

FTransform UDynamicNavSurfaceComponent::TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection) const
{
    if(VirtualSurfaceActor == nullptr)
    {
        return VirtualTransform;
    }
    FTransform LocalTransform = VirtualTransform.GetRelativeTransform(VirtualSurfaceActor->GetActorTransform());

    FTransform ResultTransform = LocalTransform * GetOwner()->GetTransform();
    if(RestoreUpDirection)
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

FVector UDynamicNavSurfaceComponent::TransformDirectionWorld2Virtual(FVector WorldDirection) const
{
    if(VirtualSurfaceActor == nullptr)
    {
        return WorldDirection;
    }
    //auto localVector = GetOwner()->GetTransform().InverseTransformVector(WorldDirection);
    //return VirtualSurfaceActor->GetTransform().TransformVector(localVector);

    
    FTransform T1 = GetOwner()->GetTransform();
    T1.SetScale3D(FVector::OneVector);//Ignore scaling
    auto localVector = T1.InverseTransformVector(WorldDirection);
    FTransform T2 = VirtualSurfaceActor->GetTransform();
    T2.SetScale3D(FVector::OneVector);//Ignore scaling
    return T2.TransformVector(localVector);
}

FVector UDynamicNavSurfaceComponent::TransformDirectionVirtual2World(FVector VirtualDirection) const
{
    if(VirtualSurfaceActor == nullptr)
    {
        return VirtualDirection;
    }
    FTransform T1 = VirtualSurfaceActor->GetTransform();
    T1.SetScale3D(FVector::OneVector);//Ignore scaling
    auto localVector = T1.InverseTransformVector(VirtualDirection);		
    FTransform T2 = GetOwner()->GetTransform();
    T2.SetScale3D(FVector::OneVector);//Ignore scaling
    return T2.TransformVector(localVector);
}

FVector UDynamicNavSurfaceComponent::TransformPositionWorld2Virtual(FVector WorldPosition) const
{
    if(VirtualSurfaceActor == nullptr)
    {
        return WorldPosition;
    }
    auto localPosition = GetOwner()->GetTransform().InverseTransformPosition(WorldPosition);
    return VirtualSurfaceActor->GetTransform().TransformPosition(localPosition);
}

FVector UDynamicNavSurfaceComponent::GetVelocityAtPosition(FVector WorldPosition) const
{
    UPrimitiveComponent* StaticMeshComponent = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());

    if (StaticMeshComponent)
    {
        // Identify the specific spot on the static mesh (e.g., a location relative to the origin)


        FVector SpotVelocity = StaticMeshComponent->GetPhysicsLinearVelocityAtPoint(WorldPosition);

        return SpotVelocity;
    }
    UE_LOG(LogTemp, Error, TEXT("Actor with dynamic surface component should have UPrimitiveComponent root component."));

    return FVector::ZeroVector;
}
