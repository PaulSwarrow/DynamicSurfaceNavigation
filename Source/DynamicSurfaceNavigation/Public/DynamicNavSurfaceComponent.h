// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VirtualNavMeshArea.h"
#include "DynamicNavSurfaceComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DYNAMICSURFACENAVIGATION_API UDynamicNavSurfaceComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this component's properties
	UDynamicNavSurfaceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DynamicSurfaceNavigation")
	FVirtualNavMesh VirtualNavMeshData;

	FTransform TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection) const;
	FTransform TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection) const;
	FVector TransformPositionWorld2Virtual(FVector WorldPosition) const;
	FVector TransformDirectionWorld2Virtual(FVector WorldDirection) const;
	FVector TransformPositionVirtual2World(FVector VirtualPosition) const;
	FVector TransformDirectionVirtual2World(FVector VirtualDirection) const;

	FVector GetVelocityAtPosition(FVector WorldPosition) const;

};
