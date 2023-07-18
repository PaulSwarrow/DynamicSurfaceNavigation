// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include <Vector>
#include "VirtualSurfaceActor.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API AVirtualSurfaceActor : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	AVirtualSurfaceActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void Init(AActor *realSurfaceActor, FIntVector Volume, FIntVector Coord);

	FIntVector GetVolume() { return ActorVolume; };
	FIntVector GetCoord() { return ActorCoord; };

private:
	void AddStaticMeshComponent(UStaticMeshComponent *StaticMeshComponent, FTransform ActorTransform);

	bool IsRootComponent(UStaticMeshComponent *StaticMeshComponent);

	bool bInitialized;
	FIntVector ActorVolume;
	FIntVector ActorCoord;
};