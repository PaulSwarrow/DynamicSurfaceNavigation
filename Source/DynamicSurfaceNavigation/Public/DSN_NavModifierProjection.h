// Copyright Pagan Games, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NavAreas/NavArea.h"
#include "NavModifierComponent.h"
#include "DSN_NavModifierProjection.generated.h"

UCLASS()
class DYNAMICSURFACENAVIGATION_API ADSN_NavModifierProjection : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADSN_NavModifierProjection();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY();
	UBoxComponent* BoxComponent;
	UPROPERTY();
	UNavModifierComponent* ModifierComponent;

public:	
	void Init(TSubclassOf< UNavArea > AreaClass, FVector Size);

};
