// Copyright Pagan Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DynamicNavSurfaceComponent.h"
#include "DSN_Ghost.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "DSN_MagneticBoots.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementComplete);

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Position Syncronization", meta = (Tooltip = "Offset for the lowest point of the actor. If Actor's pivot is at the floor - keep it zero."))
	FVector FeetOffset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Position Syncronization", meta = (Tooltip = "Sweep"))
	bool Sweep;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Position Syncronization", meta = (Tooltip = "Teleport Type"))
	ETeleportType TeleportType;
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DynamicSurfaceNavigation")
	UDynamicNavSurfaceComponent *CurrentSurface;
	
    UPROPERTY(BlueprintAssignable, Category = "DynamicSurfaceNavigation")
    FOnMovementComplete OnMovementCompleteEvent;

	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DynamicSurfaceNavigation")
	AAIController* GetGhostController() { return GhostController; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DynamicSurfaceNavigation")
	ACharacter* GetGhost() { return Ghost; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "DynamicSurfaceNavigation")
	bool HasDynamicSurface() { return DynamicSurfaceRegistered; }


private:
	bool DynamicSurfaceRegistered;

	ACharacter *CurrentCharacter;

	UCharacterMovementComponent *MovementComponent;

	ACharacter *Ghost;

	AAIController *GhostController;

	void OnReceiveSurface(UDynamicNavSurfaceComponent *Surface);

	void OnLooseSurface();

	FVector GetFeetPosition();

	void SetFeetPosition(FVector Position);
	
};
