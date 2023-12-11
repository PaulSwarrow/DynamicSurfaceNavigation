// Fill out your copyright notice in the Description page of Project Settings.

#include "NavLinkCustomComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "DSN_NavLinkGhost.h"

ADSN_NavLinkGhost::ADSN_NavLinkGhost()
{
	this->SetSmartLinkEnabled(true);
	bSmartLinkIsRelevant = true;
	if (GetSmartLinkComp())
	{
		GetSmartLinkComp()->SetMoveReachedLink(this, &ADSN_NavLinkGhost::SmartLinkReached);
	}
}


void ADSN_NavLinkGhost::SmartLinkReached(UNavLinkCustomComponent* ThisComp, UObject* PathComp, const FVector& DestPoint)
{
	if (PathComp)
	{
		UPathFollowingComponent* PathFollowingComp = Cast<UPathFollowingComponent>(PathComp);
		if (PathFollowingComp)
		{
			AController* ControllerOwner = Cast<AController>(PathFollowingComp->GetOwner());
			if (ControllerOwner)
			{
				AActor* Actor = ControllerOwner->GetPawn();
				if (Actor)
				{
					if (const ADSN_Ghost* Ghost = Cast<ADSN_Ghost>(Actor))
					{
						Ghost->OnSmartLinkReached.Broadcast(this->OriginalNavLinkProxy, DestPoint);
					}
					// Now you have the actor that reached the smart link
					// Add your custom logic here
				}
			}
		}
	}
}