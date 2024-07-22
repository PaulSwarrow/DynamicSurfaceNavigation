// Fill out your copyright notice in the Description page of Project Settings.

#include "DSN_NavLinkGhost.h"
#include "NavLinkCustomComponent.h"
#include "Navigation/PathFollowingComponent.h"

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
					OnSmartLinkReachedEvent.Broadcast(Actor, DestPoint);					
				}
			}
		}
	}
}