// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VirtualNavMeshArea.h"
#include "VirtualSurfaceActor.h"
#include "DynamicNavSurfaceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DYNAMICSURFACENAVIGATION_API UDynamicNavSurfaceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDynamicNavSurfaceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	int ownIndex;

	UPROPERTY(EditAnywhere)
	AVirtualNavMeshArea* VirtualArea;

	AVirtualSurfaceActor* VirtualSurfaceActor;

	 FTransform TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection) const;
	 FTransform TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection)const;
	 FVector TransformPositionWorld2Virtual(FVector WorldPosition) const;
	 FVector TransformDirectionWorld2Virtual(FVector WorldDirection) const;
	 FVector TransformDirectionVirtual2World(FVector VirtualDirection) const;

	 FVector GetVelocityAtPosition(FVector WorldPosition) const;
		
};
