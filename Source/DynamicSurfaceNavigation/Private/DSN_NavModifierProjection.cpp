// Copyright Pagan Games, All rights reserved.


#include "DSN_NavModifierProjection.h"
#include "NavModifierComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADSN_NavModifierProjection::ADSN_NavModifierProjection()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MyRootComponent"));
    RootComponent = BoxComponent;

	ModifierComponent = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifierComponent"));
    //ModifierComponent = NewObject<UNavModifierComponent>(this, TEXT("NavModifierComponent"));
    //ModifierComponent->RegisterComponent();// Create and attach the UNavModifierComponent to the root component
    //UNavModifierComponent* ModifierComponent = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifierComponent"));
    //ModifierComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ADSN_NavModifierProjection::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADSN_NavModifierProjection::Init(TSubclassOf<UNavArea> AreaClass, FVector Size)
{
	ModifierComponent->AreaClass = AreaClass;
	BoxComponent->SetBoxExtent(Size);
}

