// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DynamicNavSurfaceComponent.h"
#include "DSN_Ghost.h"
#include "AIController.h"
#include "DSN_MagneticBoots.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class DYNAMICSURFACENAVIGATION_API UDSN_MagneticBoots : public UActorComponent
{
	GENERATED_BODY()


public:
	// Sets default values for this component's properties
	UDSN_MagneticBoots();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void MoveTo(const FVector &Destination);

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom")
	UDynamicNavSurfaceComponent *CurrentSurface;

private:
	ACharacter *CurrentCharacter;

	UCharacterMovementComponent *MovementComponent;

	ACharacter *Ghost;

	AAIController *CurrentController;

	void OnReceiveSurface(UDynamicNavSurfaceComponent *Surface);

	void OnLooseSurface();
};
