// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VirtualSurfaceActor.h"
#include <unordered_set>
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
	AVirtualSurfaceActor *VirtualSurfaceActor;

	FTransform TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection);
	FTransform TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection);
	FVector TransformPositionWorld2Virtual(FVector WorldPosition);
	FVector TransformDirectionWorld2Virtual(FVector WorldDirection);
	FVector TransformDirectionVirtual2World(FVector VirtualDirection);

	FVector GetVelocityAtPosition(FVector WorldPosition);
	
	static std::unordered_set<UDynamicNavSurfaceComponent*> RegisterQueue;
	static std::unordered_set<UDynamicNavSurfaceComponent*> UnregisterQueue;

private:
	void LazyInit();
	bool bInitialized;
};
