// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VirtualNavMeshArea.h"
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

	int FindUniqueIndex();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Init(AActor *realSurfaceActor);

	static std::vector<int> VirtualSurfaces;

	int ownIndex;

	UPROPERTY(EditAnywhere)
	AVirtualNavMeshArea *VirtualArea;

private:
	void AddStaticMeshComponent(UStaticMeshComponent *StaticMeshComponent, FTransform ActorTransform);

	bool IsRootComponent(UStaticMeshComponent *StaticMeshComponent);
};