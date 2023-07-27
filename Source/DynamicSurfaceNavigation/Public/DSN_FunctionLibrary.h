// Copyright Pagan Games, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "DSN_FunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class DYNAMICSURFACENAVIGATION_API UDSN_FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DynamicSurfaceNavigation")
	static void CreateNavigationTaskData(APawn *Pawn, const FVector GoalLocation, AActor *GoalActor, APawn *&PawnToUse, FVector &TargetLocation, AActor *&TargetActor);
};
