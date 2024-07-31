// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IDSNProjectableChild.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UDSNProjectableChild : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DYNAMICSURFACENAVIGATION_API IDSNProjectableChild
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ProjectableChild")
	void ProjectTo(const FTransform& Origin, const FTransform& Target, AActor* Parent);
};
