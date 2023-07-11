// Copyright Pagan Games. All rights reserved.

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

	UFUNCTION(BlueprintCallable, Category = "DynamicSurfaceNavigation")
	void MoveTo(const FVector &Destination);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DynamicSurfaceNavigation", meta = (Tooltip = "Offset for the lowest point of the actor. If Actor's pivot is at the floor - keep it zero."))
	FVector FeetOffset;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Custom")
	UDynamicNavSurfaceComponent *CurrentSurface;

private:
	ACharacter *CurrentCharacter;

	UCharacterMovementComponent *MovementComponent;

	ACharacter *Ghost;

	AAIController *CurrentController;

	void OnReceiveSurface(UDynamicNavSurfaceComponent *Surface);

	void OnLooseSurface();

	FVector GetFeetPosition();

	void SetFeetPosition(FVector Position);
};
