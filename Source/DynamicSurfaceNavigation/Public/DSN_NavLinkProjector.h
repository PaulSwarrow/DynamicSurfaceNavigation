	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DSN_NavLinkGhost.h"
#include "DynamicNavSurfaceComponent.h"
#include "GameFramework/Actor.h"
#include "DSN_NavLinkProjector.generated.h"


UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_NavLinkProjector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSN_NavLinkProjector();
	
	// Points A and B
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
	FVector PointA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Navigation")
	FVector PointB;

	void ProjectTo(FTransform Origin, FTransform Target);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	ADSN_NavLinkGhost* GhostLink;
	
	// Function that can be overridden in Blueprints
	UFUNCTION(BlueprintNativeEvent, Category = "Navigation")
	void OnNavLinkEnter(AActor* Agent);

	UFUNCTION(BlueprintCallable, Category="Navigation")
	void ExitNavLink(AActor* Agent);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category="Navigation")
	void HandleSmartLinkReached(AActor* Actor, const FVector& Vector);
	void ProjectNavLink(ADSN_NavLinkProjector* Projector, FTransform ActorTransform);

private:
	// Function to draw gizmos
	void DrawGizmos();
	bool TryFindParent(AActor* StartActor, UDynamicNavSurfaceComponent*& Surface);

};
