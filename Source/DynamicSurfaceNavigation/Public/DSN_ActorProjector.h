// Copyright Pagan Games, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "DynamicNavSurfaceComponent.h"
#include <unordered_map>
#include "DSN_ActorProjector.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DYNAMICSURFACENAVIGATION_API UDSN_ActorProjector : public UBoxComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UDSN_ActorProjector();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	UPROPERTY(EditAnywhere, Category = "DSN Projection", meta = (ToolTip = "minumum movement required for projection update"))
	float LinearMinStep= 0.1f;
	UPROPERTY(EditAnywhere, Category = "DSN Projection", meta = (ToolTip = "minumum rotation required for projection update"))
	float AgulatMinStep = 0.1f;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	UFUNCTION()
	void HandleComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
    void HandleComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	std::unordered_map<UDynamicNavSurfaceComponent*, AActor*> Projections;

	void SyncProjectionPosition(UDynamicNavSurfaceComponent* Surface, AActor* Projection);
		
};
