// Copyright Pagan Games. All rights reserved.

#include "StableBuoyancyComponent.h"

UStableBuoyancyComponent::UStableBuoyancyComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UStableBuoyancyComponent::BeginPlay()
{
    Super::BeginPlay();
    GeneratePantoons();
    if(FixCenterOfMass) {
        auto Root = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
        auto Center = GetOwner()->GetTransform().InverseTransformPosition(Root->GetCenterOfMass());
        UE_LOG(LogTemp, Warning, TEXT("OLD Center: %s"), *Center.ToString());
        //Center = Center;
        Root->SetCenterOfMass(FVector::ZeroVector);
        auto NewCenter = Root->GetCenterOfMass();
        NewCenter = GetOwner()->GetTransform().InverseTransformPosition(NewCenter);
        UE_LOG(LogTemp, Warning, TEXT("Local(?) Center: %s"), *NewCenter.ToString());
    }

    //TODO LOG Center

}

float UStableBuoyancyComponent::GetWaterLevel_Implementation(FVector WorldPosition) const
{
    return 0.0f;
}

void UStableBuoyancyComponent::GeneratePantoons()
{

    float SplineLength = GetSplineLength();
    float Increment = SplineLength / static_cast<float>(NumPoints - 1);

    for (int32 i = 0; i < NumPoints; i++)
    {
        float Distance = Increment * static_cast<float>(i);
        FVector Point = GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::Local);
        Pantons.Add(Point);
    }
}

void UStableBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // Get actor's physic body
    auto Root = Cast<UPrimitiveComponent>(GetOwner()->GetRootComponent());
    // Calculate the gravity force
    auto Gravity = GetWorld()->GetGravityZ();
    auto Mass = Root->GetMass();
    FVector GravityForce = FVector(0.0f, 0.0f, Gravity) * Mass;

    // Calculate the spring force based on the displacement
    float SpringConstant = Gravity * Mass / 2 * Buoyancy; // Adjust the spring constant as needed
    float Prediction = GravityForce.Z / (SpringConstant * DeltaTime);


    for (const FVector &Panton : Pantons)
    {
        FVector Point = GetOwner()->GetTransform().TransformPosition(Panton);
        ApplyForceToPoint(Root, Point + FVector(0, 0, -Prediction), SpringConstant / Pantons.Num(), DeltaTime);
    }
}

void UStableBuoyancyComponent::ApplyForceToPoint(UPrimitiveComponent *Root, FVector Point, float BaseForce, float DeltaTime)
{

    auto DeltaZ = GetWaterLevel(Point) - Point.Z;
    if (DeltaZ > 0.0f)
    {
        FVector SpringForce = -FVector(0.0f, 0.0f, BaseForce) * DeltaZ * DeltaTime;

        // Calculate the damping force based on the actor's velocity
        FVector Velocity = Root->GetPhysicsLinearVelocityAtPoint(Point);
        float DampingFactor = abs(BaseForce * DragCoefficient); // Adjust the damping factor as needed
        FVector DampingForce = -DampingFactor * Velocity * DeltaTime;

        // Apply the combined force to the actor's root component
        Root->AddForceAtLocation(SpringForce + DampingForce, Point);
    }
}