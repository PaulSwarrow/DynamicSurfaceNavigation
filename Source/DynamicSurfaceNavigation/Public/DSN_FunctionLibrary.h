// Copyright Pagan Games, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "NavigationSystem.h"
#include "DynamicNavSurfaceComponent.h"
#include "DSN_FunctionLibrary.generated.h"



USTRUCT(BlueprintType)
struct FDSN_Point
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector Location;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UDynamicNavSurfaceComponent* Surface;

	bool IsVirtual() { return Surface != nullptr; }
	
	FVector GetNavLocation() { return Location;}
	
	FVector GetWorldLocation() {return IsVirtual()? Surface->TransformPositionVirtual2World(Location) : Location; }


};

/**
 *
 */
UCLASS()
class DYNAMICSURFACENAVIGATION_API UDSN_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintCallable, Category = "DynamicSurfaceNavigation", meta=( WorldContext = "WorldContextObject" ))
	static bool DSN_FindPointInNavMesh(UObject* WorldContextObject, FVector origin, float radius, bool reachable, FDSN_Point &point);

	UFUNCTION(BlueprintCallable, Category = "DynamicSurfaceNavigation")
	static void CreateNavigationTaskData(APawn *Pawn, const FVector GoalLocation, AActor *GoalActor, bool ConvertLocation, APawn *&PawnToUse, FVector &TargetLocation, AActor *&TargetActor);

private:
	static FDSN_Point DSN_ParseLocation(UObject* WorldContextObject, FVector origin); 

};
