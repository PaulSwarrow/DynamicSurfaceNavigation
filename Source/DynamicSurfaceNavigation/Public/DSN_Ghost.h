// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DSN_Ghost.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_Ghost : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADSN_Ghost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
};
