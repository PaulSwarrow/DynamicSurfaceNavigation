// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include <Vector>
#include "DSN_ActorProjection.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_ActorProjection : public AActor
{
	GENERATED_BODY()


public:
	// Sets default values for this actor's properties
	ADSN_ActorProjection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void Init(AActor *originalActor);


private:
	void AddStaticMeshComponent(UStaticMeshComponent *StaticMeshComponent, FTransform ActorTransform);

	bool IsRootComponent(UStaticMeshComponent *StaticMeshComponent);

	bool bInitialized;
};