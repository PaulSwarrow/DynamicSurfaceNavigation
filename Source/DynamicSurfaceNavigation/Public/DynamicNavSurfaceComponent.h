// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDSNCoordTransformer.h"
#include "Components/ActorComponent.h"
#include "VirtualNavMeshArea.h"
#include "DynamicNavSurfaceComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DYNAMICSURFACENAVIGATION_API UDynamicNavSurfaceComponent : public UActorComponent, public IDSNCoordTransformer
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDynamicNavSurfaceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	AVirtualNavMeshArea* VirtualArea;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DynamicSurfaceNavigation")
	FVirtualNavMesh VirtualNavMeshData;

	// Declare a delegate to notify when MyComponent is initialized
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitialized);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnInitialized OnInitialized;

	bool GetIsInitialized() const { return IsInitialized; }

	UFUNCTION(BlueprintCallable, Category = "CoordTransformer")
	virtual FTransform TransformWorld2Virtual(FTransform WorldTransform, bool KeepUpDirection) const override;
	UFUNCTION(BlueprintCallable, Category = "CoordTransformer")
	virtual FTransform TransformVirtual2World(FTransform VirtualTransform, bool RestoreUpDirection) const override;
	FVector TransformPositionWorld2Virtual(FVector WorldPosition) const;
	FVector TransformDirectionWorld2Virtual(FVector WorldDirection) const;
	FVector TransformPositionVirtual2World(FVector VirtualPosition) const;
	FVector TransformDirectionVirtual2World(FVector VirtualDirection) const;

	FVector GetVelocityAtPosition(FVector WorldPosition) const;

	AVirtualNavMeshArea* GetVirtualArea() const { return VirtualArea; }

private:
	bool IsInitialized;
};
