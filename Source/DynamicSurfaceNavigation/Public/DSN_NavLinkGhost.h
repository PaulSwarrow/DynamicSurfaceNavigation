// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "DSN_Ghost.h"
#include "DSN_NavLinkGhost.generated.h"


/**
 * 
 */
UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_NavLinkGhost : public ANavLinkProxy
{

	GENERATED_BODY()
public:
    ANavLinkProxy* OriginalNavLinkProxy;

    ADSN_NavLinkGhost();
    void SmartLinkReached(UNavLinkCustomComponent* ThisComp, UObject* PathComp, const FVector& DestPoint);


    // Adding destructor
    virtual ~ADSN_NavLinkGhost()
    {
        // Unsubscribe from the event
        OnSmartLinkReached.RemoveDynamic(this, &ADSN_NavLinkGhost::HandleSmartLinkReached);
    } 

protected:
    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        // Subscribe to the event
        OnSmartLinkReached.AddDynamic(this, &ADSN_NavLinkGhost::HandleSmartLinkReached);
    }

	UFUNCTION(BlueprintCallable, Category="AI|Navigation")
    void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
    {
        // Implement your logic here
        //cast actor to ADSN_Ghost and call the function
        if (const ADSN_Ghost* Ghost = Cast<ADSN_Ghost>(MovingActor))
        {
            Ghost->OnSmartLinkReached.Broadcast(this->OriginalNavLinkProxy, DestinationPoint);
        }
        
    }

};
