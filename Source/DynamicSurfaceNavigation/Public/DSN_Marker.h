// Copyright Pagan Games, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "DynamicNavSurfaceComponent.h"
#include "DSN_Marker.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_Marker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSN_Marker();

protected:
	// The root sphere component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class USphereComponent* RootSphereComponent;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void InitializeMarker();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	AActor* SyncTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	float ProjectionRadius;

private:
	USphereComponent* TargetCollider;

	UDynamicNavSurfaceComponent* CurrentSurface;
	void SyncPosition();

	UFUNCTION()
	void HandleComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	UFUNCTION()
    void HandleComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void SetSurface(UDynamicNavSurfaceComponent* Surface);

};
